# aws-iot-sensors
This is a repository containing various parts required to build an end to end setup for mqtt sensors based on aws IoT Core, DynamoDB and Flask as part of my MSC in Computer Science @ La Sapienza.

In order to reproduce the experiments have a look at the articles

### Part 1 - Building the IoT System Core
In the first part I sat up a network in which sensors will communicate directly to AWS IoT core using TLS and certificates.
Video:  https://www.youtube.com/watch?v=6vRpnfMh4z4  
Article: https://medium.com/@diego.sonaglia/build-an-iot-system-with-arduino-aws-and-flask-f3b787f263d5   

![Arch Core](https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-core.png)

### Part 2 - Adding the mqtt(sn) edges
In this second part I'm going to add a middleware composed of a mqtt bridge and a mqttsn gateway, in order to reduce the number of certificates to handle.

Video: https://youtu.be/qZdXz5S78Q4   
Article: https://medium.com/@diego.sonaglia/riot-os-and-mqttsn-for-aws-iot-62b95b9b6fca    

![aws-iot-mqttsn](https://di3go-article-images.s3.eu-central-1.amazonaws.com/uPic/aws-iot-mqttsn.png)
