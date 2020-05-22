alias mosquitto=/usr/local/sbin/mosquitto
# if [[ -z $(which mosquitto) ]]; then
# 	old_dir=$PWD
# 	mkdir /tmp/mosquitto-install
# 	git clone https://github.com/eclipse/mosquitto.git /tmp/mosquitto-install/mosquitto
# 	cmake /tmp/mosquitto-install/mosquitto -Wno-dev
# 	rm -rf  /tmp/mosquitto-install
# fi

sudo killall mosquitto
su root -c 'rm -rf /etc/mosquitto certs'
sudo mkdir /etc/mosquitto
sudo chown -R $(whoami) /etc/mosquitto

echo "AWS_HOST: $AWS_HOST"
cat bridge.conf | sed -e "s/[\*][\*]redacted[\*][\*]/$AWS_HOST/g" \
				| sed -e "s/AWS_CA/$AWS_CA/g" \
				| sed -e "s/GATEWAY_CERT/$GATEWAY_CERT/g" \
				| sed -e "s/GATEWAY_KEY/$GATEWAY_KEY/g" \
				> /etc/mosquitto/bridge.conf
cp -rf ../certs /etc/mosquitto/


mosquitto -c /etc/mosquitto/bridge.conf 