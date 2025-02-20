/*

  duty_cycle_task.h -- control an AC heater with an SSR

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

#ifndef DUTY_CYCLE_TASK_H
#define DUTY_CYCLE_TASK_H

#include <core_defines.h>
#include <core.h>
#include <OnePinHeater.h>

using namespace CogCore;

/*
  This whole class might seem like overkill;
  why do something so complicated that resemble's
  Bresenham's line drawing algorithm when PWM would work?
  SSR only turn off on a zero-crossing (usually). Since
  the AC frequency is 60 Hz, this is MUCH slower than
  our PWM frequency of 2500 Hz typical of an Arduino.
  It is very unclear to me what would happen if you
  tried to control an SSR with a PWM signal. - rlr
 */

class DutyCycleTask : public CogCore::Task
  {
  public:
    DutyCycleTask();
    Stage2Heater whichHeater;
    float dutyCycle;
    //    const int PERIOD_MS = 50;
    // AmOx has stated that the heater will have longer life
    // if the "time on" is set longer than 50 ms.
    const int PERIOD_MS = 3000;
    // This is is a test to see if it removes our oscilllations
    //    const int PERIOD_MS = 1000;
    int DEBUG_DUTY_CYCLE = 0;
    boolean isOn = false;

    // The recoded_duty_cycle represents the duty cycle
    // inside the time window.
    const unsigned long WINDOW_MS = 3 * 60 * 1000;
    float recorded_duty_cycle = 0;
    unsigned long window_start_ms = 0;
    unsigned long window_end_ms = 0;
    unsigned long prorata_on_in_window_ms = 0;

    unsigned long time_of_last_check = 0;
    float temperature_at_time_of_last_check;
    // Sadly
    const float TEMPERATURE_LOW_LIMIT = -1.0;
    const float TEMPERATURE_HIGH_LIMIT = -1.0;
    const float TEMPERATURE_UPSWING_THRESHOLD = 0.25;

    // By placing a pointer here, we can have
    // individual control of however many heaters are in the system.
    OnePinHeater *one_pin_heater;
  private:
    bool _init() override;
    bool _run() override;
  };


#endif
