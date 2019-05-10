#include "relay.h"

#include <Arduino.h>

Bulb::Bulb(int pin) {
  RELAY_PIN = pin;
  pinMode(RELAY_PIN, OUTPUT);
}

bool Bulb::getState() {
  return BULB_IS_ON;
}

void Bulb::bulbOn() {
  BULB_IS_ON = true;
  digitalWrite(RELAY_PIN, HIGH);
}

void Bulb::bulbOff() {
  BULB_IS_ON = false;
  digitalWrite(RELAY_PIN, LOW);
}
