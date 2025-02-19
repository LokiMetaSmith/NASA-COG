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

#include <machine.h>
// there is not yet anything for this to do
#include <core.h>
// #include <Wire.h>
#include <assert.h>


void MachineConfig::outputReport(MachineStatusReport *msr) {
        CogCore::Debug<const char *>("\n");
	CogCore::Debug<const char *>("Timestamp: ");
        CogCore::Debug<unsigned long>(msr->timestamp);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Machine State: ");
        CogCore::Debug<const char *>(MachineConfig::MachineStateNames[msr->ms]);
        CogCore::Debug<const char *>("\n");
        // These are the dynamic targets
        CogCore::Debug<const char *>("Target     C: ");
        CogCore::Debug<float>(msr->target_temp_C);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Setpoint   C: ");
        CogCore::Debug<float>(msr->setpoint_temp_C);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Target Ramp C: ");
        CogCore::Debug<float>(msr->target_ramp_C);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Max Stack   A: ");
        CogCore::Debug<float>(msr->max_stack_amps_A);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Max Stack   W: ");
        CogCore::DebugLn<float>(msr->max_stack_watts_W);
        CogCore::Debug<const char *>("Fan PWM   0-1: ");
        CogCore::DebugLn<float>(msr->fan_pwm);
        CogCore::Debug<const char *>("Fan Target % : ");
        CogCore::DebugLn<float>(msr->target_fan_pc);
        CogCore::Debug<const char *>("Post Heater C: ");
        CogCore::Debug<float>(msr->post_heater_C);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Post Getter C: ");
        CogCore::Debug<float>(msr->post_getter_C);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Post Stack  C: ");
        CogCore::Debug<float>(msr->post_stack_C);
        CogCore::Debug<const char *>("\n");

        CogCore::Debug<const char *>("Heater DC 0-1: ");
        // We want more precision to see this changing faster.
	char t[10];
	sprintf(t, "%.5f", msr->heater_duty_cycle);
        CogCore::Debug<const char *>(t);
        CogCore::Debug<const char *>("\n");
        //        CogCore::DebugLn<float>(msr->heater_duty_cycle);
        CogCore::Debug<const char *>("Stack amps  A: ");
        CogCore::Debug<float>(msr->stack_amps);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Stack watts W: ");
        CogCore::Debug<float>(msr->stack_watts);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Stack volts V: ");
        CogCore::Debug<float>(msr->stack_voltage);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Stack ohms  O: ");
        if (isnan(msr->stack_ohms) || msr->stack_ohms < 0.0) {
          CogCore::Debug<const char*>(" N/A");
        CogCore::Debug<const char *>("\n");
        } else {
          CogCore::Debug<float>(msr->stack_ohms);
        CogCore::Debug<const char *>("\n");
        }
        CogCore::Debug<const char *>("Fan RPM      : ");
        CogCore::DebugLn<float>(msr->fan_rpm);
        //add a state for error: no error
        for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++)
          {
	  if (msr->errors[i])//critical error detected
            {
              CogCore::Debug<const char *>("CriticalError: ");
              CogCore::Debug<const char *>(CriticalErrorNames[i]);
              CogCore::Debug<const char *>("\n");
            }
          }
}

void MachineConfig::createJSONReport(MachineStatusReport* msr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"RawMillis\": %ld",msr->timestamp);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"MachineState\": %d",msr->ms);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"TargetC\": %.2f",msr->target_temp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"SetpointC\": %.2f",msr->setpoint_temp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"RampC\": %.2f",msr->target_ramp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"MaxStackA\": %.2f",msr->max_stack_amps_A);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"MaxStackW\": %.2f",msr->max_stack_watts_W);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"FanPWM\": %.2f",msr->fan_pwm);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"TargFanPC\": %.2f",msr->target_fan_pc);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"HeaterC\": %.2f",msr->post_heater_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackC\": %.2f",msr->post_stack_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"GetterC\": %.2f",msr->post_getter_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"HeaterDutyCycle\": %.2f",msr->heater_duty_cycle);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackA\": %.2f",msr->stack_amps);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackW\": %.2f",msr->stack_watts);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackV\": %.2f",msr->stack_voltage);
  strcat(buffer, ",\n");
  if (isnan(msr->stack_ohms) || isinf(msr->stack_ohms) || msr->stack_ohms < 0.0) {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": -1.0");
    strcat(buffer, ",\n");
  } else {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": %.2f",msr->stack_ohms);
    strcat(buffer, ",\n");
  }
  //add a state for error: no error

  sprintf(buffer+strlen(buffer), "\"CriticalError\": [");
  bool is_fault_present = false;
  for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++)
  {
	if (msr->errors[i])//critical error detected
	{
	  if(is_fault_present)strcat(buffer, ",");
	  is_fault_present = true;
	  strcat(buffer, "\"");
	  sprintf(buffer+strlen(buffer),CriticalErrorNames[i]);
	  strcat(buffer, "\"");

	  strcat(buffer, "\n");
	}
  }
  if (!is_fault_present)
  {
    strcat(buffer, "\"No Error\"\n");
  }
  strcat(buffer, "],\n");

  sprintf(buffer+strlen(buffer), "\"FanRPM\": %.2f",msr->fan_rpm);
  strcat(buffer, "\n");

}

void MachineConfig::change_ramp(float ramp) {
  RAMP_UP_TARGET_D_MIN = ramp;
  RAMP_DN_TARGET_D_MIN = -ramp;
  report->target_ramp_C = ramp;
}

bool MachineHAL::init() {
  init_heaters();
}


MachineConfig::MachineConfig() {
 script = new MachineScript();
 report = new MachineStatusReport();
 initErrors();
}

void MachineConfig::initErrors() {
// here we init the errors...
 // If we lose a thermocouple for more than 10 seconds
 // we have to go into emergency shutdown.
 clearThermocoupleErrors();
 clearFanErrors();
 clearMainsPowerErrors();


 errors[POST_HEATER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_GETTER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_STACK_TC_BAD].toleration_ms  = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 // No toleration for initialization erorrs.
 errors[COULD_NOT_INIT_3_THERMOCOUPLES].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[FAN_LOSS_PWR].toleration_ms = FAN_LOSS_PWR_FAULT_TOLERATION_TIME_MS;
 errors[FAN_UNRESPONSIVE].toleration_ms = FAN_FAULT_TOLERATION_TIME_MS;
 errors[HEATER_UNRESPONSIVE].toleration_ms = HEATER_FAULT_TOLERATION_TIME_MS;

 errors[PWR_12V_BAD].toleration_ms = PWR_12V_FAULT_TOLERATION_TIME_MS;
 errors[PWR_24V_BAD].toleration_ms = PWR_24V_FAULT_TOLERATION_TIME_MS;
 errors[STACK_LOSS_CTL].toleration_ms = STACK_FAULT_TOLERATION_TIME_MS;
 errors[PSU_UNRESPONSIVE].toleration_ms = PSU_FAULT_TOLERATION_TIME_MS;
 errors[MAINS_LOSS_PWR].toleration_ms = MAINS_FAULT_TOLERATION_TIME_MS;
 errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].toleration_ms = UNABLE_TO_RAISE_TEMPERATURE_SECURELY_TOLERATION_TIME_MS;
 errors[SYSTEM_OVER_TEMPERATURE].toleration_ms =  SYSTEM_OVER_TEMPERATURE_TOLERATION_TIME_MS;
 errors[HEATER_OUT_OF_BOUNDS].toleration_ms =  ENVELOPE_FAULT_TOLERATION_TIME_MS;

 errors[POST_HEATER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_GETTER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_STACK_TC_BAD].response_state  = EmergencyShutdown;
 errors[COULD_NOT_INIT_3_THERMOCOUPLES].response_state  = EmergencyShutdown;
 errors[FAN_LOSS_PWR].response_state = EmergencyShutdown;
 errors[FAN_UNRESPONSIVE].response_state = EmergencyShutdown;
 errors[HEATER_UNRESPONSIVE].response_state = EmergencyShutdown;
 errors[STACK_LOSS_CTL].response_state = EmergencyShutdown;
 errors[PSU_UNRESPONSIVE].response_state = EmergencyShutdown;
 errors[MAINS_LOSS_PWR].response_state = EmergencyShutdown;
 errors[HEATER_OUT_OF_BOUNDS].response_state = EmergencyShutdown;
 errors[SYSTEM_OVER_TEMPERATURE].response_state = EmergencyShutdown;
 errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].response_state = EmergencyShutdown;
}

void MachineConfig::clearThermocoupleErrors() {
 // here we init the errors...
 // If we lose a thermocouple for more than 10 seconds
 // we have to go into emergency shutdown.
 errors[POST_HEATER_TC_BAD].fault_present = false;
 errors[POST_GETTER_TC_BAD].fault_present = false;
 errors[POST_STACK_TC_BAD].fault_present  = false;
  // we should never have to clear an init error, but this is
  // in expectation of future proofing...
  errors[COULD_NOT_INIT_3_THERMOCOUPLES].fault_present  = false;
}

void MachineConfig::clearFanErrors()  {
 errors[FAN_LOSS_PWR].fault_present = false;
 errors[FAN_UNRESPONSIVE].fault_present = false;
}

void MachineConfig::clearMainsPowerErrors()  {
 errors[HEATER_UNRESPONSIVE].fault_present = false;
 errors[STACK_LOSS_CTL].fault_present = false;
 errors[PSU_UNRESPONSIVE].fault_present = false;
 errors[MAINS_LOSS_PWR].fault_present = false;
 errors[HEATER_OUT_OF_BOUNDS].fault_present = false;
 errors[PWR_12V_BAD].fault_present = false;
 errors[PWR_24V_BAD].fault_present = false;
 errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].fault_present = false;
}

void MachineConfig::clearErrors() {
  clearThermocoupleErrors();
  clearFanErrors();
  clearMainsPowerErrors();
}

// This code is currently not invoked.
// On Sept. 15th, 2023, we chose to use the more manual "5 knobs"
// approach. I'm retaining this because I believe we will eventually
// need this for "one-button" operation.
void MachineConfig::runComplexAlgolAssertions() {
  // How we make certain assertions to make sure we are well configured
  CogCore::Debug<const char *>("BEGINNING ASSERTION CHECKS!!\n");
  CogCore::Debug<const char *>("IF YOU DO NOT SEE THE WORDS 'ALL CLEAR' BELOW AN ASSERTION HAS FAILED\n");
  delay(100);
  // assert(RAMP_UP_TARGET_D_MIN >= 0.0);
  // assert(RAMP_DN_TARGET_D_MIN <= 0.0);

  // assert(YELLOW_TEMP < RED_TEMP);
  // assert(OPERATING_TEMP < YELLOW_TEMP);
  // assert(STOP_TEMP < OPERATING_TEMP);

  // assert(FAN_SPEED_AT_OPERATING_TEMP < FULL_POWER_FOR_FAN);
  // assert(TEMP_TO_BEGIN_FAN_SLOW_DOWN < OPERATING_TEMP);
  // assert(OPERATING_TEMP < END_FAN_SLOW_DOWN);
  CogCore::Debug<const char *>("ALL CLEAR!!\n");
  delay(50);
}

bool MachineConfig::init() {
 CogCore::Debug<const char *>("error toleration times (ms)\n");
 for(int i = 0; i < 3; i++) {
   CogCore::Debug<const char *>("Errors index: ");
   CogCore::Debug<int>(i);
   CogCore::Debug<const char *>(" ");
   CogCore::Debug<uint32_t>(errors[i].toleration_ms);
   CogCore::Debug<const char *>("\n");
 }
  return true;
}
bool MachineConfig::IsAShutdownState(MachineState ms) {
  return ((ms == CriticalFault) || (ms == EmergencyShutdown) || (ms == OffUserAck));
}
