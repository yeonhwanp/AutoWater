#include <TFT_eSPI.h>
#include <mpu9255_esp32.h>
#include <math.h>
#include <string.h>
#include <WiFi.h>
#include <ESP32WebServer.h>
#include "camera.h"
#include "pump.h"

// Set up the display!
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

unsigned long primary_timer;
const int LOOP_PERIOD = 50;

// Setup Camera
Camera cam;

// Setup Pump
const int WATERPUMP_PIN = 12;
WaterPump pump(WATERPUMP_PIN);

// Setup WIFI and Server
const char *network = "MIT";
const char *password = "";

ESP32WebServer server(80);

void loop() {
  // display IP address
  tft.setCursor(0, 0);
  tft.print("IP: ");
  tft.println(WiFi.localIP());

  tft.print("Pump status:");
  if (pump.getStatus()) {
    tft.println("ON");
  } else {
    tft.println("OFF");
  }

  // Handle requests
  server.handleClient();

  // Consistent ticks
  while (millis() - primary_timer < LOOP_PERIOD); 
  primary_timer = millis();
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
  Serial.println("Pump ON");
}

void handlePumpOff() {
  pump.pumpOff();
  Serial.println("Pump OFF");
}

void handleGetStatus() {
  String message = "STATUS REPORT\n\n";
  message += "PUMP: ";
  message += (pump.getStatus()?) "ON":"OFF";
  message += "\n";
  server.send(200, "text/plain", message);
}

void serverStream() {
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (1) {
    start_capture();
    while (!cam.myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
    size_t len = cam.myCAM.read_fifo_length();
    if (len >= MAX_FIFO_SIZE) //8M
    {
      Serial.println("Over size.");
      continue;
    }
    if (len == 0 ) //0 kb
    {
      Serial.println("Size is 0.");
      continue;
    }
    cam.myCAM.CS_LOW();
    cam.myCAM.set_fifo_burst();
    if (!client.connected()) break;
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
    while ( len-- )
    {
      temp_last = temp;
      temp =  SPI.transfer(0x00);

      //Read JPEG data from FIFO
      if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
      {
        buffer[i++] = temp;  //save the last  0XD9
        //Write the remain bytes in the buffer
        cam.myCAM.CS_HIGH();;
        if (!client.connected()) break;
        client.write(&buffer[0], i);
        is_header = false;
        i = 0;
      }
      if (is_header == true)
      {
        //Write image data to buffer if not full
        if (i < bufferSize)
          buffer[i++] = temp;
        else
        {
          //Write bufferSize bytes image data to file
          cam.myCAM.CS_HIGH();
          if (!client.connected()) break;
          client.write(&buffer[0], bufferSize);
          i = 0;
          buffer[i++] = temp;
          cam.myCAM.CS_LOW();
          cam.myCAM.set_fifo_burst();
        }
      }
      else if ((temp == 0xD8) & (temp_last == 0xFF))
      {
        is_header = true;
        buffer[i++] = temp_last;
        buffer[i++] = temp;
      }
    }
    if (!client.connected()) break;
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

  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK); //set color of font to green foreground, black background

  cam.setupCamera();

  // SETUP WIFI
  setupWifi();

  // SETUP SERVER
  server.on("/pump/on", HTTP_POST, handlePumpOn);
  server.on("/pump/off", HTTP_POST, handlePumpOff);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Server started.");
  
  primary_timer = millis();
}
