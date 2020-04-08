# aws-iot-sensors
This is a repository containing various parts required to build an end to end setup for mqtt sensors based on aws IoT Core, DynamoDB and Flask as part of my MSC in Computer Science @ La Sapienza.

The system is composed of 4 main parts:
  - AWS Iot Backend
  - MQTT/MQTT-SN gateways
  - Sensors
  - Dashboard

Datas are collected from the sensors, they may be located everywhere, but they should have connectivity to reach at least a gateway and communicates the measurements via mqtt.

AWS backend only allows secure connection with TSL, therefore we use gateways, these connects to the iot backend and forwards all mqtt and mqttsn messages, accepting also unsecure connections.

### AWS Iot Backend
<img src="https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-core.png" alt="AWS Iot Backend" height="400px">

In order to setup the backend you have to create an amazon AWS account and get the address of the IoT mqtt broker, it has the following format `your-numbers-ats.iot.aws-zone.amazonaws.com`.

After you may want to setup a DynamoDB database table and create a new rule that automatically saves mqtt messages to the DB. ( have a look at the first article for more detailed informations)



### Gateways
<img src="https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-mqttsn.png" alt="AWS Iot Backend" height="400px">
The gateway consists of 2 parts:
##### MQTT bridge
This part is implemented with mosquitto, in order to set it up you need to create a policy with access to all the aws topics you want to forward from this node ( include the mqtt-sn topics) and place the certificates in the dummy folder `gateways/certs`.

After you need to copy the contents of /gateways in your setup environment ( in my case an ec2 instance) and export the name of your AWS mqtt broker host that is something like this
```
$ export AWS_HOST='your-numbers-ats.iot.eu-central-1.amazonaws.com'
```
Then you can use the `mqtt.sh` script that you find in my repo and it will download and configure mosquitto to connect to your broker and accept mqtt packets on port `1883`.
##### MQTT-SN gateway
This component will assume that there is an mqtt broker running on the deployment host listening for connections on port `1883` so you have two chances:
- Deploy an MQTT bridge on the host where the mqttsn gateway will be deployed
- Use ssh port forwarding ( or any similiar tool) in order to redirect port `1883` of the localhost to port `1883` of the mqtt bridge. ( have a look at my article for more detailed informations )
You don't need to setup any host or certificate, you can simply run it with the `mqtt-sn.sh` script you find in the repository

### Sensor 
Sensors can be built using different technologies, in this repo there are examples written in Arduino, Riot-Os and Bash but they all share the same protocols to communicate data: mqtt(sn).

### Dashboard
The dashboard is a simple flask app that retrieves data from DynamoDB using boto3 ( a python library to interact with AWS services ). This library is one of the upsides of working with amazon, you can simply configure `aws cli` and boto3 will connect using its credentials. (here you can find out more about installing  aws-cli https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-install.html)

In order to run the server one may simply go to  `dashboard/` and run
```
$ python3 flask 
```
additionally we can specify a second argument, that is the address on which the socket is bind, by thefault it's localhost, meaning that we can only access it from the computer on which it is running, if we want to make it accessible from the outside one may specify a global address rather than a local one


### Videos and Articles
##### Part 1 - Building the IoT System Core
In the first part I will explain how to setup the AWS Iot Backend, the dashboard and two sensors that directly connect to the backend using TLS certificates.  
Video:  https://www.youtube.com/watch?v=6vRpnfMh4z4  
Article: https://medium.com/@diego.sonaglia/build-an-iot-system-with-arduino-aws-and-flask-f3b787f263d5   



##### Part 2 - Adding the mqtt(sn) edges
In this second part I explain how to  add a middleware composed of a mqtt bridge and a mqttsn gateway, in order to reduce the number of certificates to handle.

Video: https://youtu.be/qZdXz5S78Q4   
Article: https://medium.com/@diego.sonaglia/riot-os-and-mqttsn-for-aws-iot-62b95b9b6fca


