/*
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


#ifndef MACHINE_CORE_DEFS_H
#define MACHINE_CORE_DEFS_H

enum MachineState {
  // Off is the initial state. It is a zero-power state.
  Off,
  // Attempting to reach operational temperatures.
  Warmup,
  // Operating
  NormalOperation,
  // Attempt to cool down slowly
  Cooldown,
  // A critical fault has occurred or an acknowledgment has not been received
  CriticalFault,
  // Emergency Shutdown: stop power consumption as quickly as possible
  EmergencyShutdown,
  // Remain Off until a user releases this state
  OffUserAck
};

enum IdleOrOperateSubState {
  // Operate means to produce maximum oxygen. It is the default substates
  Operate,
  // Idle means to produce minimum possible oxygen, but stay warm.
  Idle
};

// this is which of the Stage2Heaters we operate on.
// This also will control which thermocouple to use.
const int NUM_STAGE2_HEATERS = 3;

// TODO: Move Stage2HAL
enum Stage2Heater {
  Int1,
  Ext1,
  Ext2
};

class CriticalError {
public:
  bool fault_present;
  unsigned long begin_condition_ms;
  unsigned long toleration_ms;
  MachineState response_state;
};

#define NUM_CRITICAL_ERROR_DEFINITIONS 15
// WARNING! Do not reorder these!!
// The code currently depends on the 0,1, and 2 being the the thermocouple errors.enum CriticalErrorCondition {
enum CriticalErrorCondition {
  POST_HEATER_TC_BAD,
  POST_GETTER_TC_BAD,
  POST_STACK_TC_BAD,
  COULD_NOT_INIT_3_THERMOCOUPLES,
  FAN_LOSS_PWR,
  PWR_12V_BAD,
  PWR_24V_BAD,
  FAN_UNRESPONSIVE,
  HEATER_UNRESPONSIVE,
  HEATER_OUT_OF_BOUNDS,
  STACK_LOSS_CTL,
  PSU_UNRESPONSIVE,
  MAINS_LOSS_PWR,
  SYSTEM_OVER_TEMPERATURE,
  UNABLE_TO_RAISE_TEMPERATURE_SECURELY
};



struct MachineStatusReport {
  MachineState ms;
  // These targets are the "5knob" targets;
  float setpoint_temp_C;
  float target_temp_C;
  float max_stack_amps_A;
  float max_stack_watts_W;
  float target_ramp_C;
  float fan_pwm;
  float target_fan_pc;

  // currently unused
  float total_wattage_W;

  float post_heater_C;
  float post_stack_C;
  float post_getter_C;
  float stack_voltage;
  float stack_amps;
  float stack_ohms;
  float stack_watts;
  float fan_rpm;
  float heater_duty_cycle;

  unsigned long timestamp;
  CriticalError errors[NUM_CRITICAL_ERROR_DEFINITIONS];
};

#endif
