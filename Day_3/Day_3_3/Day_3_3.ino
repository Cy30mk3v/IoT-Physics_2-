#include<LiquidCrystal.h>

LiquidCrystal lcd(1, 2, 4, 5, 6, 7);
const int inPin = 9;
void setup()
{
  lcd.begin(16, 2);
}
void loop()
{
  int value = analogRead(8);
  lcd.setCursor(0, 1);
  float volts = (value / 965.0) * 5;
  float c = (volts - .5) * 100;
  float f = (c * 9 / 5 + 32);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(c / 5);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(f / 5);
  lcd.print("F");
  delay(1000);
}
