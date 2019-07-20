# AutoWater: MIT 6.08 Final Project

Water your plants anytime from anywhere around the world using an ESP32 and other accessories.

## Features
- Water your plants wirelessly via a web interface.
- Read your plant's metrics: temperature and humidity of its living space and the moisture level of the soil.
- Set as many watering schedules as you would like - granular to the second.
- Set watering schedules around plant metrics.
- View a live-stream of your plant using the ArduCam ESP32 interface.
- Optional: Schedules times for grow lights.

## Dependencies

### Hardware
- [Two ESP32 Development Boards](https://www.adafruit.com/product/3269)
- [Any TFT Display](https://www.adafruit.com/product/358)
- [Adafruit STEMMA Soil Sensor](https://www.adafruit.com/product/4026)
- [Adafruit Si7021 Temperature and Humidity Sensor Breakout Board](https://www.adafruit.com/product/3251)
- [Peristaltic Liquid Pump](https://www.adafruit.com/product/1150)
- [ArduCam](https://www.arducam.com/)

### Software
- [Arduino](https://www.arduino.cc/)
- [TFT Library](https://www.arduino.cc/en/Reference/TFTLibrary)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)

## Install
*NOTE:* Ignore all downloads as all necessary files are packaged within the repository.
1. Make sure you have all of the latest hardware and software listed earlier in the dependencies section.
2. Clone the project: `git clone git@github.com:yeonhwanp/autowater.git`
3. Follow [this]() guide to set up all hardware dependencies. 
4. Follow [this]() guide to set up all software dependencies.

## Usage
- *Client*: Open `/src/client/client.ino` in Arduino and compile it into the ESP32.
- *Server*: Upload to a designated web-server to run the code. 

## API

### CAMERA

`Camera.cpp`: The class used to setup + livestream images from the ESP
- start_capture(): turns on the camera
- serverCapture(): wrapper method to handle repeated captures (continuously calling camCapture()).
- serverStream(): sends a single picture
- handleNotFound():
- updateIP(): sends the new IP to the server such that the website can find the ESP

### CLIENT

`Button.cpp`: Button class used previously.
`Client.ino`: Main class used to interface with the ESP32 that holds the sensors. Calls methods to all of the other classes within the folder.

- setPumpDesiredState(): A method that sets the pump to its desired state by calling the 608dev server and seeing whether or not it should currenlty be on.
- setReadings(): A method that sends the readings from the sensors to the 608dev server.
- setCurrentStatus(): A method that sends the current status of the grow light and the pump to the server.
- setupWifi(): setup the wifi connection

`Pump.cpp`: Class used to represent the pump and the methods that it should have

- getState(): return the current state of the pump (on/off)
- pumpOn(): turn on the pump
- pumpOff(): turn off the pump

`Relay.cpp`: Class used to represent the grow light and the methods that it should have

- getState(): return the current state of the grow light (on/off)
- bulbOn(): turn on the grow light
- bulfOff(): turn off the grow light

`Requests.cpp`: File developed from 6.08 exercises that handles http requests
temp_soilsensor_tester.ino:

- Imports the Adafruit sensor library specific to the Temperature/Humidity sensor that we bought from Adafruit

- setupSensor(): initializes and searches for sensor and displays to the serial monitor if found. Also initializes the input pin
- updateSensors():
    - reads the temperature and humidity from the sensor
    - Converts celsius to fahrenheit
    - Averages the soil moisture reading over 5 iterations
    - Converts analog soil moisture reading to a moisture percentage
- getMoistPercent(): calls update sensors and returns moisture
- getTemp(): calls update sensors and returns temp
- getHumidity: calls update sensors and return humidity

### Server
`static/`

- Holds static files that will be used by the web interface, such as the CSS libraries and client side JavaScript code (in the `css` and `js` directories, respectively)

`templates/`

- Contains the raw HTML files to be displayed by specific endpoints

`control.py`

- Reads in files from the static and template directories to render the Web UI for controlling the project

`utils.py`

- Contains useful functions used by other endpoints, such as `seconds_since_midnight()`, which returns the number of seconds that have passed since midnight.
- If `utils.py` is reached via a web request, then it creates the database files and tables if they don't exist (`create_databases()`).

`camera/`

- `read.py`
    - This endpoint returns a JSON with the latest IP that the camera server (`ESP32WebServer`) is located. This endpoint is used by the Web UI to load the camera stream.
- `update.py`
    - This endpoint, when accessed via POST request, changes the latest IP address that the `ESP32WebServer` for the camera is located at.

`control/`

- `override/`
    - `current/`
        - `lamp.py`
            - Returns the current override status for the lamp, as one of the following: `OFF`, meaning the lamp is forced off; `ON`, meaning that the lamp is forced on; or `NONE`, meaning that there is no override active, and the lamp should function based on schedule.
        - `pump.py`
            - Returns the current override status for the pump, as one of the following: `OFF`, meaning the pump is forced off; `ON`, meaning the pump is forced on; or `NONE`, meaning that there is no override in place, and the pump should function based on the latest moisture readings, as well as the schedule.
    - `moisture/`
        - `max.py`
            - POST endpoint that allows the user to set the upper bound for the moisture, that is, the pump should not run if the measured moisture is above this value.
        - `min.py`
            - POST endpoint that allows the user to set the lower bound for the soil moisture; that is, if the measured soil moisture is below this value, the pump will begin running, even if not scheduled to do so.
        - `read.py`
            - Returns in a JSON the current upper and lower bounds set by the user. This is used for display purposes on the Web UI.
    - `lamp.py`
        - POST endpoint that allows the user to control whether or not manual override of the lamp state is active, as well as the specify whether the lamp should be in a forced `ON` or forced `OFF` state.
    - `pump.py`
        - POST endpoint that allows the user to manually control (manual override) whether or not the pump should be `ON` or `OFF`.
- `lamp.py`
    - This endpoint returns a JSON with "intent", that is, whether or not the lamp should be on or off. This is determined by considering both the lamp schedule and the lamp manual override.
- `pump.py`
    - This endpoint returns a JSON with "intent" for the pump, that is, whether or not the pump should be running or not. The intent is determined in three stages: if human control is active, then the setting determined by the human is used. Else, the latest moisture reading is used, so that the moisture stays within the bounds set by the user. Otherwise, the pump will run whenever it has been scheduled to.

`power/`

- `read.py`
    - Endpoint that returns a JSON with the current setting for the update frequency of the device, that is, how often the device should read instructions from the server and how often it should send sensor readings and effector (lamp, pump) status. This is used by both the ESP32 to determine the frequency it should be using, as well as the Web UI to display the current setting.
- `update.py`
    - POST endpoint that allows the user to set the update frequency, in milliseconds.

`schedule/`

- `update/`
    - `lamp.py`
        - POST endpoint that allows the user to set a new schedule for the lamp. The schedule should be a comma delimited list of `start_time:end_time` pairs, where `start_time` and `end_time` are represented as seconds since midnight. This also means that only daily schedules may be constructed (no weekly schedules or more complicated formats).
    - `pump.py`
        - POST endpoint that functions in the exact same way as the lamp, but for the pump. Typically, however, the time scale for the pump schedule is in seconds rather than hours.
- `lamp.py`
    - Returns a JSON with the current schedule for the lamp, formatted as a comma delimited list of `start_time`:`end_time` pairs. This endpoint is used by the Web UI to show the current schedule.
- `pump.py`
    - Returns a JSON with the current schedule for the pump, formatted as a comma delimited list of `start_time`:`end_time` pairs. This endpoint is used by the Web UI to show the current schedule.

`sensors/`

- `read.py`
    - Returns the last 1000 readings sent by the ESP32 within the last day for humidity, moisture (soil moisture), and temperature. This is used by the Web UI to display charts of the corresponding measurements.
- `update.py`
    - POST endpoint that allows the device (the ESP32) to update the server with the latest readings for temperature, humidity, and soil moisture.

`status/`

- `lamp.py`
    - Returns in a JSON the last reported status of the lamp, that is, whether or not the lamp was powered on or off.
- `pump.py`
    - Returns in JSON format the last reported status of the pump as either `ON` or `OFF`.
- `update.py`
    - POST endpoint that allows the ESP32 to report whether or not the lamp and pump are running or not.

### Databases
We used three different databases, each pertaining a specific portion of the project:

- `schedule.db`
    - This database stored the schedules for the lamp and pump, as well as the manual control settings that would override the schedule, i.e. human control. It also contained tables that stored the latest setting of the pump and lamp as reported by the device. Finally, this database also contained the latest setting for how often the device should pull and send updates from and to the server in a table called `update_frequency`.
- `sensors.db`
    - Contains three tables: `readings`, which stores all the readings reported by the ESP32; `moisture_min`, which contains the setting for the lower bound on the soil moisture (used for control); and `moisture_max`, which is used for the same thing as `moisture_min` except to set an upper limit.
- `camera.db`
    - Contains a single table `addresses` that stores the last reported IP address for the camera server, so that it may be accessed by the Web UI without user intervention.



