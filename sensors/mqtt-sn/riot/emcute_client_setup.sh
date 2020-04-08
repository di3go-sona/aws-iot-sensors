#! /bin/bash
# This script automatically compiles a riot sketch
# sets up a tap interface
# launches the script 

ipv6_broker="fd00:dead::100"
ipv6_emcute="fd00:dead::101"
emcute_bin_name="$(basename -- $1)"
emcute_bin="./$1/bin/native/$emcute_bin_name.elf"

topic='\$aws/things/riot-test-1/shadow/update'
topic2='\$aws/things/riot-test-2/shadow/update'
tap="tap0"

# choose between 2 test topics
if [[ $2 == "2" ]] ; then
	topic="$topic2"
	tap="tap1"

fi
# if not present download RIOT repo
if [[ ! -f "RIOT-src" ]] ; then
	git clone https://github.com/RIOT-OS/RIOT.git RIOT-src
fi

# check that user specified a folder
if [[ -z "$1" ]]; then
	echo "usage: $0 <emcute-app-folder>"
	exit
fi

# set up tap interfaces
if [[ -z "$( ifconfig | grep tapbr0)" ]]; then
	sudo ./RIOT-src/dist/tools/tapsetup/tapsetup -c 2
fi

if [[ -z "$( ifconfig tapbr0 | grep "$ipv6_broker")" ]]; then
	sudo ip a a $ipv6_broker/64 dev tapbr0
fi


# compile the riot program
(cd $1 && make)



# use expect tool to do preliminary configs
# try to use the rand_pub command 
# otherwise manually send a single test message on /test topic \r
cmds="
set timeout -1
spawn  $emcute_bin  $tap
expect \"> \"
send   \"ifconfig 5 add $ipv6_emcute \r \" 
send   \"rand_pub $ipv6_broker 10000 $topic \r \" 
send   \"con $ipv6_broker 10000 \r \"
send   \"sub /test \r \" 
send   \"pub /test hey-man! \r \" 
send  	\r
interact "

echo "==== Starting board setup  "
expect -c "${cmds//
/;}"
