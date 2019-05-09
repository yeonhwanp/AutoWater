#include <TFT_eSPI.h>
#include <mpu9255_esp32.h>
#include <math.h>
#include <string.h>
#include <WiFi.h>
#include <ESP32WebServer.h>
#include "camera.h"
#include "pump.h"
#include "relay.h"












class Button{
  public:
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;    
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state; // This is public for the sake of convenience
  Button(int p) {
  flag = 0;  
    state = 0;
    pin = p;
    t_of_state_2 = millis(); //init
    t_of_button_change = millis(); //init
    debounce_time = 10;
    long_press_time = 1000;
    button_pressed = 0;
  }
  void read() {
    uint8_t button_state = digitalRead(pin);  
    button_pressed = !button_state;
  }
  int update() {
    read();
    flag = 0;
    if (state==0) {
      flag = 0;
      if (button_pressed) {
        state = 1;
        t_of_button_change = millis();
      }
    }
    
    else if (state==1) {
      flag = 0;
       if (millis() - t_of_button_change >= debounce_time) {
          state = 2;
          t_of_state_2 = millis();
       }
       if (millis() - t_of_button_change < debounce_time && !button_pressed) {
        state = 0;
        t_of_button_change = millis();
       }
    }
    
    else if (state==2) { 
      flag = 0;
      if (millis() - t_of_state_2 >= long_press_time) {
        state = 3;
      }
      if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      }
    }
    
    else if (state == 3) {
      flag = 0;
      if (!button_pressed) {
        state = 4;
        t_of_button_change = millis();
      }
    }
    
    else if (state == 4) {
      flag = 0;
      if (millis() - t_of_button_change > debounce_time) {
        
        if (millis() - t_of_state_2 < long_press_time) {
          flag = 1;
        }
        else if (millis() - t_of_state_2 >= long_press_time) {
          flag = 2;
        }
        state = 0;
      }
      else if (millis() - t_of_state_2 < long_press_time && button_pressed) {
        state = 2;
        t_of_button_change = millis();
      }
      else if (millis() - t_of_state_2 >= long_press_time && button_pressed) {
        state = 3;
        t_of_button_change = millis();
      }
      
    }
    return flag;
  }
};








// Set up the display!
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN = 5;

unsigned long primary_timer;
const int LOOP_PERIOD = 50;

unsigned long effector_update_timer;
unsigned long sensor_reading_timer;
unsigned long display_timer;
const int EFFECTOR_UPDATE_PERIOD = 5000; // 5000ms
const int SENSOR_UPDATE_PERIOD = 5000;
const int DISPLAY_TIME = 10000;

// Setup Camera
Camera cam;

// Setup Pump
const int WATERPUMP_PIN = 12;
WaterPump pump(WATERPUMP_PIN);

// Setup Bulb
const int BULB_PIN = 14;
Bulb bulb(BULB_PIN);

// Setup WIFI and Server
const char *network = "MIT";
const char *password = "";

// Setup AP
const char *AP_ssid = "608FinalProject";
const char *AP_password = "ihatefinalprojects";

// Setup Raspberry Pi
char *RPI_HOST = "192.168.4.2";

ESP32WebServer server(80);

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

Button button(BUTTON_PIN); //button object!

void setLampDesiredState() {
  char request_buffer[200];
  sprintf(request_buffer, "GET /schedule/lamp HTTP/1.1\r\n");
  strcat(request_buffer, "Host: ");
  strcat(request_buffer, RPI_HOST);
  strcat(request_buffer, "\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request(RPI_HOST, request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);

  if (strstr(response, "TRUE") != NULL && strstr(response, "FALSE") == NULL) {
    bulb.bulbOn();    
  } else if (strstr(response, "FALSE") != NULL && strstr(response, "TRUE") == NULL) {
    bulb.bulbOff();
  }
}

void setPumpDesiredState() {
  char request_buffer[200];
  sprintf(request_buffer, "GET /schedule/pump HTTP/1.1\r\n");
  strcat(request_buffer, "Host: ");
  strcat(request_buffer, RPI_HOST);
  strcat(request_buffer, "\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request(RPI_HOST, request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);

  if (strstr(response, "TRUE") != NULL && strstr(response, "FALSE") == NULL) {
    pump.pumpOn();    
  } else if (strstr(response, "FALSE") != NULL && strstr(response, "TRUE") == NULL) {
    pump.pumpOff();
  }
}

void setReadings(int moist, float temp, float humidity) {
  
  char readings[200];
  sprintf(readings, "moisture=%i&temp=%f&humidity=%f", moist, temp, humidity); //I need to be changed.
  char request[500];
  sprintf(request,"POST /sensors/update HTTP/1.1\r\n");
  sprintf(request+strlen(request),"Host: %s\r\n",RPI_HOST);
  strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request+strlen(request),"Content-Length: %d\r\n\r\n",strlen(readings));
  strcat(request,readings);
  do_http_request(RPI_HOST,request,response,OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  
}

void loop() {
  // display IP address
  tft.setCursor(0, 0);
  tft.print("IP: ");
//  tft.println(WiFi.localIP());
  tft.println(WiFi.softAPIP());

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
    effector_update_timer = millis();
  }

  if (millis() - sensor_reading_timer > SENSOR_UPDATE_PERIOD) {
    int moistPercent = getMoistPercent();
    float temp = getTemp();
    float humidity = getHumidity();
    setReadings(moistPercent, temp, humidity);
    sensor_reading_timer = millis();
  }
  

  // Handle requests
  server.handleClient();

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
  }else {
    tft.println("off");
  }
}

// --- SERVER FUNCTIONS ---
void handleNotFound()
{
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    server.send(200, "text/plain", message);
    Serial.println(message);

    if (server.hasArg("ql"))
    {
        int ql = server.arg("ql").toInt();
        cam.myCAM.OV2640_set_JPEG_size(ql);
        Serial.println("QL change to: " + server.arg("ql"));
    }
}

void handlePumpOn() {
  pump.pumpOn();
  server.send(200, "text/plain", "PUMP ON");
  Serial.println("Pump ON");
}

void handlePumpOff() {
  pump.pumpOff();
  server.send(200, "text/plain", "PUMP OFF");
  Serial.println("Pump OFF");
}

void handleBulbOn() {
  bulb.bulbOn();
  server.send(200, "text/plain", "BULB ON");
  Serial.println("Bulb ON");
}

void handleBulbOff() {
  bulb.bulbOff();
  server.send(200, "text/plain", "BULB OFF");
  Serial.println("Bulb OFF");
}

void handleGetStatus() {
  String message = "STATUS REPORT\n\n";
  message += "PUMP: ";
  message += pump.getState() ? "ON":"OFF";
  message += "\n";
  message += "LAMP: ";
  message += bulb.bulbState();
  message += "\n";
  server.send(200, "text/plain", message);
}

// --- SETUP ROUTINES ---
void setupAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_ssid, AP_password);
  Serial.println(WiFi.softAPIP());
}

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

  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  // SETUP WIFI
//  setupWifi();
  setupAP();
  setupSensor();

  // SETUP SERVER
  server.on("/pump/on", HTTP_GET, handlePumpOn);
  server.on("/pump/off", HTTP_GET, handlePumpOff);
  server.on("/bulb/on", HTTP_GET, handleBulbOn);
  server.on("/bulb/off", HTTP_GET, handleBulbOff);
  server.on("/status", HTTP_GET, handleGetStatus);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Server started.");
  
  primary_timer = millis();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}
