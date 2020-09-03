#include <Wire.h>
#include <ML8511.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

// dust sensor - Wemos A0 pin
int dustPin = A0; 
float voltsMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
// dust sensor - Wemos A0 pin

//UV
#define ANALOGPIN     D4
#define ENABLEPIN     D3

ML8511 light(ANALOGPIN, ENABLEPIN);
//UV

//LCD
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x3F is the address of the LCD


void setup() {
  Serial.begin(9600);
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  //lcd.backlight();
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("IOT MAKER VIETNAM");
  lcd.setCursor(3,1);
  lcd.print("Hello, world!");
}

void loop() { 
  //printTempValue();
  //printDustValue();
  //printUvValue();
  //printLedValue();
  //delay(delayTime);
}

void printTempValue() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void printDustValue()
{
  
  delayMicroseconds(280);
 
  voltsMeasured = analogRead(dustPin); // read the dust value
 
  delayMicroseconds(40);

  delayMicroseconds(9680);
 
  //measure your 5v and change below
  calcVoltage = voltsMeasured * (3.3 / 1024.0);
  dustDensity = 0.17 * calcVoltage - 0.1;
  Serial.println("GP2Y1010AU0F readings"); 
  Serial.print("Raw Signal Value = ");
  Serial.println(voltsMeasured); 
  Serial.print("Voltage = ");
  Serial.println(calcVoltage);
  Serial.print("Dust Density = ");
  Serial.println(dustDensity); // mg/m3
  Serial.println("");
  delay(1000);
}

void printUvValue()
{
  // manually enable / disable the sensor.
  light.enable();
  float UV = light.getUV();
  light.disable();

  Serial.print(UV, 4);
  Serial.println(" mW cm^2");
  delay(1000);
}
void printLedValue(){
  lcd.setCursor(1,0);
  lcd.print("IOT MAKER VIETNAM");
  lcd.setCursor(3,1);
  lcd.print("Hello, world!");
}
