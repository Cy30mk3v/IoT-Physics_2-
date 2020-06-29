
int button =12;
int lastState=LOW;
void setup() {
  
 pinMode(button,INPUT);
 Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 int buttonState=digitalRead(button);
 if(buttonState!=lastState){
  if(buttonState==HIGH){
  Serial.println("Button pressed");
 }
 lastState=buttonState;
  }
  
  
}
