#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

#define SSID "hidden"
#define WIFI_PASS "hidden"

#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883
// Configuration of MQTT Topic Names
#define SCALES1_TOPIC_NAME "hidden"
#define SCALES2_TOPIC_NAME "hidden"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);

  //Wifi Configuration
  Serial.println("Starting Wi-Fi Connection...");
  WiFi.begin(SSID, WIFI_PASS);
  delay(2000);
  if(!WiFi.isConnected()) {
    delay(500);
  }
  Serial.print("Wi-Fi successfully connected, Local IP Address is: ");
  Serial.println(WiFi.localIP());


  //MQTT Configuration
  Serial.println("Starting MQTT Connection... (Subscriber)");
  while(!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
    delay(5000);
  }
  mqttClient.onMessage();
  Serial.println("MQTT successfully connected.");


  //Subscribing Topics
  mqttClient.subscribe(SCALES1_TOPIC_NAME);
  mqttClient.subscribe(SCALES2_TOPIC_NAME);



}

void loop() {

}
