/*
Public Invention's COG Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#include "task.h"
#ifdef ARDUINO
//
#else
#include <iostream>
#endif

#include <debug.h>

namespace CogCore
{

    TaskState Task::Init(TaskProperties *properties)
    {
        if (_state == TaskState::Undefined)
        {
          _properties = *properties;

           _state = _init() ? TaskState::Ready : TaskState::Error;
        }
        return _state;
    }

  // Argument is the "virtual time" in the scheduler
    void Task::Run()
    {
        if (_state == TaskState::Ready) {
            _state = TaskState::Running;
            // We are initating the run 0 ms from the current virtual time
            _ms_since_last_run= 0;
            if (DEBUG_TASK > 0) {
              CogCore::Debug<const char *>("about to _run\n");
            }
            _run(); // TODO: use result
            if (DEBUG_TASK > 0) {
              CogCore::Debug<const char *>("finished _run\n");
            }

        } else {
            // Not ready to run
            // TODO: report
        }
        _state = TaskState::Ready;
        if (DEBUG_TASK > 0) {
          CogCore::Debug<const char *>("Returning Run\n");
        }

    }

    TaskState Task::Wait(TimeMs now)
    {
        if (_state == TaskState::Running) {
            // TODO: do waiting stuff
            _state = TaskState::Ready;
        }
        return _state;
    }

    TaskId Task::GetId() const
    {
        return _properties.id;
    }

    TaskPriority Task::GetPriority() const
    {
        return _properties.priority;
    }

    TaskState Task::GetState() const
    {
        return _state;
    }

  // This is going to change to how long ago the task was run
  // against the virtual time (not the actual time.)
  // That means that each time we call get NextTaskToRun,
  // we have to advance virtual time. The Scheduler is a
  // Friend class and can set this directly.
    TimeMs Task::TimeSinceLastRunMs() const
    {
        return _ms_since_last_run;
    }

    TimeMs Task::GetPeriod() const
    {
        return _properties.period;
    }


    void Task::SetPeriod( TimeMs newPeriod)
    {
		if(newPeriod != _properties.period)
		{
         _properties.period = newPeriod;
		}
    }

  MachineConfig *Task::getConfig() {
    return  (MachineConfig *) _properties.state_and_config;
  }


    /*bool Task::Callback(char *message)
    {
        std::cout << "Task Callback: " << message << std::endl;
        return true;
    }*/

}
