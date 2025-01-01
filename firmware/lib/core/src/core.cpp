/*
Copyright (C) 2021 Robert Read, Ben Coombs.

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

#include "core.h"
#include <util.h>
#include "debug.h"
#ifdef ARDUINO
#include <Arduino.h>
#include <limits.h>



#define MAX_TIME (ULONG_MAX)
#else
#include <iostream>
// #include "HAL/posix/hal.h"
#define MAX_TIME = (1 << 32)
#endif

#define HARDWARE_WATCHDOG_TIMOUT_MS 16000
void watchdogSetup() {
  // WARNING! HACK! This needs an ESP32 implementation of some kind! - rlr
#ifdef BOARD_DUE
  watchdogEnable(HARDWARE_WATCHDOG_TIMOUT_MS);
#endif
}


namespace CogCore {


  unsigned long num_of_report = 0;
  unsigned long time_since_last_report = 0;
  const unsigned long TIME_TO_REPORT_SCHEDULER_MS = 20*1000;


#define SW_TICK 1
#define TICK_PERIOD 2

  // The WATCHDOG is nice but annoying if too fast;
  // I'm configuring it to every 25 seconds
  // #define WATCHDOG_TIMEOUT_MS 250
#define WATCHDOG_TIMEOUT_MS 25000
  // This is the longest hardware watchdog timeout we are allowed

bool Core::_criticalError = false;

bool Core::Boot() {
    bool result = false;
    //bool Core::_criticalError = false;
    //    ErrorHandler::SetErrorMode(CogCore::ErrorMode::StdOut);
    // TODO: configure/validate HAL

    // SchedulerProperties properties;
    // properties.mode = SchedulerMode::RealTime;
    // properties.tickPeriodMs = TICK_PERIOD;
    // _scheduler.SetProperties(properties);
    //    CreateSoftwareWatchdog(WATCHDOG_TIMEOUT_MS);

    // Note: This CANNOT take a parameter!!
    //    CreateHardwareWatchdog();

    _state = CoreState::Configured;

//     if (_state == CoreState::Configured) {
// #ifndef ARDUINO
//         std::cout << "Boot\n";
// #endif
//         //start();
//         result = true;
//     } else {
//         ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
//     }

    // bool success = _scheduler.Init();
    // if (success == false) {
    //     return false;
    // }
    // _primaryTimer.Init();
    return true;
    return result;
}

bool Core::AddTask(Task *task, TaskProperties *properties) {
  Debug<const char *>("*task, *properties\n");
  DebugLn<long>((long) task);
  DebugLn<long>((long) properties);

    for(int j = 0; j < 5; j++) {
    CogCore::Debug<int>(j);
    CogCore::Debug<const char *>("B SerialReportTask _init()\n");
    delay(1000);
    CogCore::DebugLn<bool>(j < 5);
  }
    bool taskAdded = _scheduler.AddTask(task, properties);
    if (taskAdded) {
#ifndef ARDUINO
        std::cout << "Task Added!\n";
#endif
    } else {
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToAddTask);
    }
    return taskAdded;
}

// 1. Set properties
// 2. Add tasks
// 3. Setup scheduler
// 4. Run scheduler

bool Core::Run() {
    CogCore::Debug<const char *>("Core::Run!\n");

#ifdef SW_TICK
    while (true) {
        if (_criticalError == true) {
	  CogCore::Debug<const char *>("Ending Core Run due to critical error!\n");
          delay(100);
            return false;
        }
        // I'm moving this action to inside the Tick, which checks for rollover.
        //        _elapsed = _primaryTimer.Update();
#endif

        if (DEBUG_CORE > 1) {
          unsigned long m = x_millis();
          if (m < time_since_last_report) {
            time_since_last_report = m;
          }
          if (m > (time_since_last_report + TIME_TO_REPORT_SCHEDULER_MS)) {
	    CogCore::Debug<const char *>("Scheduler Still Alive, Number of Ticks:");
	    CogCore::Debug<uint32_t>(num_of_report++);
	    CogCore::Debug<const char *>("\n");
            time_since_last_report = m;
          }
        }
        Tick();
        ResetHardwareWatchdog();
        bool reset = ResetSoftwareWatchdog();
        if (reset == false) {
	  CogCore::Debug<const char *>("Ending Core Run due to internal Watchdog!\n");
          delay(100);
            return false;
        }
#ifdef SW_TICK
        // Wait to simulate tick interrupt
        // Note: This looks dangerous relative rollovers!
        // TODO: See if we get stuck here on Rollovers and rewrite.
        bool wait_timer = true;
        while (wait_timer) {
          TimeMs ms = _primaryTimer.Update();
          if (ms < _elapsed) { // ROLLOVER EVENT!
            _elapsed = 0;
          }
            uint32_t elapsed_wait = ms - _elapsed;
            if (elapsed_wait >= _scheduler.GetTickPeriod()) {
                wait_timer = false;
            }
        }
    }
#endif
    return false;
}

////////// Private //////////

void Core::Tick() {
    // Debug<const char*>("-----------------------------------------\n");
    TimeMs previous = _elapsed;
    TimeMs current = _primaryTimer.Update();
    TimeMs increment;
    if (current > previous) {
      increment = (current - previous);
    } else if (current == previous) {
      increment = 0;
    } else { // ROLLOVER!
      increment = (MAX_TIME - previous) + current;
    }
    if (DEBUG_CORE > 4) {
	   CogCore::Debug<const char *>("_elapsed: ");
	   CogCore::DebugLn<uint32_t>(_elapsed);
    }
    _elapsed = current;


    if (DEBUG_CORE > 4) {
	   CogCore::Debug<const char *>("current: ");
	   CogCore::DebugLn<uint32_t>(current);
	   CogCore::Debug<const char *>("increment: ");
	   CogCore::DebugLn<uint32_t>(increment);
    }

    // This is the point where we want to detect a clock rollover.
    // We must advance the virtual Time before we call this!!
    _scheduler.advanceVirtualTimeByIncrementAndBaseline(increment);

    TaskState state = _scheduler.RunNextTask();
#ifndef ARDUINO
    //    std::cout << "State: " << static_cast<int>(state) << std::endl;
#endif
}

  //
void Core::CreateSoftwareWatchdog(uint32_t timeoutMs) {
    Debug<const char*>("Create watchdog (todo)\n");
    _watchdogTimer.Init();
}

void Core::ResetAllWatchdogs() {
#ifdef CTL_V_1_1
    ResetHardwareWatchdog();
    ResetSoftwareWatchdog();
#endif
}

  // WARNING! DO NOT CHANGE THIS NAME OR INTERFACE
  // This function is implemented with a weak reference in the core.
  // it MUST be overridden exactly this way.

void Core::CreateHardwareWatchdog() {
#ifdef CTL_V_1_1
    Debug<const char*>("Creating Hardware Watchdog\n");
    watchdogSetup();
#endif
}

void Core::ResetHardwareWatchdog() {
#ifdef CTL_V_1_1
  watchdogReset(); //arduino hardware reset library function
#endif
}

bool Core::ResetSoftwareWatchdog() {
    uint32_t elapsed = _watchdogTimer.Update();
    if (elapsed > WATCHDOG_TIMEOUT_MS) {
          ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::WatchdogExceeded);
        return false;
    } else {
#ifndef ARDUINO
      //        std::cout << "Watchdog reset. Elapsed: " << elapsed << std::endl;
#endif
        _watchdogTimer.Reset();
        return true;
    }
}

uint32_t Core::GetElapsedTime() {
    return _elapsed;
}

void Core::RaiseCriticalError() {
    _criticalError = true;
}

}
