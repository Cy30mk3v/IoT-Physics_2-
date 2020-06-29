int a = 0;
#include <Tone.h>
void setup() {
  // put your setup code here, to run once:
 for(int i=31;i<10000;i++){
 tone(11,i,1000);
 delay(10);
 }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(a++);
  //Serial.println("Hello World!");
  /*digitalWrite(11,LOW);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  digitalWrite(12, HIGH);
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(11, HIGH);
  delay(1000);*/
  for(int j=9;j<12;j++){
  for(int i=0;i < 255;i++){
    analogWrite(j,i);
    delay(10);
  }
  for(int i=254;i < 0;i--){
    analogWrite(j,i);
  delay(10);
  }
  
  }
  
}
