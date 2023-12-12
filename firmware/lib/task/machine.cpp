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


void MachineConfig::dumpAllData10Hz() {
  // Loop over Ring buffer and call ouptutReport and
  // do a Network output (eventually);
  // using...

  //int msr_lre_size = _log_entry.size();
  //CogCore::Debug<int>(msr_lre_size);
  for(int i = 0; i < MAX_RECORDS; i++) {
    MachineStatusReport msr_lre= _log_entry[i];
    outputReport(&msr_lre );
    watchdogReset();
  }

}
void MachineConfig::outputReport(MachineStatusReport *msr) {
        CogCore::Debug<const char *>("\n");
	CogCore::Debug<const char *>("Timestamp: ");
        CogCore::Debug<long>(msr->timestamp);
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
}

void MachineConfig::createJSONReport(MachineStatusReport* msr, char *buffer) {
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
  for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++)
  {
	  if (msr->CriticalError[i].fault_present)//critical error detected
	  {
		sprintf(buffer+strlen(buffer), "\"CriticalError\": ");  
		sprintf(buffer+strlen(buffer),msr->CriticalErrorNames[i]);
		strcat(buffer, ",\n");
	  }
  }
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
 clearErrors();
 }

void MachineConfig::clearErrors() {
// here we init the errors...
 // If we lose a thermocouple for more than 10 seconds
 // we have to go into emergency shutdown.
 errors[POST_HEATER_TC_BAD].fault_present = false;
 errors[POST_GETTER_TC_BAD].fault_present = false;
 errors[POST_STACK_TC_BAD].fault_present  = false;

 errors[POST_HEATER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_GETTER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_STACK_TC_BAD].toleration_ms  = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;

 errors[POST_HEATER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_GETTER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_STACK_TC_BAD].response_state  = EmergencyShutdown;
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
