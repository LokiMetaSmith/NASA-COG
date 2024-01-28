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


  void SerialInputTask::processStateChange(InputCommand ic,MachineConfig *mc,StateMachineManager *smm) {
    if (ic.value_c == '1') {
      mc->USE_ONE_BUTTON = false;
      if (mc->ms == Off) {
        mc->clearErrors();
        smm->turnOn();
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
      Debug<const char *>("Entering Automatic One-Button Algorithm.");
      mc->clearErrors();
      smm->turnOn();
      smm->changeTargetTemp(mc->TARGET_TEMP_C);
      mc->USE_ONE_BUTTON = true;
    } else if (ic.value_c == '3') {
      Debug<const char *>("Entering Automatic One-Button Algorithm. Set Temp to: ");
      Debug<int>(mc->OPERATING_TEMPERATURE_C);
      Debug<const char *>("\n");
      mc->clearErrors();
      smm->turnOn();
      smm->changeTargetTemp(mc->OPERATING_TEMPERATURE_C);
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
        if ((mc->ms == Warmup) || (mc->ms == Cooldown) || (mc->ms == NormalOperation)) {
          smm->changeTargetTemp(ic.value_f);
          smm->turnOn();
          CogCore::Debug<const char *>("Target Temp changed to: ");
          char t[20];
          sprintf(t, "%10.8f", ic.value_f);
          CogCore::Debug<const char *>(t);
          CogCore::Debug<const char *>("\n");
          CogCore::Debug<const char *>("New state is: ");
          CogCore::Debug<const char *>(MachineConfig::MachineStateNames[mc->ms]);
          CogCore::Debug<const char *>("\n");
        }
      }
      break;
    case 'R':
      {
        float r = min(mc->BOUND_MAX_RAMP_C_PER_MIN,ic.value_f);
        r = max(0.0,r);
        mc->change_ramp(r);
        mc->report->target_ramp_C = r;
	CogCore::Debug<const char *>("Ramp changed to: ");
	CogCore::Debug<float>(r);
	CogCore::Debug<const char *>("\n");
      }
      break;
    default:
      CogCore::Debug<const char *>("Internal Error!\n");
    }
  }
}
