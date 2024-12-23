//https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/Server_multiconnect/Server_multiconnect.ino
//https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/Write/Write.ino
#include <M5AtomS3.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


#define BT_SV_NAME "m5-test"
#define SERVICE_UUID        "e83823df-3f53-40cb-89f1-586c248ed29d"
#define CHARACTERISTIC_UUID "30448c28-af54-4ed2-ac79-f9378e91a1ad"

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool Connected = false;

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

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};


void setup() {
  AtomS3.begin(true);
  Serial.begin(9600);

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
  if (Connected) {
    pCharacteristic->setValue("Hello World!");
    pCharacteristic->notify();
    delay(1000); 
  }
}

