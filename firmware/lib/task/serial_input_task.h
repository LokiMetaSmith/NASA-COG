/*
  serial_input_task.h -- parse serial input to commands
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

#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#include <Arduino.h>
#include <core.h>
#include <debug.h>
#include <machine.h>
#include <stage2_hal.h>
#include <stage2_heater_task.h>
#include <cog_task.h>

#define INPUT_BUFFER_SIZE 256


namespace CogApp
{

  struct InputCommand {
    char com_c;
    char value_c;
    float value_f;
  };

  class SerialInputTask : public CogCore::Task {
  private:
    static const byte numChars = 32;
    char receivedChars[numChars] = {0};
    char tempChars[numChars] = {0};
    char messageFromPC[numChars] = {0};
    int integerFromPC = 0;
    float floatFromPC = 0.0;
    boolean newData = false;
    void recvWithEndMarker();
    InputCommand parseCommandLine();
  public:
    int DEBUG_SERIAL = 0;
    bool _init() override;
    bool _run() override;
    int DEBUG_LEVEL = 0;
    void showParsedData(InputCommand ic);
    virtual bool listen(InputCommand &ic);
    virtual bool executeCommand(InputCommand ic,
                                MachineConfig* mc,
                                StateMachineManager *smm);
    void processStateChange(InputCommand ic,
                            MachineConfig *mc,
                            StateMachineManager *smm);
  };

}

#endif
