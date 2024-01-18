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
#include <BigTreeTechMini12864.h>
#include <machine.h>
using namespace std;

 Mini12864 display;

    bool DisplayTask::_init()
    {
        CogCore::Debug<const char *>("DisplayTask init\n");
       display.Mini12864Setup();
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
		//MachineStatusReport *msr = config->report;
		display.Update_Display();	
        display.updateENC_BUT();		
        return true;
    }


