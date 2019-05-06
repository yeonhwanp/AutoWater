#ifndef RELAY_H
#define RELAY_H

class Bulb {
private:
  bool BULB_IS_ON;
  int RELAY_PIN;

public:
  Bulb(int pin);
  bool bulbState();
  void bulbOn();
  void bulbOff();
};

extern Bulb bulb;
#endif
