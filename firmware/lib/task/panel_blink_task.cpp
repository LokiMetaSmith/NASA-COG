/*
  panel_blink_task.cpp - alternate the panel blink status

Copyright (C) 2025 Robert L. Read

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

#include "panel_blink_task.h"
#include <util.h>
#include <cog_hal.h>

using namespace std;


namespace CogApp
{
  bool PanelBlinkTask::_init()
  {
    return true;
  }

  bool PanelBlinkTask::_run()
  {
    unsigned long ms = millis();
    COG_HAL* hal = (COG_HAL *) getConfig()->hal;
    if (hal->panel->status == LED_STATUS::BLINKING) {
      if (ms - time_of_last_change_ms > half_period_ms) {
        hal->panel->setStatusLED(!hal->panel->isStatusLEDOnOrOff());
        time_of_last_change_ms = ms;
      }
    }
    return true;
  }
}
