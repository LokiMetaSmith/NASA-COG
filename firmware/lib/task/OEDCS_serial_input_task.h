/*
  OEDCS_serial_input_task.h -- parse serial input to commands
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

#ifndef OEDCS_SERIAL_TASK_H
#define OEDCS_SERIAL_TASK_H

#include <Arduino.h>
#include <core.h>
#include <debug.h>
#include <machine.h>
#include <stage2_hal.h>
#include <stage2_heater_task.h>
#include <cog_task.h>
#include <serial_input_task.h>

#define INPUT_BUFFER_SIZE 256


namespace CogApp
{
  class OEDCSSerialInputTask : public SerialInputTask {
  public:
    CogTask* cogTask;
    
    int PERIOD_MS = 250;
    bool executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) override;
    bool _init() override;
    bool _run() override;
  };

}

#endif
