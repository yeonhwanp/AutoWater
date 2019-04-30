// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "MIT";
const char* password = "";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
bool gotTemp = false;
bool gotHumidity = false;
bool gotMoist = false;
String thisTemp = "65";
String thisHumidity = "100";
String thisMoisture = "30";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

void setup() {
  setupSensor();
  srand(time(NULL));
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /getTemp") >= 0) {
              Serial.println("Get Temp");
              gotTemp = true;
              thisTemp = getTemp();
            } else if (header.indexOf("GET /getHumidity") >= 0) {
              Serial.println("Get Humidity");
              gotHumidity = true;
              thisHumidity = getHumidity();
            } else if (header.indexOf("GET /updateTemp") >= 0) {
              thisTemp = getTemp();
            } else if (header.indexOf("GET /updateHumidity") >= 0) {
              thisHumidity = getHumidity();
            } else if (header.indexOf("GET /getMoist") >= 0) {
              Serial.println("Get Moisture");
              gotMoist = true;
              thisMoisture = getMoistPercent();
            } else if (header.indexOf("GET /updateMoist") >= 0) {
              thisMoisture = getMoistPercent();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            if (gotTemp == true) {
              client.println("<p>TEMPERATURE: " + thisTemp + "</p>");
              client.println("<p><a href=\"/updateTemp\"><button class=\"button\">Get TEMP</button></a></p>");
            } else {
              client.println("<p><a href=\"/getTemp\"><button class=\"button\">Get TEMP</button></a></p>");
            }
            client.println("</body></html>");

            if (gotHumidity == true) {
              client.println("<p>HUMIDITY: " + thisHumidity + "</p>");
              client.println("<p><a href=\"/updateHumidity\"><button class=\"button\">Get HUMIDITY</button></a></p>");
            } else {
              client.println("<p><a href=\"/getHumidity\"><button class=\"button\">Get HUMIDITY</button></a></p>");
            }
            client.println("</body></html>");

            if (gotMoist == true) {
              client.println("<p>Moisture Percentage: " + thisMoisture + "</p>");
              client.println("<p><a href=\"/updateMoist\"><button class=\"button\">Get MOISTURE</button></a></p>");
            } else {
              client.println("<p><a href=\"/getMoist\"><button class=\"button\">Get MOISTURE</button></a></p>");
            }
            client.println("</body></html>");
            
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
