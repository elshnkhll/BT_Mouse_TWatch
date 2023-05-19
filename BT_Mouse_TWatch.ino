/*

┌───────────┬─┐
│ BY ELSHAN │ │ 
│ KHALILOV  │ │ 
├───────────┼─┤ 
└───────────┴─┘ 

BT_Mouse_TWatch
---------------

Copyright © 2023 by ELSHAN KHALILOV

Permission is hereby granted, free of charge, 
to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
OR OTHER DEALINGS IN THE SOFTWARE.

-----------------------------------------

This example turns the TTGO T-WATCH 2020 into a Bluetooth LE Mouse 
that allows you to position cursor on the screen with precision
and generate left and right button clicks.

Dependencies:
  1. https://github.com/T-vK/ESP32-BLE-Mouse.git
  2. https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library

*/



#include <BleMouse.h>
#include "config.h"


BleMouse bleMouse("TTGO BT Mouse", "LilyGO");
TTGOClass *ttgo;
AXP20X_Class *power;

char buf[128];

bool touch_down = false;
bool moved = true;

int16_t x, y;
int16_t xx = 120, yy = 120;

void setup() {

  Serial.begin(115200);
  Serial.println("Starting...");
  bleMouse.begin();

  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();  // backlight ON


  power = ttgo->power;
  int bat_prcntg = power->getBattVoltage() / 100;
  bleMouse.setBatteryLevel( bat_prcntg );

  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextFont(2);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->drawString("T-Watch Touch Test", 62, 90);
  delay(1000);

  ttgo->closeBL(); // backlight OFF

}

void loop() {

  unsigned long startTime;

  if (digitalRead(TOUCH_INT) == LOW) {
    if (ttgo->getTouch(x, y)) {

      if ( !touch_down ) {
        xx = x;
        yy = y;
        touch_down = true;
        moved = false;
        Serial.print("[");
        delay(50);
      } else {
        moved = true;
        // sprintf(buf, "x:%03d  y:%03d", x, y);
        // snprintf(buf, 128, "V:%.2f ", power->getBattVoltage() / 100 );
        // ttgo->tft->drawString(buf, 80, 118);
        if (bleMouse.isConnected()) {
          startTime = millis();
          while (millis() < startTime + 30) {
            bleMouse.move( (x - xx), (y - yy));
            delay(5);
          }
          xx = x;
          yy = y;
        }
      }
    } else {
      if ( touch_down ) {
        if ( !moved ) {
          if ( x < 120 ) {
            bleMouse.click( MOUSE_LEFT );
          } else {
            bleMouse.click( MOUSE_RIGHT );
          }
        }
        Serial.println("]");
        touch_down = false;
      }
    }
  }

  int bat_prcntg = power->getBattVoltage() / 100;
  bleMouse.setBatteryLevel( bat_prcntg );

  delay(50);
}

