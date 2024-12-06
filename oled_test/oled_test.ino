#include <M5AtomS3.h>
#include "ClosedCube_TCA9548A.h"
#include <Wire.h>
#include <M5UnitOLED.h>
#define HUB_ADDR 0x70

M5UnitOLED display(2, 1, 400000);
M5Canvas canvas(&display);

ClosedCube::Wired::TCA9548A tca9548a;

void setup() {
  AtomS3.begin(true);
  Wire.begin(2, 1);
  tca9548a.address(HUB_ADDR);

  tca9548a.selectChannel(3);
  display.init();
  display.setRotation(1);
  canvas.setColorDepth(1); // mono color
  //canvas.setFont(&fonts::lgfxJapanMinchoP_32);
  canvas.setTextWrap(false);
  canvas.setTextSize(2);
  canvas.createSprite(display.width(), display.height());

}

void loop() {

  tca9548a.selectChannel(3);
  canvas.pushSprite(0, 0);
  canvas.setCursor(0, 0);
  canvas.print("y = sin(x)");
}