if [[ -z $(which mosquitto) ]]; then
	sudo apt install mosquitto
fi

killall mosquitto
mkdir /etc
mkdir /etc/mosquitto
sudo chown -R $(whoami) /etc/mosquitto
cp bridge.conf /etc/mosquitto
cp -r ../certs /etc/mosquitto/


mosquitto -c /etc/mosquitto/bridge.conf 