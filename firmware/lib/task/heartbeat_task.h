/*
 Copyright (C) 2022 Robert L. Read

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

#ifndef HEARTBEAT_TASK_H
#define HEARTBEAT_TASK_H


#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/array.h"
#include <machine.h>


namespace CogApp
{
    class HeartbeatTask : public CogCore::Task
    {
    public:
      unsigned long debug_number_of_heartbeats = 0;
      int DEBUG_HEARTBEAT = 0;
    private:
      bool _init() override;
      bool _run() override;

    };
}

#endif
