#include <LiquidCrystal.h>
int temp = 9;

//Khởi tạo với các chân
LiquidCrystal lcd(1, 2, 4, 5, 6, 7);
 
void setup() {
  lcd.begin(16, 2);
  lcd.print("Vo Tran Chi Hung");
  lcd.setCursor(0 , 1);
  lcd.print("");
  
}
 
void loop() {
for (int i = 0 ;i < 14;i++){
  lcd.scrollDisplayLeft();
  delay(400);
}
for (int i = 0 ;i < 29;i++){
  lcd.scrollDisplayRight();
  delay(400);
}
for (int i = 0 ;i < 15;i++){
  lcd.scrollDisplayLeft();
  delay(400);
}
}
