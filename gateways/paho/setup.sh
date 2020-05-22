#!/bin/bash

paho_dir="src"
paho_build_dir="mqtt-sn-gateway"
rm -rf $paho_dir $paho_build_dir
mkdir "$paho_build_dir"
git clone https://github.com/eclipse/paho.mqtt-sn.embedded-c "$paho_dir"  &&
cd "$paho_dir/MQTTSNGateway" &&    
make SENSORNET=udp6 &&
make install INSTALL_DIR="../../$paho_build_dir" CONFIG_DIR="../../$paho_build_dir"  &&
make clean &&
cd "../../"
cp -f "gateway.conf" "$paho_build_dir/"

$paho_build_dir/MQTT-SNGateway
