#include <M5AtomS3.h>
#include "ClosedCube_TCA9548A.h"
#include <Wire.h>

#define PaHub_I2C_ADDRESS 0x70

ClosedCube::Wired::TCA9548A tca9548a;

void setup() {
    AtomS3.begin();
    AtomS3.Power.begin();
    Wire.begin(2, 1);
    Serial.begin();
    Serial.println("Begin");
    tca9548a.address(PaHub_I2C_ADDRESS);
}

void loop() {
    uint8_t returnCode = 0;
    uint8_t address = 0x26;
    //uint8_t channel = 1;
    for (uint8_t channel = 0; channel < TCA9548A_MAX_CHANNELS; channel++) {
        returnCode = tca9548a.selectChannel(channel);

        if (returnCode == 0) {
          Serial.print("Current Channel: ");
          Serial.println(channel);
          Wire.beginTransmission(address);
          returnCode = Wire.endTransmission();

          if (returnCode == 0) {
            Serial.print("I2C device = ");
            Serial.printf("0X%X  \n", address);

          }
          else {
           Serial.println("Failed");
          }
        }

        delay(2000);
    }
}