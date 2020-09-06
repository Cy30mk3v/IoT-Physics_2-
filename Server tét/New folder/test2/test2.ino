/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

#include <Wire.h>
#include <ML8511.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Replace with your network credentials
const char* ssid = "Redmine";
const char* password = "123456788";

// Initialize Telegram BOT
#define BOTtoken "1320478417:AAEhCo53s1JKpG_w3XU5cDo3ZZOjSf2VUmc"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1061425817"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 16;
const int ledWifiPin = 0;//D3

bool ledState = LOW;

//Dust
int dustPin = A0; 
float voltsMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;


//UV
#define ANALOGPIN     D4
#define ENABLEPIN     D3
ML8511 light(ANALOGPIN, ENABLEPIN);
int UVOUT = A0; //Output from the sensor
int REF_3V3 = D6; //3.3V power on the Arduino board

//Temperature
Adafruit_BME280 bme; // I2C


// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  
  
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/led_on to turn GPIO ON \n";
      welcome += "/led_off to turn GPIO OFF \n";
      welcome += "/state to request current GPIO state \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }

    if (text == "/get_info") {
      sendInfo();
      /*double dust = getDustValue();
      const char* checkDust = checkDustDanger(dust);
  
      float uv = getUvValue();
      const char* checkUv = checkUvDanger(uv);
  
      float temperature = getTempValue();
      const char* checkTemp = checkTempDanger(temperature);
      
      String text = "Temperature: " + (String)temperature + " *C - " + (String)checkTemp + ".\n";
      text += "Uv index: " + (String)uv + " W/cm^2 - " + (String)checkUv + ".\n";
      text += "Air Quality 2.5 PM: " + (String)dust + " mcg/m3 - " + (String)checkDust + ".\n";
      bot.sendMessage(chat_id,text, "");*/
      //bot.sendMessage(chat_id,"Temperature: " + (String)temperature + " " + (String)checkTemp + ".\n", "");
      //bot.sendMessage(chat_id,"Uv index: " + (String)uv + " " + (String)checkUv, "");
      //bot.sendMessage(chat_id,"Air Quality 2.5 PM: " + (String)dust + " " + (String)checkDust, "");
      //ledState = LOW;
      //digitalWrite(ledPin, ledState);
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    client.setInsecure();
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

    bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  }

  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  //double dust = getDustValue();
  //(String)checkDustDanger(dust);
  
  //float uv = getUvValue();
  //const char* checkUv = checkUvDanger(uv);
  
  //double temperature = getTempValue();
  //const char* checkTemp = checkTempDanger(temperature);*/
  delay(1000);
  float temp =  getTempValue();
  double dust = getDustValue();
  float uv = getUvValue();
  
  //setupServer(temp,dust,uv);
  checkLED(checkTempDanger(temp), checkDustDanger(dust), checkUvDanger(uv));
  
  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

void sendInfo(){
  String chat_id = CHAT_ID;
  double dust = getDustValue();
      const char* checkDust = checkDustDanger(dust);
  
      float uv = getUvValue();
      const char* checkUv = checkUvDanger(uv);
  
      float temperature = getTempValue();
      const char* checkTemp = checkTempDanger(temperature);
      
      String text = "Temperature: " + (String)temperature + " *C - " + (String)checkTemp + ".\n";
      text += "Uv index: " + (String)uv + " W/cm^2 - " + (String)checkUv + ".\n";
      text += "Air Quality 2.5 PM: " + (String)dust + " mcg/m3 - " + (String)checkDust + ".\n";
      bot.sendMessage(chat_id,text, "");
}


void checkLED(const char* infoTemp, const char* infoDust, const char* infoUv){
    if ((infoTemp == "Good" || infoTemp == "Moderate") && (infoDust == "Good" || infoDust == "Moderate") && (infoUv == "Good" || infoUv == "Moderate"))  
  {
    Serial.println("Light low");
    digitalWrite(ledPin, LOW);
  }
  else
  {
    Serial.println("Light high");
    digitalWrite(ledPin, HIGH);
    sendInfo();
  }
}

float getTempValue() {
  return bme.readTemperature();
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


const char* checkDustDanger(double dust){
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
 
  return uvIntensity/10; //to W/m2
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
