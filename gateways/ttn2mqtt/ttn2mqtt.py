#! /bin/python3
import time
import ttn
from base64 import b64decode
import paho.mqtt.client as mqtt

app_id = "foobar"
access_key = "ttn-account-v2.some_numbers"

# overrides dummy secrets
from secrets import *

def make_aws_payload(msg):
	dev_id = msg.dev_id
	payload = msg.payload_raw
	data = b64decode(payload).decode('ascii')
	measurements = [ int(m) for m in data.strip().split(',') ]
	h,t,r,w,wd = measurements

	topic = f"$aws/things/{dev_id}/shadow/update"

	aws_payload = {
		'state' : {
			'desired' : {
				'temperature' : t,
				'humidity' : h,
				'rand' : r,
				'wind' : w,
				'wind_direction' : wd
			}

		}
	}

	print(dev_id, payload, data, measurements)
	print(str(aws_payload))
	return  str(aws_payload), topic
	
	

def send_mqtt_msg(payload, topic):
	client = mqtt.Client()
	client.connect_srv("localhost", port=1883)
	client.publish(topic, payload=payload, qos=0, retain=False)
	client.disconnect()

def uplink_callback(msg, client):
    print("Received uplink from ", msg.dev_id)
    # print(msg)
    payload, topic = make_aws_payload(msg)

handler = ttn.HandlerClient(app_id, access_key)

# using mqtt client
mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
time.sleep(60)
mqtt_client.close()

# using application manager client
# app_client =  handler.application()
# my_app = app_client.get()
# print(my_app)
# my_devices = app_client.devices()
# print(my_devices)