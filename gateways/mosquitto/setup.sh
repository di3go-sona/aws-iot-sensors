if [[ -z $(which mosquitto) ]]; then
	sudo apt install mosquitto
fi

killall mosquitto
mkdir /etc
mkdir /etc/mosquitto
sudo chown -R $(whoami) /etc/mosquitto
cat bridge.conf | cat bridge.conf | sed -e "s/[\*][\*]redacted[\*][\*]/$AWS_HOST/g" > /etc/mosquitto/bridge.conf
cp -rf ../certs /etc/mosquitto/


mosquitto -c /etc/mosquitto/bridge.conf 