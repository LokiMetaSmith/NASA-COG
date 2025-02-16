/*
 panel_blink_task.h - header for task to blink light

Copyright (C) 2025 Robert L. Read

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

#ifndef PANEL_BLINK_TASK_H
#define PANEL_BLINK_TASK_H


#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/array.h"
#include <machine.h>


namespace CogApp
{
    class PanelBlinkTask : public CogCore::Task
    {
    public:
      unsigned long half_period_ms = 500;
      unsigned long time_of_last_change_ms = 0;
    private:
      bool _init() override;
      bool _run() override;

    };
}

#endif
