#include "button.h"
#include <Arduino.h>

Button::Button(const int p) {
    flag = 0;
    state = 0;
    pin = p;
    pinMode(p, INPUT_PULLUP);
    t_of_state_2 = millis();       //init
    t_of_button_change = millis(); //init
    debounce_time = 10;
    long_press_time = 1000;
    button_pressed = 0;
}

void Button::read() {
    uint8_t button_state = digitalRead(pin);
    button_pressed = !button_state;
}

int Button::update() {
    read();
    flag = 0;
    if (state == 0) {
        flag = 0;
        if (button_pressed) {
            state = 1;
            t_of_button_change = millis();
        }
    } else if (state == 1) {
        flag = 0;
        if (millis() - t_of_button_change >= debounce_time) {
            state = 2;
            t_of_state_2 = millis();
        }
        if (millis() - t_of_button_change < debounce_time && !button_pressed) {
            state = 0;
            t_of_button_change = millis();
        }
    } else if (state == 2) {
        flag = 0;
        if (millis() - t_of_state_2 >= long_press_time) {
            state = 3;
        }
        if (!button_pressed) {
            state = 4;
            t_of_button_change = millis();
        }
    } else if (state == 3) {
        flag = 0;
        if (!button_pressed) {
            state = 4;
            t_of_button_change = millis();
        }
    } else if (state == 4) {
        flag = 0;
        if (millis() - t_of_button_change > debounce_time) {
            if (millis() - t_of_state_2 < long_press_time) {
                flag = 1;
            } else if (millis() - t_of_state_2 >= long_press_time) {
                flag = 2;
            }
            state = 0;
        } else if (millis() - t_of_state_2 < long_press_time && button_pressed) {
            state = 2;
            t_of_button_change = millis();
        } else if (millis() - t_of_state_2 >= long_press_time && button_pressed) {
            state = 3;
            t_of_button_change = millis();
        }
    }
    return flag;
}
