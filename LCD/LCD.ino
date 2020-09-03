/*
* NodeMCU I2C 16x2 LCD
* Created by TheCircuit
*/

#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  //Wire.begin(2,0);
  lcd.begin();   // initializing the LCD

  lcd.backlight(); // Enable or Turn On the backlight 
    // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(5, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("HELLO");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(5, 1);      
  lcd.print("WORLD");
}

void loop()
{
  // Nothing Absolutely Nothing!
}
