const int trig = 2;     // chân trig của HC-SR04
const int echo = 3;     // chân echo của HC-SR04

int light_pin = 7;
int distance_sensor = 8;
int LED = 13;
int sound = 12;
void setup()
{
    pinMode(light_pin,INPUT);
    pinMode(LED,OUTPUT);
    pinMode(sound, OUTPUT);
    pinMode(trig,OUTPUT);
    pinMode(echo,INPUT);
    Serial.begin(9600);    
   
}

float getDistance(){
  long duration, distanceCM;
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,HIGH);

  duration = pulseIn(echo,HIGH);
  distanceCM = duration*0.034/2;
  return distanceCM;
  
}
void loop()
{
    int value = digitalRead(light_pin);
    
    float distance = getDistance();
    Serial.println("Distance: ");
    Serial.println(distance);
    
    int check = 1;
    if(distance<20.00){
      check = 0;
    }
    if(!value && check){
      
      digitalWrite(LED,HIGH);
      digitalWrite(sound,HIGH);
    }
    else{
    digitalWrite(LED,LOW);
    digitalWrite(sound,LOW);  
    }
    
    delay(500);
}
