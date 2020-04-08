$(document).ready(function () {
  function isNew(data){
    var sensor = data.sensor ;
    var timestamp = data.timestamp ;
    var old_timestamp = $("#" + sensor + " tbody tr td").html();
    console.log(old_timestamp.trim());
    console.log(timestamp);
    console.log(old_timestamp.trim() == String(timestamp));
    return old_timestamp.trim() == String(timestamp);
  }

  function updateTable(data){
    if ( isNew(data)){
      console.log('no update!')
      return
    }
    console.log('update!')

    var sensor = data.sensor;
    var head = $('#'+sensor+' thead tr:first');
    var row = $('#'+sensor+' tbody tr:first').clone();

    for (var i=0; i<head.children().length; i++){

      var metric = head.children()[i].innerHTML.trim();
      row.children()[i].innerHTML = (data[metric]);
      updateHeader(sensor,metric, data[metric])

    }
     $("#" + sensor + " tbody").prepend(row);
  }

  function updateHeader(sensor, metric, value){
    $("#" + sensor + " #current-" + metric).html(value)
  }

  function ajax_update(sensor){
    jQuery.getJSON(sensor+'/last',updateTable)
  }

  $('.container >  .content-tab').each( function (i, s) {
    setInterval(ajax_update, 10000, s.id )

  })


})