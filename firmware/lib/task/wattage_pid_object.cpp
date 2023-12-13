/*

  wattage_pid_object.cpp -- A simple PID controller for the Toatal Wattage

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

#include <wattage_pid_object.h>

// Note: This routine may now be obsolete
WattagePIDObject::WattagePIDObject(float MAXIMUM_TOTAL_WATTAGE) {
    this->pidControllerWattage =
      new PID(&(this->input_temperature_C), &(this->totalWattage_Output_W),
              &(this->temperatureSetPoint_C), FKp, FKi, FKd, DIRECT);
    this->pidControllerWattage->SetOutputLimits(0.0, MAXIMUM_TOTAL_WATTAGE);
    // This should probably be changed...
    this->pidControllerWattage->SetSampleTime(MachineConfig::WATTAGE_PID_SAMPLE_TIME_MS);
    this->pidControllerWattage->SetMode(AUTOMATIC);
}

void WattagePIDObject::printTunings() {
    CogCore::Debug<const char *>("WattagePIDObject run\n");
    CogCore::Debug<const char *>("Tunings for Tottal Wattage PID Task: ");
    CogCore::Debug<const char *>(" ");
    char t[10];
    sprintf(t, "%0.5f", FKp);
    CogCore::Debug<const char *>(t);
    sprintf(t, "%0.5f", FKi);
    CogCore::Debug<const char *>(", ");
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>(", ");
    sprintf(t, "%0.5f", FKd);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("Tunings (Inverted, 1/x): ");
    CogCore::Debug<const char *>(" ");
    if (FKp == 0.0) {
      CogCore::Debug<const char *>("nan");
    } else {
      sprintf(t, "%0.2f", (1.0/FKp));
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>(", ");
    if (FKi == 0.0) {
      CogCore::Debug<const char *>("nan");
    } else {
      sprintf(t, "%0.5f", (1.0/FKi));
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>(", ");
    if (FKd == 0.0) {
      CogCore::Debug<const char *>("nan");
    } else {
      sprintf(t, "%0.5f", (1.0/FKd));
      CogCore::Debug<const char *>(t);
    }
    CogCore::Debug<const char *>("\n");
}

void WattagePIDObject::SetTunings(double p, double i, double d) {
  FKp = p;
  FKi = i;
  FKd = d;
  this->pidControllerWattage->SetTunings(p,i,d);
  if (DEBUG_PID > 0) {
    printTunings();
  }
}

double WattagePIDObject::GetKp() {
  return FKp;
}
double WattagePIDObject::GetKi() {
  return FKi;
}
double WattagePIDObject::GetKd() {
  return FKd;
}

float WattagePIDObject::compute(float controlTemp)
{
  if (DEBUG_PID > 1) {
    CogCore::Debug<const char *>("WattagePIDObject run\n");
    CogCore::Debug<const char *>("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n");
    CogCore::Debug<float>(this->temperatureSetPoint_C);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<float>(this->input_temperature_C);
    CogCore::Debug<const char *>("\n");
  }

  double previousInput = this->input_temperature_C;

  this->input_temperature_C = controlTemp;

  pidControllerWattage->Compute();

  double s = this->totalWattage_Output_W;

  this->final_totalWattage_W = s;


  if (DEBUG_PID > 1) {
    CogCore::Debug<const char *>("Setpoint");
    char t[10];
    sprintf(t, "%0.2f", this->temperatureSetPoint_C);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("previous input ");
    sprintf(t, "%0.2f", previousInput);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("Final Total Wattage ");
    sprintf(t, "%0.5f", this->final_totalWattage_W);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>("\n");
  }

  return  this->final_totalWattage_W;
}
