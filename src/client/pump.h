#ifndef PUMP_H
#define PUMP_H

#include <Servo.h>

class WaterPump {

private:
    int PUMP_PIN;
    const int ON = 2000;
    const int OFF = 1500;
    bool IS_PUMP_ON = false;
    Servo _pump;
public:
    WaterPump(int pin);
    bool getState();
    void pumpOn();
    void pumpOff();
};

extern WaterPump waterpump;
#endif
