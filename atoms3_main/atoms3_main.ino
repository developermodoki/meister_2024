#include <M5AtomS3.h>
#include <string.h>

#include <Wire.h>
#include "ClosedCube_TCA9548A.h"
#include "UNIT_SCALES.h"

#include <WiFi.h>
#include <M5UnitOLED.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include <EEPROM.h>
#include <ArduinoJson.h>
// Let each of addresses "0x00" "0x01" "0x02", "0x03", "0x04" be Wifi SSID, Wifi Pass, LINEBot ID, LINEBot Token, User ID. 

#define BT_SV_NAME "m5-test"
#define SERVICE_UUID        "e83823df-3f53-40cb-89f1-586c248ed29d"
#define CHARACTERISTIC_UUID "30448c28-af54-4ed2-ac79-f9378e91a1ad"

#define HUB_ADDR 0x70
//#define SSID "KOKI08-DYNABOOK 1420"
//#define WIFI_PASS "653R0o0<"
String SSID;
String WIFI_PASS;
String BOT_ID;
String BOT_TOKEN;
String USER_ID;

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool Connected = false;

ClosedCube::Wired::TCA9548A tca9548a;
UNIT_SCALES scales;
WiFiClient wifiClient;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    BLEDevice::startAdvertising();
    Connected = true;
    Serial.println("Connected");
  };

  void onDisconnect(BLEServer *pServer) {
    delay(500);         
    BLEDevice::startAdvertising();
    Connected = false;
    Serial.println("Disconnected");
  }
};


class MyReceiveCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    JsonDocument recv;
    DeserializationError err = deserializeJson(recv, value.c_str());

    if (err) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(err.f_str());
    }
    else {
      SSID = data["ssid"].as<String>();
      WIFI_PASS = data["password"].as<String>();
    }


    EEPROM.writeString(0x00, value.c_str());
    EEPROM.commit();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }

    bool WIFI_flag = 0;
    Serial.println("Starting Wi-Fi Reconnection...");
    Serial.println(WIFI_PASS);
    Serial.println(SSID);
    WiFi.begin(SSID, WIFI_PASS);
    delay(2000);

    if(!WiFi.isConnected()) {
      int i = 0;
      while(WiFi.begin(SSID, WIFI_PASS) != WL_CONNECTED) {
        delay(2000);
        i++;
        if(i <= 3) break;
      };
    }
    if(WiFi.isConnected()) WIFI_flag = 1;
    if (WIFI_flag) {
      Serial.print("Wi-Fi Connected. Local IP is: ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println("Wifi Connection failed.");
    }

  }
};

M5UnitOLED display(2, 1, 400000);
M5Canvas canvas(&display);

void setup() {
    AtomS3.begin(true);
    AtomS3.dis.setBrightness(100);

    Serial.begin(9600);
    Wire.begin(2, 1); // Use Port Custom of M5AtomS3, so SDA is 2, SCL is 1.
    tca9548a.address(HUB_ADDR);

    tca9548a.selectChannel(3);
    display.init();
    display.setRotation(1);
    canvas.setColorDepth(1);
    canvas.setTextWrap(false);
    canvas.setTextSize(1.5);
    canvas.createSprite(display.width(), display.height());
    EEPROM.begin(100);

    std::string JsonData = EEPROM.readString(0x00).c_str();
    //EEPROM.get(0x00, JsonData);
    JsonDocument data;
    DeserializationError err = deserializeJson(data, JsonData);

    if (err) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(err.f_str());
      //return;
    }
    else {
      SSID = data["ssid"].as<String>();
      WIFI_PASS = data["password"].as<String>();
      BOT_ID = data["botId"].as<String>();
      BOT_TOKEN = data["token"].as<String>();
      USER_ID = data["userId"].as<String>();
    }

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
    bool WIFI_flag = 0;
    Serial.println("Starting Wi-Fi Connection...");
    Serial.println(WIFI_PASS);
    Serial.println(SSID);
    WiFi.begin(SSID, WIFI_PASS);
    delay(2000);

    if(!WiFi.isConnected()) {
      int i = 0;
      while(WiFi.begin(SSID, WIFI_PASS) != WL_CONNECTED) {
        delay(2000);
        i++;
        if(i <= 3) break;
      };
    }
    if(WiFi.isConnected()) WIFI_flag = 1;
    if (WIFI_flag) {
      Serial.print("Wi-Fi Connected. Local IP is: ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println("Wifi Connection failed.");
    }

    BLEDevice::init(BT_SV_NAME);

    pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyServerCallbacks()); 
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
    );
    pCharacteristic->setCallbacks(new MyReceiveCallbacks());

    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x0);
    pAdvertising->setMinPreferred(0x06); 
    pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();
    Serial.println("Bluetooth Slave Started");
}


void loop() {

  AtomS3.update();

  unsigned long currentTime = millis();
  unsigned long currentTime2 = millis();
  unsigned long currentTime3 = millis();
  static unsigned long previousTime = 0;
  static unsigned long previousTime2 = 0;
  static unsigned long previousTime3 = 0;

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

  tca9548a.selectChannel(3);
  canvas.pushSprite(0, 0);
  canvas.setCursor(0, 0);


  if(currentTime2 - previousTime2 >= 100) {
    previousTime2 = currentTime2;

    canvas.clear();
    canvas.println("Measuring..");
    canvas.printf("Cup1: %dg\n", weight1);
    canvas.printf("Cup2: %dg\n", weight2);
  }

  if(currentTime - previousTime >= 2000) {
    previousTime = currentTime;

    Serial.print("gram1 : ");
    Serial.println(weight1);
    Serial.print("gram2 : ");
    Serial.println(weight2);
  }

  if (Connected) {
    if(currentTime3 - previousTime3 >= 1000){
      previousTime3 = currentTime3;
      pCharacteristic->setValue("Hello World!");
      pCharacteristic->notify();
    }

  }
}

