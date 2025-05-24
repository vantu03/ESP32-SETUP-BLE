#ifndef LED_H
#define LED_H

#include <Arduino.h>


class LED {
private:
  uint8_t pin;
  int type;
  unsigned long lastToggle;
  bool state;

public:
  LED(uint8_t gpioPin) {
    pin = gpioPin;
    pinMode(pin, OUTPUT);
    type = 0;
    lastToggle = 0;
    state = false;
  }

  void setMode(int newType) {
    type = newType;
    if (type == 3) {
      digitalWrite(pin, HIGH);
    }
  }

  void update() {
    unsigned long now = millis();
    switch (type) {
      //Từ từ
      case 1:
        if (now - lastToggle >= 500) {
          toggle();
          lastToggle = now;
        }
        break;
      //Nháy nhanh
      case 2:
        if (now - lastToggle >= 150) {
          toggle();
          lastToggle = now;
        }
        break;
      //Liên tục
      case 3:
        break;
      // Nháy đôi rồi nghỉ 2s
      case 4:
        static int blinkCount = 0;
        static bool inPause = false;

        if (!inPause && (now - lastToggle >= 150)) {
          toggle();
          lastToggle = now;
          blinkCount++;

          if (blinkCount >= 4) { // 2 lần bật + 2 lần tắt (4 lần toggle)
            inPause = true;
            lastToggle = now;
          }
        } else if (inPause && (now - lastToggle >= 2000)) {
          blinkCount = 0;
          inPause = false;
          lastToggle = now;
        }
        break;

      default:
        digitalWrite(pin, LOW);
        break;
    }
  }

private:
  void toggle() {
    state = !state;
    digitalWrite(pin, state);
  }
};

#endif
