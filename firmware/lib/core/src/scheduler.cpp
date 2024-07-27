
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

//#include "HAL/posix/hal.h"
#include "scheduler.h"
#include "util.h"
#include "debug.h"
#include <error_handler.h>

namespace CogCore {
Scheduler::Scheduler()
{
	_properties.mode = SchedulerMode::RealTime;
}

void Scheduler::setupIdleTask() {
    _idleTask._properties.priority = static_cast<TaskPriority>(TaskPriorityOS::Idle);
    _idleTask._properties.period = 1;
    _idleTask._properties.id = 0;
}

  // We should change this so that the parameter means
  // not the currenTime, but how much to advance time by.
  // By creating a "virtual tine" in this way, we become impervious to
  // clock rollover, and in fact become completely abstract, and never
  // actually call millis() or x_millis() with this code.

  // void Scheduler::decrementVirtualTimesToBaseline(TimeMs ms) {
  //   for (int i = 0; i < map.getCount(); i++) {
  //       Task *task = map.getValueByIndex(i);
  //       task->_ms_since_last_run -= ms;
  //   }
  //   virtualTime_ms -= ms;
  // }

  void Scheduler::incrementVirtualTimes(TimeMs ms) {
    for (int i = 0; i < map.getCount(); i++) {
        Task *task = map.getValueByIndex(i);
        task->_ms_since_last_run += ms;
    }
    virtualTime_ms += ms;
  }

  void Scheduler::advanceVirtualTimeByIncrementAndBaseline(TimeMs increment) {
    // In order to atomically change the VirtualTime, must we must add the
    // same amount to all of the _lastRunAgo_ms fields.
    // This is a very large sentinel value;

    if (DEBUG_SCHEDULER > 4) {
	   CogCore::Debug<const char *>("increment: ");
	   CogCore::DebugLn<uint32_t>(increment);
    }

    incrementVirtualTimes(increment);
    // TimeMs min_time_ago = MAX_TIME;
    // boolmax_time_found = false;
    // for (int i = 0; i < map.getCount(); i++) {
    //     Task *task = map.getValueByIndex(i);
    //     TimeMs this_time_ms = task->GetLastRunTime();
    //     if (this_time_ms > min_time_ago) {
    //       min_time_ago = this_time_ms;
    //       max_time_found = true;
    //     }
    // }
    // // Now we can substract the minimum times

    // if (DEBUG_SCHEDULER > 4) {
	//    CogCore::Debug<const char *>("min_time: ");
	//    CogCore::DebugLn<uint32_t>(min_time);
    // }
    // if (min_time_found) {
    //   decrementVirtualTimesToBaseline(min_time);
    // }
  }
  Task* Scheduler::getNextTaskToRun() {
    // Record how long the previous task took to run
  if (DEBUG_SCHEDULER > 1) {
    //    CogCore::Debug<const char *>("getNextTask: ");
    //    CogCore::DebugLn<uint32_t>(x_millis());
    CogCore::Debug<const char *>("virtualTime_ms: ");
    CogCore::DebugLn<uint32_t>(virtualTime_ms);
  }

    Task* nextTask = nullptr;
    TimeMs maxTimeUntilDeadline = -99999;
    for (int i = 0; i < map.getCount(); i++) {
        Task *task = map.getValueByIndex(i);

        TimeMs ms_since_last_run = task->TimeSinceLastRunMs();
        TimeMs period = task->GetPeriod();

        // The next task to run is the most positive one
        // < 0 time remaining to run
        // = 0 due now
        // > 0 time-to-runoverrun
       task->_timeUntilDeadline = ms_since_last_run - period;

        if (DEBUG_SCHEDULER > 1) {
        double x = ms_since_last_run;
        double y = period;
        double z = virtualTime_ms;
        double q = z - (x+y);
	      CogCore::Debug<const char *>("Q Deadline: ");
          CogCore::DebugLn<double>(q);
	      CogCore::Debug<const char *>("LastRunTimeAgo: ");
          CogCore::DebugLn<uint32_t>(ms_since_last_run);
	      CogCore::Debug<const char *>("timeUntilDeadline: ");
	      CogCore::Debug<const char *>(task->_properties.name);
	      CogCore::Debug<const char *>(" ");
          CogCore::DebugLn<int32_t>( task->_timeUntilDeadline);
              //	      CogCore::Debug<int32_t>(task->_timeUntilDeadline);
	      CogCore::Debug<const char *>("\n");
        }

        if (task->_timeUntilDeadline > maxTimeUntilDeadline) {
            maxTimeUntilDeadline = task->_timeUntilDeadline;
            nextTask = task;
        }

        // // this is a rather confusing way to do this math.
        // if ((-1 * nextTask->_timeUntilDeadline) > nextTask->_lastRunDuration) {
        //     nextTask = nullptr;
        // }
    }
  if (DEBUG_SCHEDULER > 0) {
    if (nextTask == nullptr) {
      CogCore::Debug<const char *>("nextTask: IDLE\n");
    } else {
      CogCore::Debug<const char *>("nextTask\n");
      CogCore::Debug<const char *>(nextTask->_properties.name);
      CogCore::Debug<const char *>("\n");
      if (DEBUG_SCHEDULER > 2)
        delay(50);
    }
  }

    if (nextTask == nullptr) {
        nextTask = &_idleTask;
    }
    return nextTask;
}

bool Scheduler::AddTask(Task *task, TaskProperties *properties) {
    if (WithinArrayBounds(_numberOfTasks, MAX_TASKS)) {
        TaskState state = task->Init(properties);
        if (state == TaskState::Ready) {
            map.add(properties->id, task);
            _numberOfTasks++;
            return true;
        } else {
            ErrorHandler::Log(ErrorLevel::Error, ErrorCode::CoreFailedToAddTask);
        }
    }
    // Out of bounds

    return false;
}

bool Scheduler::Init() {
    switch (_properties.mode) {
        case SchedulerMode::RoundRobin:
            //
            return true;
        break;
        case SchedulerMode::RealTime:
            //
            return true;
        break;
        default:
            return false;
        break;
    }
}

  // Note our naming here is inconsistent...
  // This makes this look like it is a timestamp,
  // but when it is called it looks like a duration.
  // I think this is actoung the time elapsed since the timer started,
  // Though that is not Init'ed only once in the current code.

TaskState Scheduler::RunNextTask() {

  Task* nextTask = nullptr;

    switch (_properties.mode) {
        case SchedulerMode::RoundRobin:
            ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::NotImplemented);
        break;
        case SchedulerMode::RealTime:
            nextTask = getNextTaskToRun();
        break;
        default:
            ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::NotImplemented);
        break;
    }

    // Calling the clock is dangerous, but this is short-duration
    // and inside debugging code.
    unsigned long ms ;
  if (DEBUG_SCHEDULER > 1) {
    CogCore::Debug<const char *>("About to Run task!\n");
    CogCore::Debug<const char *>(nextTask->_properties.name);
    CogCore::Debug<const char *>(" : ");
    ms = x_millis();
    CogCore::DebugLn<uint32_t>(ms);
    CogCore::Debug<const char *>("\n");
  }
    nextTask->Run();
  if (DEBUG_SCHEDULER > 1) {
    CogCore::Debug<const char *>("Finished Run! ");
    CogCore::Debug<const char *>(" : ");
    CogCore::DebugLn<uint32_t>(x_millis() - ms);
    CogCore::Debug<const char *>("\n");
  }
  //    _lastTaskRan = nextTask;
    return nextTask->GetState();
}

TaskState Scheduler::RunTaskById(TaskId id) {
    Task* tp = map.getValue(id);
    if (tp == nullptr) {
        return TaskState::Error;
    } else {
        _currentRunningTaskId = id;
        tp->Run();
        return TaskState::Running;
    }
}

TaskId Scheduler::GetRunningTaskId() const {
    return _currentRunningTaskId;
}

Task* Scheduler::GetTaskById(TaskId id) {
    Task* tp = map.getValue(id);
    return tp;
}

void Scheduler::SetProperties(SchedulerProperties properties) {
    _properties = properties;
}

SchedulerProperties Scheduler::GetProperties() {
    return _properties;
}

uint32_t Scheduler::GetTickPeriod() {
    return _properties.tickPeriodMs;
}

}
