#define MQTT_SERVER "your-numbers-ats.iot.eu-central-1.amazonaws.com"
#define SSID "redacted"
#define PASSWORD "redacted"
#define MQTT_SERVER_FINGERPRINT "AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:AA:88"


// MQTT broker
const char* mqtt_server = MQTT_SERVER;

// WiFi credentials
const char* ssid = SSID;
const char* password = PASSWORD;

// Fingerprint of the broker CA
// openssl x509 -in  mqttserver.crt -sha1 -noout -fingerprint
const char* fingerprint = MQTT_SERVER_FINGERPRINT;