#include <M5AtomS3.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define SSID "hidden"
#define WIFI_PASS "hidden"

WiFiClient wifiClient;

void setup() {
    AtomS3.begin(true);
    Serial.begin(9600);

    Serial.println("Starting Wi-Fi Connection...");
    WiFi.begin(SSID, WIFI_PASS);
    delay(2000);

    if(!WiFi.isConnected()) {
      while(WiFi.begin(SSID, WIFI_PASS) != WL_CONNECTED) delay(2000);
    }
    Serial.print("Wi-Fi Connected. Local IP is: ");
    Serial.println(WiFi.localIP());
}
//curl https://httpbin.org/ip

void loop() {

}
