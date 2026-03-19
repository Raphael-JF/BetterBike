/* Grove-RGB_LCD.ino
   This code will set the color of the LCD backlight and keep counting
   the seconds since the last reset of the Arduino
*/

#include "Arduino.h"
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

//Color code, change it to whatever you want
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

void setup() {
  // put your setup code here, to run once:
  // set up the number of columns and rows in the LCD
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  // print a message to the LCD
  lcd.print("Hello World!");
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  // set the cursor to the column 0, line 1
  //(note: line 1 is the second row, since counting begins with 0)
  lcd.setCursor(0,1);
  //print the number of seconds since reset:
  lcd.print(millis()/1000);
  delay(100);
}