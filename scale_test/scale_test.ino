#include <M5AtomS3.h>
#include "ClosedCube_TCA9548A.h"
#include "UNIT_SCALES.h"
#define HUB_ADDR 0x70

ClosedCube::Wired::TCA9548A tca9548a;

void setup() {
    AtomS3.begin(true);  // Init M5AtomS3Lite.
    tca9548a.address(HUB_ADDR);
    Serial.begin(9600);
    //AtomS3.dis.setBrightness(100);
}

void loop() {
    uint8_t returnCode = 0;
    uint8_t address;

    for(uint8_t channel = 0; channel < TCA9548A_MAX_CHANNELS; channel++) {
      UNIT_SCALES scales;

      Serial.println("Changing Channel...");
      returnCode = tca9548a.selectChannel(channel);
      if(returnCode == 0) {
        Serial.println("Success");
      }

      while (!scales.begin(&Wire, 21, 22, DEVICE_DEFAULT_ADDR)) {
        Serial.println("scales connect error");
        delay(1000);
      }

      Serial.print("OK : ");
      Serial.println(scales.getWeight());
      delay(1000);
    }
}