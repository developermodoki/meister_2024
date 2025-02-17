#include <M5AtomS3.h>
#include <ESP32Servo.h>

Servo servo1;

const int servo1Pin = G39;

void setup() {
  AtomS3.begin(true);
  Serial.begin(9600);

  servo1.setPeriodHertz(50);
  servo1.attach(servo1Pin, 500, 2400); 
  Serial.println("Servo initalized");

}


void loop() {

  AtomS3.update();

  servo1.write(90);
  delay(1000);
  servo1.write(45);
  delay(1000);
}
