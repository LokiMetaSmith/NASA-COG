/*

  DutyCycle.cpp -- control an AC heater with an SSR

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

#include <duty_cycle_task.h>

const int DEBUG_ISSUE_393 = 1;

/*

  Theory

  There is a problem here in that we are choosing to run this task only once
  every 3 seconds. We are changing the duty cycle far more rapidly (about 500ms).
  We must, therefore, look into the past to compute the duty cycle---we cannot
  compute ON or OFF for the next 3 seconds and then recompute it in the next 3
  seconds without reference to the past.

  Therefore, we will construct a time window of length (at least) W seconds. We will not
  record the full ON/OFF history within this window, but represent the actual duty
  cycle within this window as a single floating point fraction. We store the
  start and end of this window as milliseconds timings.

  The reset_duty_cycle will no longer have any actual purpose and will be removed.
  When the task is run, it will first update the window. It will then use the updated
  recorded window duty cycle against the requested duty cycle to determine whether
  to turn the heater on.
 */

DutyCycleTask::DutyCycleTask() {
}

bool DutyCycleTask::_init()
{
  CogCore::Debug<const char *>("DutyCycleTask init\n");
    if (DEBUG_DUTY_CYCLE > 1) {
      CogCore::Debug<const char *>("DUTY CYCLE RUN!\n");
  }

  return true;
}

/* There is a significant danger here that we will change the duty cycle
   more rapidly than our turn-on time and turn-off time can support. This is
   a rather subtle problem that will have to be thought about carefully.
   I suspect that when we reset we should somehow use the state of the
   system, which was probably either in the middle of an On state or Off
   state.
*/

/* void DutyCycleTask::reset_duty_cycle() {
  recorded_duty_cycle = 0;
  recorded_dc_ms = 0;
  time_of_last_check = millis();
}
*/

// The fundamental purpose of this task is just to
// drive the dynamic from DutyCycle of the heater.
bool DutyCycleTask::_run()
{
  if (DEBUG_DUTY_CYCLE > 0) {
    CogCore::Debug<const char *>("DUTY CYCLE RUN!\n");
  }
  // WARNING: This will fail when 2^32 ms are reached, about 28 days I think.
  // TODO: Figure out how to make this immune to rollover error.
  if (DEBUG_DUTY_CYCLE > 1) {
    CogCore::Debug<const char *>("DUTY CYCLE MID!\n");
  }

  unsigned long ms = millis();
  unsigned long delta_t = ms - time_of_last_check;
  // now we update the recorded duty_cycle weight
  float old_dc = recorded_duty_cycle;
  float old_ms = prorata_on_in_window_ms;
  if (isOn) {
    prorata_on_in_window_ms += delta_t;
  }
  window_end_ms += delta_t;
  // now we have to more the amount on on a pro-rata-basis...
  prorata_on_in_window_ms -= (recorded_duty_cycle * delta_t);
  // now we move the window start_forward only
  // if the total length will remain greater than WINDOW_MS
  long window_length_ms = window_end_ms - window_start_ms;
  if (window_length_ms >= WINDOW_MS + delta_t) {
    window_start_ms += delta_t;
    window_length_ms -= delta_t;
  }

  if (window_length_ms != 0) {
    if (DEBUG_DUTY_CYCLE > 1) {
      CogCore::Debug<const char *>("prorata_on_in_window_ms: ");
      CogCore::Debug<long>(prorata_on_in_window_ms);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("window_length_ms: ");
      CogCore::Debug<long>(window_length_ms);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("window_length_ms - prorata_on_in_window_ms: ");
      CogCore::Debug<long>(window_length_ms - prorata_on_in_window_ms);
      CogCore::Debug<const char *>("\n");
    }
    recorded_duty_cycle = (float) prorata_on_in_window_ms / (float) window_length_ms;
  } else {
    recorded_duty_cycle = 0.0;
  }


  // Now, if we have been on up until now, and the temperature has actually
  // gone down, we probably have a broken heater, and we will mark an error
  if (PERIOD_MS >= 3000) {
    float current_temp = getConfig()->report->post_heater_C;

    if (isOn) {
      float difference = current_temp - temperature_at_time_of_last_check;
      if (DEBUG_DUTY_CYCLE > 0) {
        CogCore::Debug<const char *>("IS ON!");
        CogCore::DebugLn<bool>(isOn);
        CogCore::Debug<const char *>("TESTING\n");
        CogCore::Debug<const char *>("Current Temp: ");
        CogCore::DebugLn<float>(current_temp);
        CogCore::Debug<const char *>("Temp at last time of check: ");
        CogCore::DebugLn<float>(temperature_at_time_of_last_check);
        CogCore::Debug<const char *>("Difference (current temp - Temp at last check): ");
        CogCore::DebugLn<float>(difference);
      }
#ifdef REDUCE_HEATER_UNRESPONSIVE_MIN_TEMP
      float tl =  35.0;
#else
      float tl = 60.0;
#endif
      if ((!getConfig()->errors[HEATER_UNRESPONSIVE].fault_present)) {
        if (current_temp < tl) {
          if (difference <= TEMPERATURE_LOW_LIMIT) {
            getConfig()->errors[HEATER_UNRESPONSIVE].fault_present = true;
            getConfig()->errors[HEATER_UNRESPONSIVE].begin_condition_ms = millis();
            CogCore::Debug<const char *>("HEATER_UNRESPONSIVE THROWN\n");
          }
        } else {
          if (difference <= TEMPERATURE_HIGH_LIMIT) {
            getConfig()->errors[HEATER_UNRESPONSIVE].fault_present = true;
            getConfig()->errors[HEATER_UNRESPONSIVE].begin_condition_ms = millis();
            CogCore::Debug<const char *>("HEATER_UNRESPONSIVE THROWN\n");
          }
        }
        if ((getConfig()->errors[HEATER_UNRESPONSIVE].fault_present) &&
            difference >= TEMPERATURE_UPSWING_THRESHOLD) {
          getConfig()->errors[HEATER_UNRESPONSIVE].fault_present = false;
          CogCore::Debug<const char *>("HEATER_UNRESPONSIVE WITHDRAWN\n");
        }
      }
    }
    // now we decided to turn on or off (until called before)!
    isOn = ((dutyCycle > 0.0) && (recorded_duty_cycle <= dutyCycle));

    // now we actually turn the heater on or off!
    if (DEBUG_DUTY_CYCLE > 1) {
      CogCore::Debug<const char *>("DUTY Heater On: ");
      CogCore::Debug<int>(isOn);
      CogCore::Debug<const char *>("\n");
    }

    if (DEBUG_DUTY_CYCLE > 1) {
      CogCore::Debug<const char *>("Window Length: ");
      CogCore::Debug<long>(window_length_ms);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("Recorded Duty Cycle: ");
      CogCore::Debug<float>(recorded_duty_cycle);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("Duty Cycle: ");
      CogCore::Debug<float>(dutyCycle);
      CogCore::Debug<const char *>("\n");
    }

    time_of_last_check = ms;
    temperature_at_time_of_last_check = getConfig()->report->post_heater_C;

    one_pin_heater->setHeater(0,isOn);

    if (DEBUG_DUTY_CYCLE > 1) {
      CogCore::Debug<const char *>("DUTY HEATERS SET!\n");
    }

    return true;
  }
}
