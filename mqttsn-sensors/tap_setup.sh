#! /bin/bash

if [-z "$(ifconfig tabr0)"]
then
	sudo riot/RIOT-src/dist/tools/tapsetup/tapsetup -c 2
fi

sudo ip a a fec0:affe::1/64 dev tapbr0
gateway/broker_mqtts broker.conf &
cd riot/emcute_mqtt
sensor_bin=$(make)
echo " ifconfig 5 add fec0:affe::99" |  $sensor_bin tap0
cd ../../