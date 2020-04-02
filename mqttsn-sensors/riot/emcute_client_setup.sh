#! /bin/bash
ipv6_broker="fd00:dead::100"
ipv6_emcute="fd00:dead::99"
emcute_bin="./$1/bin/native/app.elf"
topic='\$aws/things/riot-test-1/shadow/update'
topic2='\$aws/things/riot-test-2/shadow/update'
topic_r='\$aws/things/riot-test-1/shadow/update'
topic_r2='\$aws/things/riot-test-2/shadow/update'
tap="tap0"

if [[ $2 == "2" ]] ; then
	topic="$topic2"
	tap="tap1"
fi
if [[ $2 == "r" ]] ; then
	topic="$topic_r"	
fi
if [[ $2 == "r2" ]] ; then
	topic="$topic_r2"
	tap="tap1"
fi

echo "$emcute_bin" 

if [[ -z "$1" ]]; then
	echo "select the emcute folder"
	exit
fi

if [[ -z "$( ifconfig tapbr0)" ]]; then
	sudo riot/RIOT-src/dist/tools/tapsetup/tapsetup -c 2
fi

if [[ -z "$( ifconfig tapbr0 | grep "$ipv6_broker")" ]]; then
	sudo ip a a $ipv6_broker/64 dev tapbr0
fi


cd $1 &&
make &&
cd ../

cmds="
set timeout -1
spawn  $emcute_bin  $tap
expect \"> \"
send   \"ifconfig 5 add $ipv6_emcute \r \" 
send   \"rand_pub $ipv6_broker 10000 $topic \r \" 
send   \"con $ipv6_broker 10000 \r \"
send   \"sub /test/ \r \" 
send   \"pub /test/ hey man! \r \" 
send  	\r
interact "

# cmds="
# set timeout -1
# spawn  $emcute_bin  $tap
# expect \"> \"
# send   \"ifconfig 5 add $ipv6_emcute \r \" 
# send   \"rand_pub $ipv6_broker 10000 $topic \r \" 
# send   \"con $ipv6_broker 10000 \r \"
# send   \"sub /test/ \r \" 
# send   \"pub /test/ hey man! \r \" 
# send  	\r
# interact "

# cmds="
# set timeout -1
# spawn  $emcute_bin
# expect \"> \"
# send   \"ifconfig 5 add $ipv6_emcute \r \" 
# expect \" *success:* \"
# send   \"con $ipv6_gateway \r \" 
# expect \" *connected*  \"
# send   \"sub /test/ \r \" 
# expect \" Successfully* \"
# send   \"pub /test/ hey man! \r \" 
# expect \"> \"
# interact "


echo "==== Starting board setup  "
expect -c "${cmds//
/;}"
