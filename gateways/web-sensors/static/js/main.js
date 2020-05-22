// Setting various global variables in order to keep
// track of browser state
var sampling=false;
var sending=false;
var pinging=false;
var moving = false
// We are going to store a fixed number of sampling at any time
var buffer_size=128
// ios and android have different
// meters of time (s for ios, ms for android)
var device_unit = 1
var gateway_status = 'OFFLINE';
var threshold = 1.5


var send_interval;
var ping_interval;

// keep track of the samplings done
var ticks=0;
// As the sampling rate is fixed, the only thing
// we can do is to skip some samplings in order
// to simulate a lower sampling rate
var tick_skip=1;
var sensor_interval;
var sensor_freq;

var sampling_freq;
var sampling_interval;

// buffers to store various measurements
var data_x = []
var data_y = []
var data_z = []
var data_raw = []
var data_sum = []
var data_freq = []
var walking_freq_mag;
var x,y,z,t,s;


function motion_handler(event) {

  if ( ticks % tick_skip == 0 ){
    // retrieve measurements and derive required values
    x = event.acceleration.x
    y = event.acceleration.y
    z = event.acceleration.z
    sensor_interval = event.interval * device_unit
    sensor_freq = 1/sensor_interval
    sampling_freq = sensor_freq / tick_skip
    sampling_interval = 1/sampling_freq
    t = ticks*sampling_interval
    s = x + y + z

    // push data into the buffers
    data_x.push({x: t, y:Math.abs(x)})
    data_y.push({x: t, y:Math.abs(y)})
    data_z.push({x: t, y:Math.abs(z)})
    data_raw.push(s)
    data_sum.push({x: t, y:Math.abs(s)})

    // if our buffer is full start to elaborate the frequency
    // histogram with fast fourier transform using
    // dspjs : https://github.com/corbanbrook/dsp.js/
    if (ticks > buffer_size ){

      var signal = new Float32Array(data_raw);
      var fft = new FFT(buffer_size,sampling_freq);
      fft.forward(signal);
      data_freq.length = 0; //fast way to empty an array
      for (i=0 ; i < fft.spectrum.length/3; i++) {
        if (data_freq.length >= buffer_size){
          data_freq.shift()
        }
        // fill the buffer with the histogram values i.e the pair (freq,magnitude)
        // for each freq from 0 to sampling_freq/(buffer_size*2) with the granularity of
        // buffer_size
        data_freq.push({x: i*sampling_freq/buffer_size, y: 2*Math.log(fft.spectrum[i])})

      }
      // locate the frequency bar of the spectrum corresponding to
      walking_freq_mag = fft.spectrum[Math.round(buffer_size/sampling_freq*1.8)]

      if (walking_freq_mag > threshold){
        moving = true
      } else {
        moving = false
      }
      // console.log(data_freq)

    }

    // updates the gui
    update()

    // if buffers are full pop out the oldest element
    if (data_x.length >= buffer_size){
      data_x.shift()
    }
    if (data_y.length >= buffer_size){
      data_y.shift()
    }
    if (data_z.length >= buffer_size){
      data_z.shift()
    }
    if (data_raw.length >= buffer_size ){
      data_raw.shift()
    }
    if (data_sum.length >= buffer_size){
      data_sum.shift()
    }
  }
  // increase tick count
  ticks += 1;

}


// starts accelerometer handler and updates gui
function motion_handler_start() {
  var button = $('#permission-button')
  button.addClass('active')
  window.addEventListener('devicemotion', motion_handler)
  render()
}
// stops accelerometer handler and updates gui
function motion_handler_stop() {
  var button = $('#permission-button')
  button.removeClass('active')
  window.removeEventListener('devicemotion',motion_handler)
}
// functions that queries the js engine to access the
// devicemotion event
function motion_handler_req() {
  if (typeof DeviceMotionEvent.requestPermission === 'function') {
    DeviceMotionEvent.requestPermission()
      .then(permissionState => {
        if (permissionState === 'granted') {
          console.log("Permission Granted")
        } else {
          device_unit = 1000
          }}).catch(console.error);
  }
}

// pings the iot gateway in order to check
// if it's online and reachable
function ping_iot_gateway(){

  console.log('ping_iot')
  xhttp =new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200){
        // alert(this.responseText == "OK")
        gateway_status = "ONLINE"}
      else
      {
        gateway_status = "OFFLINE"
      }
  }
  xhttp.open("GET", 'walk-sensor/raw', true);
  xhttp.send();

}

// sends raw data, consisting of the current raw data buffer
function send_raw_data(){

  console.log('send raw data')
  xhttp =new XMLHttpRequest();
  // xhttp.setRequestHeader("Content-type", "application/json");
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // alert(this.responseText == "OK")
      sending = "RAW"
    }
      else
      {
        sending = "ERR"
      }
  };

  xhttp.open("POST", 'walk-sensor/raw', true);
  xhttp.send( JSON.stringify(data_raw));



  // setTimeout(ping_iot_gateway,1000)
}
// sends processed data
function send_proc_data(){

  console.log('send proc data')
  xhttp =new XMLHttpRequest();
  // xhttp.setRequestHeader("Content-type", "application/json");
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // alert(this.responseText == "OK")
      sending = "PROC"
    }
      else
      {
        sending="ERR"
      }
  };
  xhttp.open("POST", 'walk-sensor/proc', true);
  xhttp.send(JSON.stringify(moving));
  console.log(JSON.stringify(moving))


  // setTimeout(ping_iot_gateway,1000)
}

// button handlers
function ping_button_handler() {

  if (!pinging) {
    pinging = true
    ping_interval = setInterval(ping_iot_gateway, 1000)
  } else {
    clearInterval(ping_interval)
    pinging=false
    gateway_status = "OFFLINE"
  }
}
function sendraw_button_handler() {

  if (!sending) {
    sending = true
    clearInterval(send_interval)
    send_interval = setInterval(send_raw_data, 1000)
  } else {

    clearInterval(send_interval)
    sending=false
  }
}
function sendproc_button_handler() {

  if (!sending) {
    sending = true
    clearInterval(send_interval)
    send_interval = setInterval(send_proc_data, 1000)
  } else {
    setInterval(send_interval)
    sending=false
  }
}
function sensor_button_handler() {
  console.log('click_handler')

  // feature detect
  if (ticks == 0){
    motion_handler_req()
  }

  if ( ! sampling) {
    console.log('starting')
    motion_handler_start();
    sampling = true;
  } else{
    console.log('stopping')
    motion_handler_stop()
    sampling = false;
  }
}

var graph1 = new Rickshaw.Graph( {
  element: document.querySelector('#chart1'),
  renderer:'area',
  height: 200,
  series: [
    {
      color: 'steelblue',
      data: data_x
    }, {
      color: 'red',
      data: data_y
    },{
      color: 'green',
      data: data_z
    }
  ],
  max :40,
  padding:{top:0 , right:0.03 , bottom:0 , left:0.03}
} );
var graph2 = new Rickshaw.Graph( {
  element: document.querySelector('#chart1'),
  renderer:'area',
  height: 200,
  series: [
    {
      color: 'steelblue',
      data: data_sum
    }],
  max :40,
  padding:{top:0 , right:0.03 , bottom:0 , left:0.03}
} );
var graph3 = new Rickshaw.Graph( {
  element: document.querySelector('#chart3'),
  renderer:'area',
  height: 200,
  series: [
    {
      color: 'steelblue',
      data: data_freq
    }],
  max :15,
  padding:{top:0 , right:0.03 , bottom:0 , left:0.03}
} );




// initial render of the gui with rickshaw
// code @ https://github.com/shutterstock/rickshaw
function render(){
  graph1.render();
  graph2.render();
  var x_axis_1 = new Rickshaw.Graph.Axis.X( {
    graph: graph1,
    orientation: 'bottom',
    tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
  } );
  var x_axis_2 = new Rickshaw.Graph.Axis.X( {
    graph: graph2,
    orientation: 'bottom',
    tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
  } );
  var x_axis_3 = new Rickshaw.Graph.Axis.X( {
    graph: graph3,
    orientation: 'bottom',
    tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
  } );
  x_axis_1.render();
  x_axis_2.render();
  x_axis_3.render();
  graph3.render();
}
// gui update
function update(){
  graph1.update();
  graph2.update();
  graph3.update();
  $("#sensor_freq").html("" + (sensor_freq)+" Hz")
  $("#sampling_freq").html("" + (sampling_freq)+" Hz")
  $("#walking_mag").html("" + (walking_freq_mag)+"")
  $("#sending").html("" + (sending)+"")
  $("#gateway_status").html("" + (gateway_status)+"")

  if ( sending){
    $("#sending").html("" + ("<span class='green'>Sending</span>")+"")
  } else {
    $("#sending").html("" + ("<span class='red'>Not Sending</span>")+"")
  }
  if ( ! moving){
    $("#activity").html("" + ("<span class='green'>Idle</span>")+"")
  } else {
    $("#activity").html("" + ("<span class='red'>Moving</span>")+"")
  }




  $("#acc-x").html(x)
  $("#acc-y").html(y)
  $("#acc-z").html(z)



}

// Button to ask permission for using sensor ( required by ios )
$('#permission-button').on('click', sensor_button_handler)
// Button to start ping the web sensors gateway
$('#ping-button').on('click', ping_button_handler)
// Button to start sending raw data to gateway
$('#send-raw-button').on('click', sendraw_button_handler)
// Button to start sending proc data to gateway
$('#send-proc-button').on('click', sendproc_button_handler)








