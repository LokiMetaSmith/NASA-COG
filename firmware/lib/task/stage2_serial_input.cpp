/*
  stage2_serial_input.cpp -- parse serial input to commands
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
#include <stage2_serial_input.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <stage2_hal.h>

using namespace CogCore;
#define DEBUG_SERIAL_LISTEN 0
#define DEBUG_INPUT 0

namespace CogApp
{
  bool Stage2SerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("OEDSCSerialTask Inited\n");
    }
    SerialInputTask::_init();
    return true;
  }


  bool Stage2SerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("executeCommand\n");
    }

    showParsedData(ic);
    if ((ic.com_c == 'S') ) {
      processStateChange(ic,mc,smm);
    } else if ((ic.com_c == 'H') || (ic.com_c == 'R')
               || ic.com_c == 'P' || ic.com_c == 'I' || ic.com_c == 'D') {
      SerialInputTask::executeCommand(ic,mc,smm);
    } else {
      switch(ic.com_c) {
      case '1':
        {
          mc->hal->s2heaterToControl = Int1;
          Debug<const char *>("Switching to controlling the Int1 Heater!\n");
          return false;
          break;
        }
      case '2':
        {
          mc->hal->s2heaterToControl = Ext1;
          Debug<const char *>("Switching to controlling the Ext1 Heater!\n");
          return false;
          break;
        }
      case '3':
        {
          mc->hal->s2heaterToControl = Ext2;
          Debug<const char *>("Switching to controlling the Ext2 Heater!\n");
          return false;
          break;
        }
      default: {
        CogCore::Debug<const char *>("unknown command: ");
        CogCore::Debug<char>(ic.com_c);
        CogCore::Debug<const char *>("\n");
      }
      }
    }
  }

  bool Stage2SerialInputTask::_run()
  {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("Stage2SerialInputTask run\n");
    }
    InputCommand ic;
    if (listen(ic)) {
      if (DEBUG_SERIAL > 0) {
	CogCore::Debug<const char *>("Got command\n");
        showParsedData(ic);
      }
      executeCommand(ic,
                     mcs[getConfig()->hal->s2heaterToControl],
                     stage2HeaterTasks[getConfig()->hal->s2heaterToControl]);
    }
    return true;
  }
}
