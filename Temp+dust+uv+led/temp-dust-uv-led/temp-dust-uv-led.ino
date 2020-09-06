#include <Wire.h>
#include <ML8511.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>
#define SEALEVELPRESSURE_HPA (1013.25)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "Redmine";
const char* password = "123456788";

//float temp = 0.0;
//float uv = 0.0;
//float dust = 0.0;

int LED = 16;                 // led connected to D0
int LEDwifi = 0;//D3
WiFiServer server(80);

Adafruit_BME280 bme; // I2C

unsigned long delayTime = 500;

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
int UVOUT = A0; //Output from the sensor
int REF_3V3 = D6; //3.3V power on the Arduino board
//UV



void setup() {
  Serial.begin(115200);
  connectWifi();
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  }

  Serial.println();
  //UV
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);


  
}

void loop() {
  float temp =  getTempValue();
  float dust = getDustValue();
  float uv = getUvValue();
  
  setupServer(temp,dust,uv);
  checkLED(checkTempDanger(temp), checkDustDanger(dust), checkUvDanger(uv));
  Serial.println();
  delay(delayTime);
}

void connectWifi(){
  //Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(LEDwifi, OUTPUT);
  digitalWrite(LEDwifi, LOW);

   
  Serial.print("Connecting to Internet ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  digitalWrite(LEDwifi, HIGH);
 /*-------- server started---------*/ 
  server.begin();
  Serial.println("Server started");
 
  /*------printing ip address--------*/
  Serial.print("IP Address of network: ");
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void setupServer(float temp, float dust, float uv){
  //Get sensor info
  const char* infoTemp = checkTempDanger(temp);
  const char* infoDust = checkDustDanger(dust);
  const char* infoUv = checkUvDanger(uv);
  //Wifi setup
  WiFiClient client = server.available();    
    if (!client) 
    {
      return;
    }
  Serial.println("Waiting for new client");   
  while(!client.available())
  {
    delay(1);
  }
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
/*------------------Creating html page---------------------*/

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  //client.println("<meta http-equiv='refresh' content='5'>");
  
  client.println("<table style='width:100%;border: 1px solid black;border-collapse: collapse;'>");
  client.println("<tr><th>Type</th><th>Stat</th><th>Danger</th></tr>");
  
  client.println("<tr style='border: 1px solid black;border-collapse: collapse;'>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>Temperature</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(temp);
  client.print("</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(infoTemp);
  client.print("</td>");
  client.println("</tr>");

  client.println("<tr style='border: 1px solid black;border-collapse: collapse;'>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>Air Quality PM2.5</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(dust);
  client.print("</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(infoDust);
  client.print("</td>");
  client.println("</tr>");

  client.println("<tr style='border: 1px solid black;border-collapse: collapse;'>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>UV index</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(uv);
  client.print("</td>");
  client.print("<td style='border: 1px solid black;border-collapse: collapse;text-align: center;'>");
  client.print(infoUv); 
  client.print("</td>");
  client.println("</tr>");
  /*
  client.println("Temperature: ");
  
  client.print(temp);
  client.print(" *C ");
  
  client.print(infoTemp);
  client.println("<br><br>");
  
  client.println("Air Quality PM2.5: ");
  client.print(dust); // mg/m3
  client.print(" mcg/m^3 ");
  client.print(infoDust);
  client.println("<br><br>");

  client.println("UV index: ");
  client.print(uv);
  client.print(" W/cm^2 ");
  client.print(infoUv);
  client.println("<br><br>");
*/
  //checkLED(infoTemp,infoDust,infoUv);
  
  //if (request.indexOf("/LED=OFF") != -1)  
  client.println("<br><br>");
  //client.println("<a href=\"/LED=ON\"\"><button>ON</button></a>");
  //client.println("<a href=\"/LED=OFF\"\"><button>OFF</button></a><br />");  
  client.println("</html>");
  
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  
}

void checkLED(const char* infoTemp, const char* infoDust, const char* infoUv){
    if ((infoTemp == "Good" || infoTemp == "Moderate") && (infoDust == "Good" || infoDust == "Moderate") && (infoUv == "Good" || infoUv == "Moderate"))  
  {
    Serial.println("Light low");
    digitalWrite(LED, LOW);
  }
  else
  {
    Serial.println("Light high");
    digitalWrite(LED, HIGH);
  }
}

const char* checkTempDanger(float temp){
  if (temp < 26.0){
    return "Good";
  }
  if (temp < 33.0){
    return "Moderate";
  }
  if (temp < 42.0){
    return "Unhealthy";
  }
  else{
    return "Extremely Unheathy";
  }
}

const char* checkDustDanger(float dust){
  if (dust < 15.4){
    return "Good";
  }
  if (dust < 40.4){
    return "Moderate";
  }
  if (dust < 65.4){
    return "Unhealthy";
  }
  if (dust < 150.4){
    return "Very Unhealthy";
  }
  else{
    return "Extremely Unhealthy";
  } 
}

const char* checkUvDanger(float uv){
  if (uv < 0.067){
    return "Good";
  }
  if (uv < 0.15){
    return "Moderate";
  }
  if (uv < 0.2){
    return "Unhealthy";
  }
  if (uv < 0.267){
    return "Very Unhealthy";
  }
  else{
    return "Extremely Unhealthy";
  } 
}

float getTempValue() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  Serial.println(checkTempDanger(bme.readTemperature()));
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  //Serial.print("Pressure = ");
  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println(" hPa");

  //Serial.print("Approx. Altitude = ");
  //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //Serial.println(" m");

  //Serial.print("Humidity = ");
  //Serial.print(bme.readHumidity());
  //Serial.println(" %");
  return bme.readTemperature();
  //Serial.println();
}

float getDustValue()
{
  
  delayMicroseconds(280);
 
  voltsMeasured = analogRead(dustPin); // read the dust value
 
  delayMicroseconds(40);

  delayMicroseconds(9680);
 
  //measure your 5v and change below
  calcVoltage = voltsMeasured * (3.3 / 1024.0);
  dustDensity = 0.17 * calcVoltage - 0.1;
  //Serial.println("GP2Y1010AU0F readings"); 
  //Serial.print("Raw Signal Value = ");
  //Serial.println(voltsMeasured); 
  //Serial.print("Voltage = ");
  //Serial.println(calcVoltage);
  Serial.print("Dust Density = ");
  Serial.println(dustDensity*100); // mg/m3
  Serial.println(checkDustDanger(dustDensity*100));
  //delay(1000);
  return dustDensity*100; // doi sang micro/m3 *1000
}

//void printUvValue()
//{
  // manually enable / disable the sensor.
//  light.enable();
//  float UV = light.getUV();
//  light.disable();
//  Serial.print("UV Index = ");
//  Serial.print(UV, 4);
//  Serial.println(" mW/cm^2");
//}
//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 1;
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
 
float getUvValue()
{
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
 
  /*Serial.print("output: ");
  Serial.print(refLevel);

  Serial.print("ML8511 output: ");
  Serial.print(uvLevel);
 
  //Serial.print(" / ML8511 voltage: ");
  //Serial.print(outputVoltage);
 
  Serial.print(" / UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);*/

  Serial.print("UV Ray Intensity");
  //lcd.setCursor(0, 1);
  //Serial.print(uvIntensity);
  //Serial.print(" mW/cm^2");
  Serial.print(uvIntensity/10);
  Serial.print(" W/cm^2");
  Serial.print(checkUvDanger(uvIntensity/10));
  //Serial.println();
  return uvIntensity/10; //to W/m2
  //delay(200);
}
