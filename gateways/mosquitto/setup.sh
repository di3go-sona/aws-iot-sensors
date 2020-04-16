if [[ -z $(which mosquitto) ]]; then
	sudo apt install mosquitto
fi

mkdir /etc
mkdir /etc/mosquitto
sudo killall mosquitto
sudo chown -R $(whoami) /etc/mosquitto
cat bridge.conf | cat bridge.conf | sed -e "s/[\*][\*]redacted[\*][\*]/$AWS_HOST/g" > /etc/mosquitto/bridge.conf
cp -rf ../certs /etc/mosquitto/


mosquitto -c /etc/mosquitto/bridge.conf 