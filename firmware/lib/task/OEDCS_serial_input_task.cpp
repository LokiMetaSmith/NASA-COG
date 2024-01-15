/*
  OEDCS_serial_input_task.cpp -- parse serial input to commands
  Copyright (C) 2023 Robert Read.


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

#include <Arduino.h>

#include <debug.h>
#include <OEDCS_serial_input_task.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <stage2_hal.h>

using namespace CogCore;
#define DEBUG_SERIAL_LISTEN 0
#define DEBUG_INPUT 0

namespace CogApp
{
  bool OEDCSSerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("OEDSCSerialTask Inited\n");
    }
    SerialInputTask::_init();
    return true;
  }

  bool OEDCSSerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("executeCommand\n");
    }

    if (ic.com_c == 'S' ||  ic.com_c == 'H' || ic.com_c == 'R' ||
        ic.com_c == 'P' || ic.com_c == 'I' || ic.com_c == 'D') {
      SerialInputTask::executeCommand(ic,mc,smm);
    } else {
      switch(ic.com_c) {
      case 'K':
        {
          if (mc->ms == OffUserAck) {
            CogCore::Debug<const char *>("Having received a acKnowledgment, we are now moving to a normal Off state. You can restart the machine from the Off state.\n");
            mc->ms = Off;
          } else {
            CogCore::Debug<const char *>("The acKnowledgment command is only accepted when you are in the OffUserAck state.\n");          }
        }
        break;
      case 'A':
        {
          float a = min(mc->BOUND_MAX_AMPERAGE_SETTING,ic.value_f);
          a = max(0,a);
          mc->MAX_AMPERAGE = a;
          mc->report->max_stack_amps_A =
            mc->MAX_AMPERAGE;
	  CogCore::Debug<const char *>("Maximum amperage changed to: ");
	  CogCore::Debug<float>(a);
	  CogCore::Debug<const char *>("\n");
        }
        break;
      case 'W':
        {
          float w = min(mc->BOUND_MAX_WATTAGE,ic.value_f);
          w = max(0,w);
          mc->MAX_STACK_WATTAGE = w;
          mc->report->max_stack_watts_W =
            mc->MAX_STACK_WATTAGE;
	  CogCore::Debug<const char *>("Wattage changed to: ");
	  CogCore::Debug<float>(w);
	  CogCore::Debug<const char *>("\n");
          break;
        }
      case 'F':
        {
          float f = min(1.0,ic.value_f);
          f =  max(0,f);
          mc->FAN_SPEED = f;
          mc->report->fan_pwm =
            mc->FAN_SPEED;
	  CogCore::Debug<const char *>("Fan Speed changed to: ");
	  CogCore::Debug<float>(f);
	  CogCore::Debug<const char *>("\n");
          break;
        }
      default:
	CogCore::Debug<const char *>("Unknown command.\n");
        break;
      };
    }
  }

  bool OEDCSSerialInputTask::_run()
  {
    if (DEBUG_SERIAL > 2) {
      CogCore::Debug<const char *>("OEDCS SerialInput Taske Run\n");
    }
    InputCommand ic;
    if (listen(ic)) {
      executeCommand(ic,getConfig(),cogTask);
    }
  }
}
