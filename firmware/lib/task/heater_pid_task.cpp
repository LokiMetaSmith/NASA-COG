/*

  heater_pid_ttask.cpp -- A simple PID controller for the SSR AC heater

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

#include <heater_pid_task.h>

// Note: This routine may now be obsolete
HeaterPIDTask::HeaterPIDTask() {
  // dutyCycle is measured betwen 0.0 and 1.0 (and ends
  // up being a PWM duty cycle)
  this->pidControllerHeater =
    new PID(&(this->Input_temperature_C), &(this->dutyCycle_Output),
            &(this->HeaterSetPoint_C), FKp, FKi, FKd, DIRECT);
  this->pidControllerHeater->SetOutputLimits(-1.0, 1.0);
  this->pidControllerHeater->SetSampleTime(MachineConfig::INIT_PID_PERIOD_MS);
  this->pidControllerHeater->SetMode(AUTOMATIC);
}

bool HeaterPIDTask::_init()
{
  CogCore::Debug<const char *>("HeaterPIDTask init\n");
  last_temp_change = millis(); //initialize last_temp_change when the task starts
  return true;
}

void HeaterPIDTask::printTunings() {
  CogCore::Debug<const char *>("Tunings for            : ");
  CogCore::Debug<const char *>(MachineConfig::HeaterNames[whichHeater]);
  CogCore::Debug<const char *>(" ");
  char t[10];
  sprintf(t, "%.5f", FKp);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>(", ");
  sprintf(t, "%.5f", FKi);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>(", ");
  sprintf(t, "%.5f", FKd);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>("\n");
  CogCore::Debug<const char *>("Tunings (Inverted, 1/x): ");
  CogCore::Debug<const char *>(MachineConfig::HeaterNames[whichHeater]);
  CogCore::Debug<const char *>(" ");
    if (FKp == 0.0) CogCore::Debug<const char *>("nan");
    else {
      sprintf(t, "%.2f", 1.0/FKp);
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>(", ");
    if (FKi == 0.0) CogCore::Debug<const char *>("nan");
    else {
      sprintf(t, "%.2f", 1.0/FKi);
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>(", ");
    if (FKd == 0.0) CogCore::Debug<const char *>("nan");
    else {
      sprintf(t, "%.2f", 1.0/FKd);
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>("\n");
}

void HeaterPIDTask::SetTunings(double p, double i, double d) {
  FKp = p;
  FKi = i;
  FKd = d;
  this->pidControllerHeater->SetTunings(p,i,d);
  if (DEBUG_PID > 0) {
    printTunings();
  }
}

double HeaterPIDTask::GetKp() {
  return FKp;
}
double HeaterPIDTask::GetKi() {
  return FKi;
}
double HeaterPIDTask::GetKd() {
  return FKd;
}

void HeaterPIDTask::shutHeaterDown() {
  this->HeaterSetPoint_C = 25.0;
  getConfig()->report->heater_duty_cycle = 0.0;
  dutyCycleTask->dutyCycle = 0.0;
}
//  evaluateHeater(previousInput,this->Input_temperature_C,this->HeaterSetPoint_C,s)

double HeaterPIDTask::evaluateHeater(	int idx, \
							CritcalErrorCondition ec, \
							double &previous_input_temperature, 
							double &current_input_temperature,
							double &goal_temperature)
{

	time_now = millis();
	
	if(previous_input_temperature != current_input_temperature)
	{
		last_temp_change = time_now;
	}
	
	if ((time_now - last_temp_change) > getConfig()->BOUND_MAX_TEMP_TRANSITION)
	{
	  // As long as there is not a fault present, this creates;
      // if one is already present, we leave it.
      if (!getConfig()->errors[ec].fault_present) {
        getConfig()->errors[ec].fault_present = true;
        getConfig()->errors[ec].begin_condition_ms = millis();
      }
	}
}

bool HeaterPIDTask::_run()
{
  // if we are running the One Button Algorithm
  if (getConfig()->USE_ONE_BUTTON) {
    // I'm not sure what the return value should be
    return true;
  }

  if (DEBUG_PID > 1) {
    CogCore::Debug<const char *>("HeaterPIDTask run\n");
    double test_spud = getConfig()->report->post_heater_C;
    CogCore::Debug<const char *>("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    CogCore::Debug<float>(this->HeaterSetPoint_C);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<float>(this->Input_temperature_C);
   CogCore::Debug<const char *>("\n");
  }


  
  MachineState ms = getConfig()->ms;
  if ((ms == Off) || (ms == EmergencyShutdown) || (ms == OffUserAck)) {
    // in this case, we do nothing...but we will put the set point
    // to room temperature.
    this->HeaterSetPoint_C = 25.0;
    getConfig()->report->heater_duty_cycle = 0.0;
    dutyCycleTask->dutyCycle = 0.0;
    return true;
  }

  double previousInput = this->Input_temperature_C;

  this->Input_temperature_C = getConfig()->report->post_heater_C;
  
  //evaluate the temerature and verify it's changing within the predescribed time interval
  evaluateHeater(0,HEATER_UNRESPONSIVE,previousInput,this->Input_temperature_C,this->HeaterSetPoint_C);
  
  // didn't hang when return was here.
  pidControllerHeater->Compute();

  // double s = this->dutyCycle_Output + this->final_dutyCycle;
  double s = this->dutyCycle_Output;
  // didn't hang when return was here

  s = min(s, 1.0);
  s = max(s, 0.0);
  this->final_dutyCycle = s;

  dutyCycleTask->dutyCycle = s;

  // This resets our duty_cycle computation
  //  dutyCycleTask->reset_duty_cycle();

  getConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;

  if (DEBUG_PID > 1) {
    CogCore::Debug<const char *>("Setpoint");
    char t[10];
    sprintf(t, "%.2f", this->HeaterSetPoint_C);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("previous input ");
    sprintf(t, "%.5f", previousInput);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("Final dutyCycle_Output ");
    sprintf(t, "%.5f", this->dutyCycle_Output);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("Final dutyCycle ");
    sprintf(t, "%.5f", this->final_dutyCycle);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
  }

  return true;
}
