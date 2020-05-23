# aws-iot-sensors
This is a repository containing various parts required to build an end to end setup for mqtt sensors based on aws IoT Core, DynamoDB and Flask as part of my MSC in Computer Science @ La Sapienza.

The system is composed of 4 main parts:
  - AWS Iot Backend
  - MQTT/MQTT-SN gateways
  - Sensors
  - Dashboard

Datas are collected from the sensors, they may be located everywhere, but they should have connectivity to reach at least a gateway and communicate the measurements via mqtt.

AWS backend only allows secure connection with TSL, therefore we may want to use gateways, these connects to the iot backend using certificates and forwards all mqtt and mqttsn messages, accepting also unsecure connections.

### AWS Iot Backend
Here you can have a quick glance of the architecture of the system.  

<img src="https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-core.png" alt="AWS Iot Backend" height="400px">

In order to setup the backend you need to create an amazon AWS account and get the address of the IoT mqtt broker, it has the following format `your-numbers-ats.iot.aws-zone.amazonaws.com`.

After you may want to setup a DynamoDB database table and create a new rule that automatically saves mqtt messages to the DB. ( have a look at the first article for more detailed informations)



### Gateways
<img src="https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-mqttsn.png" alt="AWS Iot Backend" height="400px">
The gateway consists of various parts, added in the 2nd, 3rd and 4th part, in order to connect heterogeneous architecture to the same backend.

##### MQTT bridge
This part is implemented with mosquitto and has the role of accepting insecure connection on the local network and forward them to the aws broker. In order to set it up you need to create a policy with access to all the aws topics you want to forward from this node.
Then you need to download and install the aws cli tool on the machine on which you want to set it up and run the `setup.sh` script you can find in the gateways folder.

It will cretae a new certificate and register it to AWS, the only manual part you should do is going to AWS, locate the newly created certificate, and attach all the things that we want to forward from this local bridge to that certificate.

##### MQTT-SN gateway
This component will assume that there is an mqtt broker running on the deployment host listening for connections on port `1883` so you have two chances:
- Deploy an MQTT bridge on the host where the mqttsn gateway will be deployed ( you can simply decomment the corresponding line in `setup.sh` )
- Use ssh port forwarding ( or any similiar tool) in order to redirect port `1883` of the localhost to port `1883` of the mqtt bridge and use the `./mqttsn.sh` script. ( have a look at the 2nd article for more detailed informations )

You don't need to setup any host or certificate, you can simply run it with the `gateways/mqtt-sn.sh` script you find in the repository

##### Websensor gateway
This component was added in the fourth part: it is basically a flask web server that is serving an HTML static page and listens for incoming measurements on two endpoints.
This is not fired automatically by `gateways/setup.sh` but can be launched as simply as 
```
$ python3 app.py  
```


### Sensors 
Sensors can be built using different technologies, in this repo there are examples written in Arduino, Riot-Os and Bash but they are all gathered by the fact that they all rely mostly over mqtt or mqttsn for communication of them.
- In the first part you can find how to directly connect to an aws broker with arduino or mosquitto. ( And using real sensors as well )
- In the second part there is an example of RioT mqttsn client using emcute and virtual random sensors
- In the third part you can find a LoRa client written using Riot-Os and virtual random sensors
- In the four part you will find a mobile websensor, in particular the accelerometer one ( this time the code is not contained on the sensor folder but in the gateways/websensor )


### Dashboard
The dashboard is a simple flask app that retrieves data from DynamoDB using boto3 ( a python library to interact with AWS services ). This library is one of the upsides of working with amazon, you can simply configure `aws cli` and boto3 will connect using its credentials. (here you can find out more about installing  aws-cli https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-install.html)

In order to run the server one may simply go to  `dashboard/` and run
```
$ python3 app.py [hostname] 
```
additionally we can specify a second argument, that is the address on which the socket is bind, by thefault it's localhost, meaning that we can only access it from the computer on which it is running, if we want to make it accessible from the outside one may specify a global address rather than a local one


### Videos and Articles
##### Part 1 - Building the IoT System Core
In the first part I will explain how to setup the AWS Iot Backend, the dashboard and two sensors that directly connect to the backend using TLS certificates.  
Video:  https://www.youtube.com/watch?v=6vRpnfMh4z4  
Article: https://medium.com/@diego.sonaglia/build-an-iot-system-with-arduino-aws-and-flask-f3b787f263d5   

##### Part 2 - Adding the mqtt(sn) edges
The second part is about how to add a middleware composed of an mqtt bridge and an mqttsn gateway, in order to reduce the number of certificates to handle and the complexity of the sensors. and how to realize a simple mqtt/sn client based on Riot-OS and integrate it with the preexisting environment.

Video: https://youtu.be/qZdXz5S78Q4   
Article: https://medium.com/@diego.sonaglia/riot-os-and-mqttsn-for-aws-iot-62b95b9b6fca

##### Part 3 - Integratind LoRa with TTN
In the third part we were asked to intagrate sensors communicating over LoRa into the main architecture, using TheThingsNetwork as LoraWan gateway.

Video: https://youtu.be/17uHJPXGsOA    
Article: https://medium.com/@diego.sonaglia/integrate-lorawan-in-your-iot-system-8c793404c756

##### Part 4 - CrowdSensing
In the last part we were asked to use web sensors in order to detect if the holder of a smartphone is walking or not, and display the results using the preexistening infrastructure.

Video: https://youtu.be/s9QwHSHgmMc    
Article: https://medium.com/@diego.sonaglia/detecting-the-walking-frequency-iot-websensors-ea9044e1430c



