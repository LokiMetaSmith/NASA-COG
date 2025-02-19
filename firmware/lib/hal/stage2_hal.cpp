/*
  stage2_hal.cpp -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

  Copyright 2023, Robert L. Read, Lawrence Kincheloe

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

#include <stage2_hal.h>
#include <debug.h>

Stage2HAL::Stage2HAL() {

  HEATER_PINS[0] = INT1_OUTPUT_PIN;
  HEATER_PINS[1] = EXT1_OUTPUT_PIN;
  HEATER_PINS[2] = EXT2_OUTPUT_PIN; //  Int1,Ext1, Ext2
 
}

bool Stage2HAL::init() {
  pinMode(MAX31850_DATA_PIN, INPUT);

  // TODO: This block of code appears in cog_hal.cpp
  // as well. There should be a way to move this into the
  // superclass, but I can't figure out how to do it.
  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("About to initialize heaters\n");
    CogCore::Debug<uint32_t>(NUM_HEATERS);
    CogCore::Debug<const char *>("\n");
      delay(100);
  }
  _ac_heaters = new OnePinHeater*[NUM_HEATERS];
  for(int i = 0; i < NUM_HEATERS; i++) {
    _ac_heaters[i] = new OnePinHeater();
    _ac_heaters[i]->heater_pin = HEATER_PINS[i];
    _ac_heaters[i]->init();
  }

  if (DEBUG_HAL > 0) {
    CogCore::Debug<const char *>("HAL:About to return!\n");
      delay(100);
  }
  return true;
}

float Stage2HAL::getTemperatureReading(Stage2Heater s2heaterToControl,MachineConfig *mc) {
    float t;
    t =  mc->report->post_heater_C;
    // switch (s2heaterToControl) {
    // case 0:
    //   t = mc->report->post_heater_C;
    //   break;
    // case 1:
    //   t = mc->report->post_getter_C;
    //   break;
    // case 2:
    //   t = mc->report->post_stack_C;
    //   break;
    // default:
    //   CogCore::Debug<const char *>("s2heater_task HeaterControl not set!\n");
    //   break;
    // }
    return t;
  }
