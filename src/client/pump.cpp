#include "pump.h"

#include <Arduino.h>

WaterPump::WaterPump(int pin) {
  PUMP_PIN = pin;
  pinMode(PUMP_PIN, OUTPUT);
  _pump.attach(PUMP_PIN);
}

bool WaterPump::getState() {
  return IS_PUMP_ON;
}

void WaterPump::pumpOn() {
  IS_PUMP_ON = true;
  _pump.writeMicroseconds(ON);
}

void WaterPump::pumpOff() {
  IS_PUMP_ON = false;
  _pump.writeMicroseconds(OFF);
}
