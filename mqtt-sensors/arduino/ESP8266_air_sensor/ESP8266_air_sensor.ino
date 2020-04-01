#include <FS.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

#include "secrets.h"


// DHT sensor handler for
// DHT11 sensor on pin 2 ( GPIO4 on ESP8266 )
DHT dht_sensor(2, DHT11);

// Struct to handle two measurements 
// in the same function
struct measure{
  float t;
  float h;
  };

// Topic
char* topic = "$aws/things/air-sensor/shadow/update";
String clientName;
long lastReconnectAttempt = 0;
long lastMsg = 0;
int test_para = 2000;
unsigned long startMills;

WiFiClientSecure wifiClient;
PubSubClient client(mqtt_server, 8883, wifiClient);


void verifytls() {
  // Use WiFiClientSecure class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(mqtt_server);
  if (!wifiClient.connect(mqtt_server, 8883)) {
    Serial.println("connection failed");
    return;
  }

  if (wifiClient.verify(fingerprint, mqtt_server)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
}

// Load Certificates
void loadcerts() {

  if (!SPIFFS.begin()) {
   Serial.println("Failed to mount file system");
   return;
 }

 // Load client certificate file from SPIFFS
 File cert = SPIFFS.open("/certs/certificate.der.crt", "r"); //replace esp.der with your uploaded file name
 if (!cert) {
   Serial.println("Failed to open cert file");
 }
 else
   Serial.println("Success to open cert file");

 delay(1000);

 // Set client certificate
 if (wifiClient.loadCertificate(cert))
   Serial.println("cert loaded");
 else
   Serial.println("cert not loaded");

 // Load client private key file from SPIFFS
 File private_key = SPIFFS.open("/certs/private.der.key", "r"); //replace espkey.der with your uploaded file name
 if (!private_key) {
   Serial.println("Failed to open private cert file");
 }
 else
   Serial.println("Success to open private cert file");

 delay(1000);

 // Set client private key
 if (wifiClient.loadPrivateKey(private_key))
   Serial.println("private key loaded");
 else
   Serial.println("private key not loaded");


 // Load CA file from SPIFFS
 File ca = SPIFFS.open("/certs/amazon-ca.der.crt", "r"); //replace ca.der with your uploaded file name
 if (!ca) {
   Serial.println("Failed to open ca ");
 }
else
  Serial.println("Success to open ca");
  delay(1000);

  // Set server CA file
   if(wifiClient.loadCACert(ca))
   Serial.println("ca loaded");
   else
   Serial.println("ca failed");

}

void getTime(){
  // Synchronize time useing SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "de.pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 1000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  }

boolean reconnect()
{
  if (!client.connected()) {
    if (client.connect((char*) clientName.c_str())) {
        Serial.println("===> mqtt connected");
    } else {
        Serial.print("---> mqtt failed, rc=");
        Serial.println(client.state());
    }
  }
  return client.connected();
}

void wifi_connect()
{
  if (WiFi.status() != WL_CONNECTED) {
    // WIFI
      Serial.println();
      Serial.print("===> WIFI ---> Connecting to ");
      Serial.println(ssid);

      delay(10);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);


      int Attempt = 0;
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print(". ");
        Serial.print(Attempt);

        delay(100);
        Attempt++;
      if (Attempt == 150)
      {
        Serial.println();
        Serial.println("-----> Could not connect to WIFI");

        ESP.restart();
        delay(200);
      }

    }
      Serial.println();
      Serial.print("===> WiFi connected");
      Serial.print(" ------> IP address: ");
      Serial.println(WiFi.localIP());
    }
  }

void sendmqttMsg(char* topictosend, String payload)
{

  if (client.connected()) {
      Serial.print("Sending payload: ");
      Serial.print(payload);

    unsigned int msg_length = payload.length();

      Serial.print(" length: ");
      Serial.println(msg_length);

    byte* p = (byte*)malloc(msg_length);
    memcpy(p, (char*) payload.c_str(), msg_length);

    if ( client.publish(topictosend, p, msg_length)) {
        Serial.println("Publish ok");
      free(p);
      //return 1;
    } else {
        Serial.println("Publish failed");
      free(p);
      //return 0;
    }
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}



// Read humidity and temperature
// from DHT11 analog sensor
measure get_measurements(DHT dht_sensor) {
  float humidity;
  float temperature;
  measure measurement;
  
  // read the values
  humidity = dht_sensor.readHumidity();
  temperature = dht_sensor.readTemperature();
  measurement = {temperature,humidity};

  // check if read was successfull
  if (isnan(humidity) ){
    Serial.println("Failed to read humidity from DHT sensor!");
    }
  if (isnan(temperature) ){
    Serial.println("Failed to read humidity from DHT sensor!");
    }
    
   // print results
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F("% "));
  
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));
  return measurement;
  }


void setup()
{
  Serial.begin(9600);
  dht_sensor.begin();
  startMills = millis();

    wifi_connect();
  delay(500);
  getTime();
   delay(500);

  loadcerts();
  delay(200);


  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

}

void loop()
{
 
  
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > 2000) {
        lastReconnectAttempt = now;
        if (reconnect()) {
          lastReconnectAttempt = 0;
        }
      }
    } else {
       measure m = get_measurements(dht_sensor);
        String payload = "{ \"state\": {\"desired\":{ \"temperature\" : "+String(m.t)+", \"humidity\":"+String(m.h)+"}}}";
        sendmqttMsg(topic, payload);
        delay(3*60*1000);
    }
  } else {
    wifi_connect();
  }
  

}
