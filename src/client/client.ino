#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu9255_esp32.h>
#include<math.h>
#include<string.h>

// Set up the display!
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

// Set up the IMU (accelerometer)
MPU9255 imu;                              //imu object called, appropriately, imu

// Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000;        // ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000;    // size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE];           // char array buffer to hold HTTP request

unsigned long primary_timer;

void setup() {
  Serial.begin(115200); //for debugging if needed.

  
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK); //set color of font to green foreground, black background
  
  primary_timer = millis();
}


void loop() {


  // Consistent ticks
  while (millis() - timer < LOOP_PERIOD); 
  timer = millis();
}
