#! /bin/bash

while :
do
temp=$(echo "scale=8; $RANDOM/32768*100-50" | bc)
hum=$(echo "scale=8; $RANDOM/32768*100" | bc)
wind_dir=$(echo "scale=8; $RANDOM/32768*360" | bc)
wind=$(echo "scale=8; $RANDOM/32768*100" | bc)
rain=$(echo "scale=8; $RANDOM/32768*50" | bc)

if [ -z "$AWS_HOST" ]
then
	echo "you need to set AWS_HOST variable first" 
	exit 1
fi

mosquitto_pub -d -h $AWS_HOST   \
              -t "\$aws/things/test/shadow/update" -p 8883        \
              --cafile certs/amazon-ca.pem.crt                          \
              --cert certs/certificate.pem.crt                          \
              --key certs/private.pem.key                               \
              -m  "{ \"state\": {\"desired\":{\"temperature\":$temp, \"humidity\":$hum, \"wind_direction\":$wind_dir, \"wind\":$wind,\"rain\":$rain} }}"  \
              -q 1

sleep 180
done