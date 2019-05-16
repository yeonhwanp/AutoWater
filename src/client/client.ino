#include <TFT_eSPI.h>
#include <mpu9255_esp32.h>
#include <math.h>
#include <string.h>
#include <WiFi.h>
#include "camera.h"
#include "pump.h"
#include "relay.h"
#include "button.h"

// Set up the display!
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

unsigned long primary_timer;
const int LOOP_PERIOD = 50;

unsigned long effector_update_timer;
unsigned long sensor_reading_timer;
unsigned long display_timer;
const int EFFECTOR_UPDATE_PERIOD = 1000; // 1000ms
const int SENSOR_UPDATE_PERIOD = 1000;
const int DISPLAY_TIME = 10000;

// Setup Camera
Camera cam;

// Setup Pump
const int WATERPUMP_PIN = 12;
WaterPump pump(WATERPUMP_PIN);

// Setup Bulb
const int BULB_PIN = 14;
Bulb bulb(BULB_PIN);

// Setup WIFI
const char *network = "MIT";
const char *password = "";

// Setup requests
// Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000;        // ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000;    // size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE];           // char array buffer to hold HTTP request

// Camera constants
static const size_t bufferSize = 2048;
static uint8_t buffer[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;

const int BUTTON_PIN = 5;
Button button(BUTTON_PIN);                // for controlling the screen

void setLampDesiredState() {
  char request_buffer[200];
  sprintf(request_buffer, "GET /sandbox/sc/mattfeng/finalproject/server/control/lamp.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net");
  strcat(request_buffer, "\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request("608dev.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);

  if (strstr(response, "TRUE") != NULL && strstr(response, "FALSE") == NULL) {
    bulb.bulbOn();    
  } else if (strstr(response, "FALSE") != NULL && strstr(response, "TRUE") == NULL) {
    bulb.bulbOff();
  }
}

void setPumpDesiredState() {
  char request_buffer[200];
  sprintf(request_buffer, "GET /sandbox/sc/mattfeng/finalproject/server/control/pump.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net");
  strcat(request_buffer, "\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request("608dev.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);

  if (strstr(response, "TRUE") != NULL && strstr(response, "FALSE") == NULL) {
    pump.pumpOn();    
  } else if (strstr(response, "FALSE") != NULL && strstr(response, "TRUE") == NULL) {
    pump.pumpOff();
  }
}

void setReadings(int moist, float temp, float humidity) {
  char readings[200];
  sprintf(readings, "moisture=%i&temp=%f&humidity=%f", moist, temp, humidity);
  char request[500];
  sprintf(request,"POST /sandbox/sc/mattfeng/finalproject/server/sensors/update.py HTTP/1.1\r\n");
  sprintf(request + strlen(request),"Host: 608dev.net\r\n");
  strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request + strlen(request),"Content-Length: %d\r\n\r\n", strlen(readings));
  strcat(request, readings);
  do_http_request("608dev.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}

void setCurrentStatus(bool pump, bool lamp) {
  char state[200];
  sprintf(state, "pump_status=%s&lamp_status=%s", pump ? "ON" : "OFF", lamp ? "ON" : "OFF");
  char request[500];
  sprintf(request,"POST /sandbox/sc/mattfeng/finalproject/server/status/update.py HTTP/1.1\r\n");
  sprintf(request + strlen(request),"Host: 608dev.net\r\n");
  strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request + strlen(request),"Content-Length: %d\r\n\r\n", strlen(state));
  strcat(request, state);
  do_http_request("608dev.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}

void loop() {
  // display IP address
  tft.setCursor(0, 0);
  tft.print("IP: ");
  tft.println(WiFi.localIP());

  tft.print("Pump status:");
  if (pump.getState()) {
    tft.println("ON ");
  } else {
    tft.println("OFF");
  }

  // Determine if pump should be on
  if (millis() - effector_update_timer > EFFECTOR_UPDATE_PERIOD) {
    setPumpDesiredState();
    setLampDesiredState();
    setCurrentStatus(pump.getState(), bulb.getState());
    effector_update_timer = millis();
  }

  if (millis() - sensor_reading_timer > SENSOR_UPDATE_PERIOD) {
    int moistPercent = getMoistPercent();
    float temp = getTemp();
    float humidity = getHumidity();
    setReadings(moistPercent, temp, humidity);
    sensor_reading_timer = millis();
  }
  
  // Consistent ticks
  while (millis() - primary_timer < LOOP_PERIOD); 
  primary_timer = millis();

  // Reset screen on time
  if (button.update() == 1) {
    display_timer = millis();
  }

  // keep display on
  if (millis() - display_timer > DISPLAY_TIME) {
    tft.println("on");
  } else {
    tft.println("off");
  }
}

// --- SETUP ROUTINES ---

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just ry again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200); //for debugging if needed.

  // Setup display
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  // SETUP WIFI
  setupWifi();
  setupSensor();

  primary_timer = millis();
}
