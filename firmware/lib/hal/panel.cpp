/*
  panel.cpp -- the front panel lights and switch

  Copyright 2023, Robert L. Read

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <panel.h>
#include <gpio_pin_defs.h>
#include <arduino.h>

void FrontPanel::setFaultLED(bool onOrOff) {
  digitalWrite(PANEL_LED_FAULT,onOrOff);
}
void FrontPanel::setStatusLED(bool onOrOff) {
  digitalWrite(PANEL_LED_STATUS,onOrOff);
}
void FrontPanel::setStatusLEDfromState(LED_STATUS ls) {
  // Now make the panel lights reflect our status...

  bool onOrOff = false;
  switch (ls) {
  case LED_STATUS::OFF:
    onOrOff = false;
    break;
  case LED_STATUS::BLINKING: {
    unsigned long int time_now_ms = millis();
    onOrOff = ((time_now_ms / 1000) & 2);
  }
    break;
  case LED_STATUS::STEADY_ON:
    onOrOff = true;
    break;
  };
  setStatusLED(onOrOff);
  setStatusLED(true);
}
void FrontPanel::init() {
  pinMode(PANEL_LED_FAULT,OUTPUT);
  pinMode(PANEL_LED_STATUS,OUTPUT);
  pinMode(PANEL_SWITCH_DETECTION,OUTPUT);
};
