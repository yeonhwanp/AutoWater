#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {

public:
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;    
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state; // This is public for the sake of convenience

  Button(const int p);
  void read();
  int update();
};

#endif
