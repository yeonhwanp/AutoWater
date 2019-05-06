#include "Adafruit_Si7021.h" //for temp and humidity sensor

//for temp sensor
Adafruit_Si7021 sensor = Adafruit_Si7021();
float temperature;

//for soil sensor
const int soil_sensor_pin = 34;
int Moistlevel = 0;
int Moistpercent = 0;

float humidity = 0;

void setupSensor() {
  Serial.begin(115200); //initialize serial!

  //Look for temperature and humidity sensor
  Serial.println("Searching for Si7021");
  
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
  }else{
    Serial.println("Found Si7021 sensor!");
  }

  //Set up soil sensor
  pinMode(soil_sensor_pin, INPUT);
  
}

void updateSensors() {
  //Displays temperature and humidity to the Serial Monitor
  Moistpercent = 0;
  Moistlevel = 0;
  Serial.print("Humidity:    ");
  Serial.print(sensor.readHumidity(), 2);
  humidity = sensor.readHumidity();
  Serial.print("\tTemperature: ");
  temperature = (sensor.readTemperature()*9.0/5.0) + 32; //convert temp to F
  Serial.println(temperature, 2);

  //Displays Soil moisture as a percentage to the Serial Monitor
  for (int t = 1; t < 6 ; t++)   // take 5 consecutive measurements
    {
      Moistlevel = Moistlevel + analogRead(soil_sensor_pin); //Read data from soil sensor pin
    }
  Moistlevel = Moistlevel / 5;  //Average out the measurements
  
  //converts Analog Moistlevel reading to a moisture percentage
  Moistpercent = map(Moistlevel, 1100, 3200, 100, 0); //1,000 is totally wet, 3500 is totally dry

  //make sure percentage is between 0 and 100
  if (Moistpercent < 0){
    Moistpercent = 0;
  }else if (Moistpercent >100){
    Moistpercent = 100;
  }

  //print moisture Percentage
  Serial.print("Soil Moisture: ");
  Serial.print(Moistpercent);
  Serial.println(" %");
}

int getMoistPercent() {
  updateSensors();
  return Moistpercent;
}

float getTemp() {
  updateSensors();
  Serial.println("!!!!!!!");
  Serial.println(temperature);
  return temperature;
}

float getHumidity() {
  updateSensors();
  return humidity;
}
