#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 #include <Wire.h>
#include <ML8511.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Replace with your network credentials
const char* ssid = "Tinypink";
const char* password = "hoilamgi";
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

//Dust
int dustPin = A0; 
float voltsMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

//Temperature
Adafruit_BME280 bme; // I2C

//UV
#define ANALOGPIN     D4
#define ENABLEPIN     D3
ML8511 light(ANALOGPIN, ENABLEPIN);
int UVOUT = A0; //Output from the sensor
int REF_3V3 = D6; //3.3V power on the Arduino board
 
String page = "";
String text = "";
String data;
 
void setup(void){
 //pinMode(A0, INPUT);
 delay(1000);
 Serial.begin(115200);
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");

 //Temp
 bme.begin(0x76);  

 //UV
 pinMode(UVOUT, INPUT);
 pinMode(REF_3V3, INPUT);

 
 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
}
 
 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 server.on("/data.txt", [](){
   text = (String)data;
   server.send(200, "text/html", text);
 });
 server.on("/", [](){
   page = "<h1>Sensor to Node MCU Web Server</h1><h1>Data:</h1> <h1 id=\"data\">""</h1>\r\n";
   page += "<script>\r\n";
   page += "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, 1000);\r\n";
   page += "function loadData(url, callback){\r\n";
   page += "var xhttp = new XMLHttpRequest();\r\n";
   page += "xhttp.onreadystatechange = function(){\r\n";
   page += " if(this.readyState == 4 && this.status == 200){\r\n";
   page += " callback.apply(xhttp);\r\n";
   page += " }\r\n";
   page += "};\r\n";
   page += "xhttp.open(\"GET\", url, true);\r\n";
   page += "xhttp.send();\r\n";
   page += "}\r\n";
   page += "function updateData(){\r\n";
   page += " document.getElementById(\"data\").innerHTML = this.responseText;\r\n";
   page += "}\r\n";
   page += "</script>\r\n";
   server.send(200, "text/html", page);
});
 
 server.begin();
 Serial.println("Web server started!");
}
 
void loop(void){
 data = (String)getDustValue() + "" + (String)getTempValue() + " " + (String)getUvValue();
 
 //data = analogRead(A0);
 Serial.print(data);
 delay(200);
 server.handleClient();
}

double getDustValue()
{
  
  delayMicroseconds(280);
 
  voltsMeasured = analogRead(dustPin); // read the dust value
 
  delayMicroseconds(40);

  delayMicroseconds(9680);
 
  //measure your 5v and change below
  calcVoltage = voltsMeasured * (3.3 / 1024.0);
  dustDensity = 0.17 * calcVoltage - 0.1;

  return dustDensity*100; // doi sang micro/m3 *1000
}

double getTempValue() {
  return bme.readTemperature();
}

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
 
double getUvValue()
{
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
 
  return uvIntensity/10; //to W/m2
}
