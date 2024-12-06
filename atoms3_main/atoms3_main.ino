#include <M5AtomS3.h>

#include <Wire.h>
#include "ClosedCube_TCA9548A.h"
#include "UNIT_SCALES.h"

#include <ArduinoMqttClient.h>
#include <WiFi.h>

#define HUB_ADDR 0x70
#define SSID "hidden"
#define WIFI_PASS "hidden"

#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883
// Configuration of MQTT Topic Names
#define SCALES1_TOPIC_NAME "hidden"
#define SCALES2_TOPIC_NAME "hidden"



ClosedCube::Wired::TCA9548A tca9548a;
UNIT_SCALES scales;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
    AtomS3.begin(true);
    AtomS3.dis.setBrightness(100);

    Serial.begin(9600);
    Wire.begin(2, 1); // Use Port Custom of M5AtomS3, so SDA is 2, SCL is 1.
    tca9548a.address(HUB_ADDR);

    // use Channel 2 and 3 in Pa.HUB2
    for(uint8_t ch = 1; ch < 3; ch++) {
      tca9548a.selectChannel(ch);
      while (!scales.begin(&Wire, 2, 1, DEVICE_DEFAULT_ADDR)) {
        Serial.println("MiniScales Connect Error, trying again...");
        delay(500);
      }
    }
    Serial.println("All MiniScales are successfully initialized");
    
    //Wifi Configuration
    Serial.println("Starting Wi-Fi Connection...");
    WiFi.begin(SSID, WIFI_PASS);
    delay(2000);

    if(!WiFi.isConnected()) {
      while(WiFi.begin(SSID, WIFI_PASS) != WL_CONNECTED) delay(2000);
    }
    Serial.print("Wi-Fi Connected. Local IP is: ");
    Serial.println(WiFi.localIP());

    //MQTT Connection
    Serial.println("Starting MQTT Connection... (Publisher)");
    while(!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
      delay(500);
    }
    Serial.println("MQTT successfully connected.");
}


void loop() {
  // keep MQTT Connection alive
  mqttClient.poll();
  AtomS3.update();
  unsigned long currentTime = millis();
  static unsigned long previousTime = 0;

  int32_t weight1 = 0;
  int32_t weight2 = 0;

  //0kg Adjustment
  static int32_t zeroTare1 = 0;
  static int32_t zeroTare2 = 0;

  if(AtomS3.BtnA.wasPressed()) {
    AtomS3.dis.drawpix(0x00FF00);
    tca9548a.selectChannel(1);
    zeroTare1 = scales.getWeight();
    tca9548a.selectChannel(2);
    zeroTare2 = scales.getWeight();
  }

  tca9548a.selectChannel(1);
  weight1 = scales.getWeight() - zeroTare1;
  tca9548a.selectChannel(2);
  weight2 = scales.getWeight() - zeroTare2;


  if(currentTime - previousTime >= 2000) {
    previousTime = currentTime;

    Serial.print("gram1 : ");
    Serial.println(weight1);
    Serial.print("gram2 : ");
    Serial.println(weight2);

    //Send to Server
    Serial.println("Sending to MQTT...");
    mqttClient.beginMessage(SCALES1_TOPIC_NAME);
    mqttClient.print(weight1);
    mqttClient.endMessage();

    mqttClient.beginMessage(SCALES2_TOPIC_NAME);
    mqttClient.print(weight2);
    mqttClient.endMessage();
  }
}

