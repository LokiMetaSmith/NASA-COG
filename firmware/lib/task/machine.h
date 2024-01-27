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

#ifndef MACHINE_H
#define MACHINE_H

// Hardware Abstraction Layer
// #include "SensirionSFM3X00.h"
//#include <SanyoAceB97.h>
//#include <abstract_fan.h>

#include <OnePinHeater.h>

#include <machine_script.h>
#include "../collections/circular_array.h"



#define HAND_TEST 1

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef RIBBONFISH
// #define RF_FAN 2
#define RF_HEATER 3
#define RF_STACK DAC0
#define SENSE_12V A2
#define SENSE_24V A1
#define MAX31850_DATA_PIN 5
// #define RF_FAN_TACH 5
// This is obsolete
#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER

#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75
// This is the order in which the thermocouples are wired;
// in a perfect world we might use device address
#define POST_STACK_0_IDX 0
#define POST_HEATER_0_IDX 1

#elif STAGE2_HEATER

// WARNING! These values are obsolete.
// There is probably no real dependence on these.
// They should be hunted down and removed.
#define MAX31850_DATA_PIN 5
// This is obsolete
#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER
// #define RF_FAN DAC1 //DIFFERENT FOR STAGE2_HEATER
#define RF_STACK DAC0
#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75

#endif
//Name the pins from the Due
#define DISPLAY_CS 48 // display LOW->Enabled, HIGH->Disabled
#define DISPLAY_DC 47 //display data / command line, keep high for display cs control
#define DISPLAY_RESET 46 // display reset, keep high or don't care



#include <machine_core_defs.h>

#ifdef STAGE2_HEATER
#include <stage2_config.h>
#endif


class MachineHAL {
public:

  OnePinHeater **_ac_heaters;
  int DEBUG_HAL = 0;
  Stage2Heater s2heaterToControl = Int1;

  bool init_heaters();
  virtual bool init() = 0;
};

#define NUM_CRITICAL_ERROR_DEFINITIONS 14
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
  SYSTEM_OVER_TEMPERATURE
};

constexpr inline static char const *CriticalErrorNames[NUM_CRITICAL_ERROR_DEFINITIONS] = {
    "Post Heater TC-A Bad",
    "Post Getter TC-B Bad",
    "Post Stack  TC-C Bad",
    "Can not init three TC's",
    "Fan Power Loss",
    "Lost 12v Power",
    "Lost 24v Power",
    "Fan TACH unresponsive",
    "Lost control of Heater",
    "pid pegged, temp out of bounds",
    "Lost control of the Stack",
    "Lost control of the programmable PSU",
    "Lost mains power, on UPS",
    "System Over Temperature"
  };

class CriticalError {
public:
  bool fault_present;
  unsigned long begin_condition_ms;
  unsigned long toleration_ms;
  MachineState response_state;
};


// These are the controllable pre-set parameters to the algorithm
// that can be tuned (before the algorithm is running).
class PreSetParameters  {
public:
  const float M_w = 250.0; // stack wattage at 0 wafer difference
  const float Q_c = 30.0;  // maximum wafer difference
};

class MachineConfig {
public:
  MachineConfig();

  PreSetParameters p;

  CriticalError errors[NUM_CRITICAL_ERROR_DEFINITIONS];
void change_ramp(float ramp);


  // TEST CONFIGURATION PARAMETERS
  // ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
  // THEY ARE "HARD_WIRED" HERE.
  // Edit these directly and re-upload to run a different test.
  // This test is designed to abort the processeor when done.


  // These are the so called "5 knob" parameters that
  // can be dynamically changed through the serial port.

  // I am going to have these two move together as one.
  // There is only one Ramp parameter, even though we use
  // two numbers
  float RAMP_UP_TARGET_D_MIN = 0.5; // R (degrees C per minute)
  float RAMP_DN_TARGET_D_MIN = -0.5; // R (degrees C per minute)
    float TARGET_TEMP_C = 30.0; // This is the goal target

  float MAX_AMPERAGE = 30.0; // A (Amperes)
  float MAX_STACK_WATTAGE = 250.0; // W (Wattage)

  float FAN_SPEED = 0.0; // F (fraction between 0.0 and 1.0)

  unsigned long BEGIN_DN_TIME_MS = 0;
  unsigned long BEGIN_UP_TIME_MS = 0;

  float SETPOINT_TEMP_C = 30.0; // This is the CURRENT setpoint, which ramps up or down to TARGET_TEMP.

  // TODO: Need to check this.

  // The beginning temperature of the current warming
  // or cooling cycle.
   float COOL_DOWN_BEGIN_TEMP;
   float WARM_UP_BEGIN_TEMP;




  // TODO: This would better be attached to the statemanager
  // class, as it is used in those task---but also in the
  // separate temp_refresh_task. Until I can refactor
  // temp_refresh_task by placing its funciton in the
  // state manager, I need this gloabl.
  float GLOBAL_RECENT_TEMP = 30.0;


void _reportFanSpeed();

  static const int NUM_MACHINE_STATES = 8;

  constexpr inline static char const *MachineStateNames[8] = {
    "Off",
    "Warmup",
    "NormalOperation",
    "Cooldown",
    "CriticalFault",
    "EmergencyShutdown",
    "OffUserAck"
  };
  constexpr inline static char const *MachineSubStateNames[2] = {
    "(Not Idling)",
    "(Idling)"
  };
  constexpr inline static char const *TempLocationNames[2] = {
    "Post Heater",
    "Post Stack"
  };

  constexpr inline static char const *HeaterNames[3] = {
    "Int1",
    "Ext1",
    "Ext2"
  };

  MachineState ms;
  // This is used to make decisions that happen at transition time.
  MachineState previous_ms;
  bool IS_STAGE2_HEATER_CONFIG = false;
  Stage2Heater s2heater;

  MachineScript* script;

  IdleOrOperateSubState idleOrOperate = Operate;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usually 0-255) but this should be
  // the last step.
  float fanDutyCycle = 0.0;

  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};
  int post_getter_indices[1] = {2};

  MachineHAL* hal;

  MachineStatusReport *report;

  bool init();

  //CogCollections::CircularArray<MachineStatusReport, MAX_RECORDS> _log_entry;
  //  void dumpAllData10Hz();


  void outputReport(MachineStatusReport *msr);
  void createJSONReport(MachineStatusReport *msr, char *buffer);

  // Stage2 specific stuff; this should be handled
  // as a subclass, not a decorator, but I don't have time for that,
  // and it puts the main code at risk, so adding it in here is
  // reasonable - rlr

  void outputStage2Report(Stage2Heater s2h,MachineStatusReport *msr,
                          float target_temp,
                          float setpoint_temp,
                          float measured_temp,
                          float heater_duty_cycle,
                          float ramp_C_per_min);
  void createStage2JSONReport(Stage2Heater s2h,MachineStatusReport *msr, char *buffer);


  // This is currently not in use; we expect to need it
  // when we are making the system more automatic.
  void runComplexAlgolAssertions();
  void initErrors();
  void clearErrors();
  void clearThermocoupleErrors();
  void clearFanErrors();
  void clearMainsPowerErrors();
  // This is the number of periods around a point in time we will
  // average to produce a smooth temperature. (Our thermocouples have
  // only 0.25 C resolution, which is low for a 0.5C/minute control
  // situation!) These are always taken to be BACKWARD in time.
  // This IS NOT USED in the current code.
  const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
  // Ddelta is the change in temperature in C per min
  float Ddelta_C_per_min = 0.0;

  // Here are new parameters associated with the "One Button" algorithm
  bool USE_ONE_BUTTON = true;

  float CURRENT_TOTAL_WATTAGE_W;
  float CURRENT_HEATER_WATTAGE_W;
  float CURRENT_STACK_WATTAGE_W;

  // I believe this should be tested to see if a much slower
  // rate creates a more stable system. It makes no sense to me
  // to do this faster then than the 3-second turn-on time for the
  // heater. I suggest this be set to 20 seconds.
  static bool IsAShutdownState(MachineState ms);

  /********************************************
   Begin compile-time parameters
   ********************************************/

// our CFC Heater measures at 14.4 ohms, by W = V^2 / R assuming
// V = 115, W = 918.402
  // Change this based on the measurement of your CFC
  static constexpr float HEATER_MAXIMUM_WATTAGE = 918;
  // This is a bit of a fudge factor...
  static constexpr float HEATER_MAX_WATTAGE_FOR_DC_CALC = 0.95*HEATER_MAXIMUM_WATTAGE;
  // This should not change, unless you change your PSU
  const float MAX_STACK_VOLTAGE = 12.0;

  // This is the most important parameter!
  static constexpr float OPERATING_TEMPERATURE_C = 750.0;

  // You may have to adjust these based on altitude;
  // if the air is thin, the fan can over-spin, and
  // you may want to lower the max speed to 60%.
  const float FAN_SPEED_MAX_p = 80;
  const float FAN_SPEED_MIN_p = 30;
  static constexpr float FAN_SPEED_PREFERRED_p = 40;


  // The is an absolute max wattage allowed into the stack.
  const float BOUND_MAX_WATTAGE = 300.0;
  // This is the maximum amperage you can set to go into the stack.
  const float BOUND_MAX_AMPERAGE_SETTING = 60.0;
  // This is the maximum ramp in degrees C per minute.
  const float BOUND_MAX_RAMP_C_PER_MIN = 3.0;
  const float BOUND_MAX_TEMP_TRANSITION = 20.0;
  const unsigned long BOUND_MAX_TEMP_TRANSITION_TIME_MS = 10000;

  // The Number of Wafers (used in computing Pumping Voltage)
  const float NUM_WAFERS = 30;

// These are bounds; we won't let values go outside these.
  // They can only be changed here and forcing a recompilation.

  // This will cause an immediate shutdown if exceeded.
  static constexpr float OVER_TEMPERATURE_C = 800.0;

  // The target temp cannot be changed outside of these bounds.
  const float BOUND_MAX_TEMP = 750.0;
  const float BOUND_MIN_TEMP = 25.0;

  // these are the +/- over wattage and percent settings
  // for the purpose of verifying the operation of the PSU,
  // which is known to be inaccurate at low amperage
  const float MINIMUM_ACCURATE_WATTAGE_W = 20.0;
  const float MAXIMUM_STACK_OVER_WATTAGE_W = 20.0;
  const float MAXIMUM_STACK_OVER_WATTAGE_PC = 20.0;


  // AmOx has requested that we also have small forward bias,
  // and the easiest way to accomplish this is with a small amperage
  // in current control mode.
  static constexpr float MIN_OPERATING_STACK_AMPERAGE = 0.1;

  // Note: The MAX31850, OneWire system, and the MAX31855, both,
  // can not read reliably faster than 100ms.
  // We have tested the TEMP_READ_PERIOD_MS at 100,
  // but see no reason for it to be that fast.
  // At present the code does not really use or log readings
  // that are faster than the heater PID task period, so
  // there is no reason for it to be more than twice as fast as that.
  // Please refer to the documentation here:
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31850-MAX31851.pdf
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31855.pdf
  // Please make sure that the INIT_PID_PERIOD_MS is more than
  // the TEMP_READ_PERIOD_MS.
  static const int TEMP_READ_PERIOD_MS = 225; // this is intentionally a little less than half the PID PERIOD
  static const int INIT_PID_PERIOD_MS = 500;

  static const int WATTAGE_PID_SAMPLE_TIME_MS = 500;

  // The "heartbeat" on the OEDCS v.1.1 is both a red LED and PIN 13.
  static const int INIT_HEARTBEAT_PERIOD_MS = 500; // heartbeat task period

  // The is the basic recording of values in "emergency logging mode"
  static const int INIT_LOG_RECORDER_PERIOD_MS = 1000;

  // Task2 asked for 10 minutes of logging data at 1 Hz, so 600 records
  static constexpr unsigned int  MAX_RECORDS = 600;
  MachineStatusReport _log_entry[MAX_RECORDS];

    // if the operating temp is higher than the current setpoint temp and and the heater is off,
    // we have not choice but to decrease the stack watts...this is a bit of "magic"
    // that has no good rationale. - rlr
  const float DECREASE_STACK_WATTAGE_INCREMENT_W = 1.0;

  // If we are LOW_TEMP_TRIGGER below our setpoint, we will lower the fan speed
  const float LOW_TEMP_TRIGGER = 20;

  // These are the times that we will tolerate a given
  // detectable error condition before we treat the error as a critical error.
  // Normally, these are a number of minutes.
  const unsigned long THERMOCOUPLE_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long PWR_12V_FAULT_TOLERATION_TIME_MS = 1 * 60 * 1000;
  const unsigned long PWR_24V_FAULT_TOLERATION_TIME_MS = 1 * 60 * 1000;
  const unsigned long FAN_FAULT_TOLERATION_TIME_MS = 3 * 60 * 1000;
  const unsigned long HEATER_FAULT_TOLERATION_TIME_MS = 3 * 60 * 1000;
  const unsigned long ENVELOPE_FAULT_TOLERATION_TIME_MS = 3 * 60 * 1000;
  const unsigned long COULD_NOT_INIT_3_THERMOCOUPLES_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long FAN_LOSS_PWR_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long FAN_UNRESPONSIVE_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long HEATER_UNRESPONSIVE_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long STACK_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long PSU_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;
  const unsigned long MAINS_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;


  /********************************************
   FUTURE PARAMETERS, THESE HAVE NO MEANING AT PRESENT
   ********************************************/

  // // This is currently unused, as we do not support an Idle mode yet...
  // static constexpr float IDLE_STACK_VOLTAGE = 1.0;
  // //
  static const int INIT_SHUTDOWN_BUTTON_PERIOD_MS = 250;

  static const int DISPLAY_UPDATE_MS = 2000;




};


#endif
