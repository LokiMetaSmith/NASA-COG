/*
  cog_hal.cpp -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

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

#include <cog_hal.h>
#include <TF800A12K.h>
#include <SanyoAceB97.h>

COG_HAL::COG_HAL() {
	   

}

bool COG_HAL::init() {

  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("HAL: About to init Fan!\n");
  }
   HEATER_PINS[0] = HEATER_PIN;
   
  // This exist purely for the convenience of having another 3.3V signal for testing!
  pinMode(FIXED_HIGH_43, OUTPUT);
  digitalWrite(FIXED_HIGH_43,HIGH);


  pinMode(MAX31850_DATA_PIN, INPUT);
  pinMode(RF_STACK, OUTPUT);

  pinMode(SHUT_DOWN_BUTTON,INPUT_PULLUP);

  _fans[0] = new SanyoAceB97("FIRST_FAN",0);
  _fans[0]->init();

  // TODO: This block of code appears in cog_hal.cpp
  // as well. There should be a way to move this into the
  // superclass, but I can't figure out how to do it.
  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("About to initialize heaters\n");
      delay(100);
  }
  _ac_heaters = new OnePinHeater*[NUM_HEATERS];
  for(int i = 0; i < NUM_HEATERS; i++) {
    _ac_heaters[i] = new OnePinHeater();
    _ac_heaters[i]->heater_pin = HEATER_PINS[i];
    _ac_heaters[i]->init();
  }
  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("HEATERS_INITIALIZED\n");
      delay(100);
  }

  //init  PSU

  _stacks[0] = new SL_PS("FIRST_STACK",0);
  _stacks[0]->init();

  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("HAL:About to return!\n");
  }
  return true;
}

// updateTheFanSpeed to a percentage of the maximum flow.
// We may have the ability to specify flow absolutely in the future,
// but this is genertic.
void COG_HAL::_updateFanPWM(float unitInterval) {
  for (int i = 0; i < NUM_FANS; i++) {
     _fans[i]->updatePWM(unitInterval);
  }
}

bool COG_HAL::isShutDownButtonPushed() {
  bool isPressed = !digitalRead(SHUT_DOWN_BUTTON);
  return isPressed;
}
