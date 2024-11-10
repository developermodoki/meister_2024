#include <M5AtomS3.h>
#include "ClosedCube_TCA9548A.h"
#include <Wire.h>
#include <WiFi.h>
#include "UNIT_SCALES.h"

#define HUB_ADDR 0x70

ClosedCube::Wired::TCA9548A tca9548a;
UNIT_SCALES scales;

void setup() {
    AtomS3.begin(true);
    Serial.begin(9600);
    Wire.begin(2, 1);
    tca9548a.address(HUB_ADDR);

    // use Channel 2 and 3
    for(uint8_t ch = 1; ch < 3; ch++) {
      tca9548a.selectChannel(ch);
      while (!scales.begin(&Wire, 2, 1, DEVICE_DEFAULT_ADDR)) {
        Serial.println("scales connect error");
        delay(1000);
      }
    }
}

void loop() {

  for(uint8_t ch = 1; ch < 3; ch++) {
    int weight = 0;
    tca9548a.selectChannel(ch);
    Serial.print("Current Channel: ");
    Serial.println(ch);

    weight = scales.getWeight();
    Serial.print("gram : ");
    Serial.println(weight);
    delay(500);
  }
}

