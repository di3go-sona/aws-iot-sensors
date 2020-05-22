from flask import Flask, request, abort
import paho.mqtt.client as mqtt
import json
mqtt_client = mqtt.Client()

app = Flask(__name__,static_folder='static/')

@app.route('/')
def root():
    print('hola')
    return app.send_static_file('index.html')


@app.route('/walk-sensor/raw', methods = ['GET', 'POST', 'DELETE'])
def walking_sensor_raw():
    if request.method == 'POST':
        mqtt_client.connect('localhost', 1883)
        """modify/update the information for <user_id>"""
        # you can use <user_id>, which is a str but could
        # changed to be int or whatever you want, along
        # with your lxml knowledge to make the required
        # changes
        data = request.get_data()
        arr = json.loads(data)
        avg = sum(arr)/len(arr)
        walking = 0
        msg = {"state":{"desired":{"acc_avg":avg, "walking":avg>1.5 }}}
        mqtt_client.publish('$aws/things/walk-sensor-raw/shadow/update', json.dumps(msg))
        mqtt_client.disconnect()


        print(data)
        return 'OK'
    else:
        return 'OK'

@app.route('/walk-sensor/proc', methods = ['GET', 'POST'])
def walking_sensor_proc():
    if request.method == 'POST':
        mqtt_client.connect('localhost', 1883)
        """modify/update the information for <user_id>"""
        # you can use <user_id>, which is a str but could
        # changed to be int or whatever you want, along
        # with your lxml knowledge to make the required
        # changes
        data = request.get_data()
        data = json.loads(data)
        msg = {"state":{"desired":{"walking":data}}}
        mqtt_client.publish('$aws/things/walk-sensor-proc/shadow/update', json.dumps(msg) )
        mqtt_client.disconnect()
        return 'OK'
    else:
        return 'OK'

@app.after_request
def add_header(r):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    r.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    r.headers["Pragma"] = "no-cache"
    r.headers["Expires"] = "0"
    r.headers['Cache-Control'] = 'public, max-age=0'
    return r

if __name__ == '__main__':
    app.run('0.0.0.0', 443, ssl_context=('server.crt', 'server.key'))
