#include "SharpGP2Y10.h"

 //UV sensor
 LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//Hardware pin definitions
int UVOUT = A0; //Output from the sensor
int REF_3V3 = A1; //3.3V power on the Arduino board

 
//DUST Sensor
int voPin = A0;
int ledPin = 2;
float dustDensity = 0;

//Temperature sensor
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11 
#define BME_CS 10
 
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
SharpGP2Y10 dustSensor(voPin, ledPin);
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

  lcd.begin(16, 2);
 
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
}
  
void blink(int LED, int sec, int time)
{
  for (int i = 0; i< time;i++){
  digitalWrite(LED,HIGH);
  delay(sec);
  digitalWrite(LED,LOW);
  delay(sec);
  }
  
}

float readUV(){
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0);
  return uvIntensity;
}
float readPM(){
  return dustSensor.getDustDensity();
}

float returnTemp(){
  return bme.readTemperature();
}


float returnHumid(){
  return bme.readHumidity();
}


float returnPress(){
  return bme.readPressure();
}

int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
 
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
 
  return(runningValue);
}
 
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//https://how2electronics.com/uv-sensor-ml8511-arduino-uv-ray-intensity-meter/
