/*
  serial_input_task.cpp -- parse serial input to commands
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
#include <serial_input_task.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <stage2_hal.h>

using namespace CogCore;
#define DEBUG_SERIAL_LISTEN 0
#define DEBUG_INPUT 0

namespace CogApp
{
  void SerialInputTask::recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false) {
      rc = Serial.read();
      // We'll do an echo here, we have to do it explicitly...
      CogCore::Debug<char>(rc);

      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0';
        ndx = 0;
        newData = true;
      }
    }
  }

  InputCommand SerialInputTask::parseCommandLine() {
    InputCommand ic;

    char * strtokIndx;

    strtokIndx = strtok(tempChars,":");
    strcpy(messageFromPC, strtokIndx);
    ic.com_c = toupper ( tempChars[0] );

    strtokIndx = strtok(NULL, ":");
   if (ic.com_c == 'S') {
      ic.value_c = toupper( strtokIndx[0]) ;
    } else {
      floatFromPC = atof(strtokIndx);
      ic.value_f = floatFromPC;
    }
    return ic;
  }

  void SerialInputTask::showParsedData(InputCommand ic) {
    CogCore::Debug<const char *>("Command ");
    CogCore::Debug<char>(ic.com_c);
    CogCore::Debug<const char *>("\n");
    if (ic.com_c == 'S') {
      CogCore::Debug<const char *>("State ");
      CogCore::Debug<char>(ic.value_c);
    CogCore::Debug<const char *>("\n");
    } else {
      CogCore::Debug<const char *>("Value ");
      char t[20];
      sprintf(t, "%10.8f", ic.value_f);
      CogCore::Debug<const char *>(t);
      CogCore::Debug<const char *>("\n");
    }
  }

  // true if a new command found
  bool SerialInputTask::listen(InputCommand &ric) {
    if (DEBUG_SERIAL > 2) {
      CogCore::Debug<const char *>("listening...\n");
    }
    recvWithEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      InputCommand ic = parseCommandLine();
      ric.com_c = ic.com_c;
      ric.value_c = ic.value_c;
      ric.value_f = ic.value_f;
      newData = false;
      return true;
    }
    return false;
  }

  bool SerialInputTask::_init() {
    newData = false;
    return true;
  }

  bool OEDCSSerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("OEDSCSerialTask Inited\n");
    }
    SerialInputTask::_init();
    return true;
  }
  bool Stage2SerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("OEDSCSerialTask Inited\n");
    }
    SerialInputTask::_init();
    return true;
  }

  void SerialInputTask::processStateChange(InputCommand ic,MachineConfig *mc,StateMachineManager *smm) {
    if (ic.value_c == '1') {
        mc->USE_ONE_BUTTON = false;
      if (mc->ms == Off) {
        mc->clearErrors();
        smm->changeTargetTemp(mc->TARGET_TEMP_C);
        Debug<const char *>("Turning on: New State: Warmup!");
      } else {
        Debug<const char *>("Already On.");
      }
    } else if (ic.value_c == '0') {
      if (mc->ms != Off) {
        mc->ms = Off;
        Debug<const char *>("New State: Off.\n");
      } else {
        Debug<const char *>("Already Off.\n");
      }
    } else if (ic.value_c == '2') {
        Debug<const char *>("Enterring Automatic One-Button Algorithm.");
        mc->clearErrors();
        smm->changeTargetTemp(mc->TARGET_TEMP_C);
        mc->USE_ONE_BUTTON = true;
    } else {
    }
  }

  bool SerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      CogCore::Debug<const char *>("Serial Input Task executeCommand\n");
    }

    switch(ic.com_c) {
    case 'S': // set state based on the next character
      processStateChange(ic,mc,smm);
      break;
    case 'H':
      {
        smm->changeTargetTemp(ic.value_f);
	CogCore::Debug<const char *>("Target Temp changed to: ");
      char t[20];
      sprintf(t, "%10.8f", ic.value_f);
      CogCore::Debug<const char *>(t);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("New state is: ");
      CogCore::Debug<const char *>(MachineConfig::MachineStateNames[mc->ms]);
      CogCore::Debug<const char *>("\n");
      }
      break;
    case 'R':
      {
        float r = min(mc->BOUND_MAX_RAMP,ic.value_f);
        r = max(0.0,r);
        mc->change_ramp(r);
        mc->report->target_ramp_C = r;
	CogCore::Debug<const char *>("Ramp changed to: ");
	CogCore::Debug<float>(r);
	CogCore::Debug<const char *>("\n");
      }
      break;
    case 'P':
      {
        double P = ic.value_f;
        double I = smm->heaterPIDTask->GetKi();
        double D = smm->heaterPIDTask->GetKd();
        smm->heaterPIDTask->SetTunings(P,I,D);
        smm->heaterPIDTask->printTunings();
      }
      break;
    case 'I':
      {
        double P = smm->heaterPIDTask->GetKp();
        double I = ic.value_f;
        double D = smm->heaterPIDTask->GetKd();
        smm->heaterPIDTask->SetTunings(P,I,D);
        smm->heaterPIDTask->printTunings();
      }
      break;
    case 'D':
      {
        double P = smm->heaterPIDTask->GetKp();
        double I = smm->heaterPIDTask->GetKi();
        double D = ic.value_f;
        smm->heaterPIDTask->SetTunings(P,I,D);
        smm->heaterPIDTask->printTunings();
      }
      break;
    default:
      CogCore::Debug<const char *>("Internal Error!\n");
    }
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
