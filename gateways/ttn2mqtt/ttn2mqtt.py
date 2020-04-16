#! /usr/bin/python3
import time
import ttn
from base64 import b64decode
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

app_id = "foobar"
access_key = "ttn-account-v2.some_numbers"

# overrides dummy secrets
from secrets import *

def parse_ttn_msg(msg):
	dev_id = msg.dev_id
	payload = msg.payload_raw
	data = b64decode(payload).decode('ascii')
	measurements = [ int(m) for m in data.strip().split(',') ]
	h,t,r,w,wd = measurements

	topic = f"$aws/things/{dev_id}/shadow/update"

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
	
	print (f'mqtt msg sending on topic {topic}')
	print(payload)

	publish.single(topic, payload, qos=0, retain=False, hostname="localhost",
    port=1883, client_id="ttnbridge", keepalive=60, will=None, auth=None, tls=None,
    protocol=mqtt.MQTTv311, transport="tcp")

	print ('mqtt msg sent')

def uplink_callback(msg, client):
    print("Received uplink from ", msg.dev_id)
    print(msg)
    payload, topic = parse_ttn_msg(msg)
    send_aws_msg(payload,topic)


print(app_id)
ttn_handler = ttn.HandlerClient(app_id, access_key)

ttn_mqtt_client = ttn_handler.data()
ttn_mqtt_client.set_uplink_callback(uplink_callback)
ttn_mqtt_client.connect()


publish.single('test', 'hola', qos=0, retain=False, hostname="localhost",
    port=1883, client_id="ttnbridge", keepalive=60, will=None, auth=None, tls=None,
    protocol=mqtt.MQTTv311, transport="tcp")



time.sleep(60)
ttn_mqtt_client.close()
