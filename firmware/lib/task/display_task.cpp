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

#include "display_task.h"

using namespace std;

 

    bool DisplayTask::_init()
    {
        CogCore::Debug<const char *>("DisplayTask init\n");

        pinMode(DISPLAY_CS, OUTPUT);      // set the display pin CS pin mode 
		pinMode(DISPLAY_DC, OUTPUT);
		pinMode(DISPLAY_RESET, OUTPUT);
		digitalWrite(DISPLAY_CS, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high) 
		digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
		digitalWrite(DISPLAY_RESET, HIGH);
		 
		delay(5);
        return true;
    }

    bool DisplayTask::_run()
    {
      // Note:adding a display task
      // CogCore::Debug<const char *>("DisplayTask run\n");
        //Toggeling the LED
        //    if (digitalRead(DISPLAY_CS) == LOW) {
         //   digitalWrite(DISPLAY_CS, HIGH);   // turn the LED on (HIGH is the voltage level)
        //    } else {
        //    digitalWrite(DISPLAY_CS, LOW);   // turn the LED on (HIGH is the voltage level)
        //    }     
        return true;
    }


