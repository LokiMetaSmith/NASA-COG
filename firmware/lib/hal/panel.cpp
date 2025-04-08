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
#include <Arduino.h>
#include <debug.h>

void FrontPanel::setFaultLED(bool onOrOff) {
  digitalWrite(PANEL_LED_FAULT,onOrOff);
}

void FrontPanel::setStatusLED(bool onOrOff) {
  digitalWrite(PANEL_LED_STATUS,onOrOff);
}

bool FrontPanel::isStatusLEDOnOrOff() {
  return digitalRead(PANEL_LED_STATUS);
}

bool FrontPanel::isSwitchOn() {
  return digitalRead(PANEL_SWITCH_DETECTION);
}


void FrontPanel::setStatusLEDfromState(LED_STATUS ls) {
  // Now make the panel lights reflect our status...
  status = ls;
  bool onOrOff = false;
  switch (ls) {
  case LED_STATUS::OFF:
    onOrOff = false;
    setStatusLED(onOrOff);
    break;
  case LED_STATUS::BLINKING: {
    break;
  }
  case LED_STATUS::STEADY_ON:
    onOrOff = true;
    setStatusLED(onOrOff);
    break;
  }

  int switchState = analogRead(PANEL_SWITCH_DETECTION);
  if (DEBUG_LEVEL > 0) {
    CogCore::Debug<const char *>("Switch Status : ");
    CogCore::Debug<int>(switchState);
    CogCore::Debug<const char *>("\n");
  }
  bool dswitchState = digitalRead(PANEL_SWITCH_DETECTION);
  if (DEBUG_LEVEL > 0) {
    CogCore::Debug<bool>(dswitchState);
    CogCore::Debug<const char *>("\n");
  }
}
void FrontPanel::init() {
  pinMode(PANEL_LED_FAULT,OUTPUT);
  pinMode(PANEL_LED_STATUS,OUTPUT);
  pinMode(PANEL_SWITCH_DETECTION,INPUT_PULLUP);
  digitalWrite(PANEL_LED_FAULT,false);
  digitalWrite(PANEL_LED_STATUS,false);
}
