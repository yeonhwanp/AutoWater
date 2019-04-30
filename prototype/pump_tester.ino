const int input_pin = 16;
#include <Servo.h>

//Set up water pump
Servo water_pump;

const int WATERPIN = 12;

const int pump = 2000;
const int off = 1500;
 
void setup(){
    Serial.begin(115200); //initialize serial!
    //Set up an Active Low Switch!
    pinMode(input_pin,INPUT_PULLUP); //sets IO pin 19 as an input which defaults to a 3.3V signal when unconnected and 0V when the switch is pushed
    pinMode(WATERPIN, OUTPUT);
    water_pump.attach(WATERPIN);
}
 
void loop(){
    int value = digitalRead(input_pin);
    if (value){
        //executed if pin is HIGH (voltage of 3.3V)
       //code here for printing "unpushed" 
       Serial.println("unpushed");
       water_pump.writeMicroseconds(off);
    }else{ 
        //executed if pin is LOW (voltage of 0V)
       //code here to print "pushed" 
       Serial.println("pushed");
       water_pump.writeMicroseconds(pump);
    }
}
