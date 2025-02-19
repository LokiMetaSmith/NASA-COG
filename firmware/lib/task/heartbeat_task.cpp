/*
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

#include "heartbeat_task.h"

using namespace std;


namespace CogApp
{
  bool HeartbeatTask::_init()
  {
    CogCore::Debug<const char *>("HeartbeatTask init\n");

    pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

    return true;
  }

  bool HeartbeatTask::_run()
  {
    debug_number_of_heartbeats++;
    // Note:adding a heartbeat task
    // CogCore::Debug<const char *>("HeartbeatTask run\n");
    //Toggeling the LED
    bool beat_high = digitalRead(LED_BUILTIN);
    if (beat_high == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
    }

    beat_high = digitalRead(LED_BUILTIN);
    if (DEBUG_HEARTBEAT > 0) {
        CogCore::Debug<const char *>("Setting HEARTBEAT: ");
        CogCore::DebugLn<bool>(beat_high);
        CogCore::Debug<const char *>("beats / runtime, should trend to 2.0: ");
        CogCore::DebugLn<float>((float) debug_number_of_heartbeats * 1000.0 / (float) millis());
            }     
        return true;
    }

}
