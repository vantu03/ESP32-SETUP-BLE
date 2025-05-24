#include <WiFi.h>
#include "LED.h"
#include <Arduino.h>
#include "WiFiManager.h"

WiFiManager wfm;
LED led(2);

void setup() {
  led.setMode(1);
  Serial.begin(115200);
  wfm.begin();
}

void loop() {
  led.update();
  wfm.update();
  if (!wfm.isWifiReady()) {
    led.setMode(4);
  } else if (!wfm.isOnline()) {
    led.setMode(3);
  }
  delay(100);
}


