#! /usr/bin/python3
import time
import ttn
from base64 import b64decode
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish


# dummy credentials
app_id = "foobar"
access_key = "ttn-account-v2.some_numbers"

# import real credentials
from secrets import *


def parse_ttn_msg(msg):
	# parse ttn messages received as a ttn message object
	# returns aws payload and topic

	dev_id = msg.dev_id
	payload = msg.payload_raw
	# this is a quite 'hacky' unpacking, is basically doing the following
	# decoding base 64 | split on ',' | convert str to int | sets h,t,r,w,wd variables
	data = b64decode(payload).decode('ascii')
	measurements = [ int(m) for m in data.strip().split(',') ]
	h,t,r,w,wd = measurements

	topic = f"$aws/things/{dev_id}/shadow/update"

	# this is the json format required by aws to update things' shadows
	aws_payload = str({
		"state" : {
			"desired" : {
				"temperature" : t,
				"humidity" : h,
				"rain" : r,
				"wind" : w,
				"wind_direction" : wd
			}

		}
	}).replace('\'','\"')

	return  aws_payload, topic
	
	

def send_aws_msg(payload, topic):
	# one line mqtt publisher to aws local bridge
	publish.single(topic, payload, qos=0, retain=False, hostname="localhost",
	port=1883, client_id="ttnbridge", keepalive=60, will=None, auth=None, tls=None,
	protocol=mqtt.MQTTv311, transport="tcp")
	print(payload)
	print ('mqtt msg sent')

def uplink_callback(msg, client):
	# callback fired when a message is published on ttn mqtt broker
	print("Received uplink from ", msg.dev_id)
	print(msg)
	payload, topic = parse_ttn_msg(msg)
	send_aws_msg(payload,topic)


print(app_id)
ttn_handler = ttn.HandlerClient(app_id, access_key)

ttn_mqtt_client = ttn_handler.data()
ttn_mqtt_client.set_uplink_callback(uplink_callback)
ttn_mqtt_client.connect()

# publish a test message
publish.single('test', 'hola', qos=0, retain=False, hostname="localhost",
	port=1883, client_id="ttnbridge", keepalive=60, will=None, auth=None, tls=None,
	protocol=mqtt.MQTTv311, transport="tcp")


# the client subrscriber is asynchronous, therefore
# if we do not sleep the application will be 
# automatically closed 
time.sleep(60*60)
ttn_mqtt_client.close()
