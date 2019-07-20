# Hardware

## Parts List
You should obtain these parts:
* 2 ESP32 Dev Boards
* TFT Display
* Soil Sensor (https://www.adafruit.com/product/4026)
* Humidity & Temperature sensor (https://www.adafruit.com/product/3251)
* Button
* Peristaltic Pump
* PWM Motor controller
* Arducam camera

## The ESP32

Here is a pin map of the ESP32 for your convenience:


![](https://lh5.googleusercontent.com/qAmrR2Ei7PKazafn09lWFvVZTpWsNjda-pTQ6aDTH2lGReFuTPXM7X1w78oUcHD-Q0xafxjLPKZQ1HF2uj7QHUXv24fXu3ET8ETCt-1cQGuiPiDZWEtvg5dFE0yhmURmDDTZ1_Ou)


## The LCD Display and Button

The TFT LCD Display has 8 pins. Wire up the corresponding pins to the ESP32 according to the diagram below:

![](https://lh5.googleusercontent.com/1Dj3H1oCBy4AFDwDNaJgbFJAkkVIfSu-5kxkiqXNhfUd0eI1DpHbYSZz-SPvLL8b6NjwTDkJPrkux3zo5sl3NhFb2SkehQgQ7CzwDNyOccFyaHOSNXyeWLN7VB86nhVRO6YNbWq7)

Also be sure to download and install the TFT library [here](https://www.dropbox.com/s/y7ru5lovpjhtl69/TFT_eSPI-1.1.3.zip?dl=0).


## The Button

We will be using a button in our implementation in order to control the screen. Wire the button up according to the diagram below.

![](https://lh3.googleusercontent.com/_9cbtrcHR8Jhq8KWqpEOSVd20IwmUdtoAWJv6EI_iHOE7Ki39U6e8K6NoQJ_E2A1qNtSqXhNAugH1bEOv5G32BFNcvy7ZNi5WzEGup2RzobPSFlESUMnUvIVaY6TGcg4EDUaXh_Z)

## Soil Sensor

We will be using a capacitive soil sensor. Capacitive soil sensors are advantageous over
resistive soil sensors, which have the tendency to rust over time. Hook up the soil sensor
to the ESP following the diagram below.

![](https://lh4.googleusercontent.com/0yd2EaSkP3m3grm9iJwOEj51YaRpW6kvj3MdNB3M4F6w6senclsGJQkHsxzQl3hPwtbj7d9MpIkC4wKva-IGTyp4IeSYHyGdExY2wfMuthO_huCZ9QRRYLnAh3f2pNTNzSZyR_Yp)

The Soil Sensor is an analog sensor, so use these two lines of code to set up the sensor:

    const int soil_sensor_pin = 34;
    pinMode(soil_sensor_pin, INPUT);
And use this line of code to get a reading from the Soil Sensor:
    ```analogRead(soil_sensor_pin);```

## Temperature & Humidity Sensor

We will be using an “Adafruit Si7021 Temperature & Humidity Sensor Breakout Board”
to take temperature and humidity readings. What’s nice about this board is that there is
an Arduino Library available with good documentation. Wire up the sensor following
the diagram below.

![](https://lh5.googleusercontent.com/44y9yG-flYEgVRL-VQ_Zubh96V_OK4h7nyqfnn59kJkIeAsNC0tWjgUbfB-3D4tgA3zKv4szCVs5BzPNwjwnZB0eVOclTV0iyVk-wNRieVRyZoJioB7CnV9VWCQQztB8IWLHr2sQ)

Follow the instructions at this link [here](https://learn.adafruit.com/adafruit-si7021-temperature-plus-humidity-sensor/arduino-code), which will tell you how to install the proper Adafruit Library to run this sensor.
Use this line of code to set up the sensor:

    Adafruit_Si7021 sensor = Adafruit_Si7021();

Use this line of code to get a temperature reading:

    temperature = sensor.readTemperature()

Use this line of code to get a humidity reading:

    humidity = sensor.readHumidity();

## Peristaltic Pump

We will be using a Peristaltic Pump for this project. Peristaltic Pumps work by repeatedly squishing the silicone tubing that contains the liquid causing a pressure difference. The benefits of a Peristaltic Pump versus a normal pump is that the pump never touches the liquid! We will be controlling the pump using a PWM motor controller and the Arduino Servo class. We will be using a PWM motor controller because it will give us more flexibility in controlling the pump. We can control direction and speed of pumping. Hook up the motor to the ESP32 by following the diagram below.

![](https://lh5.googleusercontent.com/qvRysY7_4rd-PccqLp6DT0GXbmrCrZ_ycg_aJB4fBPI65mmWn4e8-DDTx4EbNCBsG_sJQPNYaiil_keq-o4bUxEOcT4D0Zl1CR5fU6qVxvRyH6uCdqNNcgaY1aVFT8APQC8Y1Im6)


You will also need to download and install [this library](https://www.dropbox.com/s/nysbgvylb8cj504/ESP32-Arduino-Servo-Library-master.zip?dl=0) in order to control the PWM Motor Controller.

## Arducam Camera

We will be using an Arducam camera with an onboard image processor in order to simplify ESP32 integration. We will be using the camera in order to enable live stream video of the plant.

**USING YOUR SECOND ESP32** hook up the camera to a new ESP32 board following the diagram below:

![](https://lh5.googleusercontent.com/QYquaUBbhvlGyjDWmc8fI9xE00uHm79CSDGBOOR7F2uESamQP2B_GoEOPKFJvCVaMN5dxqg-pk5PfZgwGpWrecKpP_pb4x_VRu2b3U7hsbujJOkT2TJqoQ24Cnk2gxRDb2RB3znx)


Download the code [here](https://github.com/ArduCAM/ArduCAM_ESP32S_UNO) which sets up the Arducam.

## Housing for Hardware
Now that we have all of the electrical hardware setup, let’s create a plastic housing for our parts. The finished plastic housing for the pump/sensor system will look like this when finished.

![](https://lh5.googleusercontent.com/Dc8AVAg6cqw6UREaqepX30KAmID9N7HAO8bZvcYjixS4_CGZqkHx7ljnWNJTy3NNHQimdI9_7K_6EcJXsM8ZHF5WsuL9N8JQ8o1zFR50k-pKZnbzAxFvA5Ke6hNVwwbC-EBkNiwb)

The finished plastic housing for the camera will look like this:

![](https://lh4.googleusercontent.com/q8qNNVr0G00gTDhxUZBzV3O9CfmpWtv38BZyj_cUvP-zhAWkNMAH9fcdh_2q5O_dvA3OTJpTx5Lu62WUqbDMQRrLh_QVpHnNPTMzLg6wJLAcZpNLySuL1G98hIj8wLRJih4jylj9)

Download [these CAD files](https://www.dropbox.com/sh/ace51x2jgkwtft2/AACu94Qn-kIl7UbCrq-tejFma?dl=0), which contain the information needed to print out your very own housing.

**3D printing instructions:**

* Use any 3D printer that prints PLA filament
* Set nozzle temperature to 200c
* Set bed temperature to 60c
* Use support material everywhere

**Assembly instructions:**

Following the images posted above to see where each component goes, screw in the
components into their corresponding holes using a #4 screw.

Now that we have the hardware all set up, let’s program our Automatic Plant Watering system!
