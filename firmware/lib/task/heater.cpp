// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include "heater.h"

#define DEBUG_LEVEL 0

namespace CogApp {

    void Heater::update(float voltage) {
        _voltage = voltage;


#ifdef RIBBONFISH
        if (DEBUG_LEVEL > 0) {
          CogCore::Debug<const char *>("Heater update: ");
          CogCore::Debug<int>(id);
          CogCore::Debug<const char *>("   ");
          CogCore::Debug<const char *>(name);
          CogCore::Debug<const char *>(" Voltage: ");
        }

        if (_voltage > 6.0) {
          digitalWrite(this->pin,HIGH);
          if (DEBUG_LEVEL > 0) {
            CogCore::DebugLn<float>(12);
          }
        } else {
          digitalWrite(this->pin,LOW);
          if (DEBUG_LEVEL > 0) {
            CogCore::DebugLn<float>(0);
          }
        }


#else
        if (DEBUG_LEVEL > 0) {
          CogCore::Debug<const char *>("Heater update: ");
          CogCore::Debug<const char *>("   ");
          CogCore::Debug<const char *>(name);
          CogCore::Debug<const char *>(" Voltage: ");
          CogCore::DebugLn<float>(_voltage);
        }
#endif

    }


  float Heater::compute_change_in_voltage(float current_C,float current_V,float desired_C, float watts_per_degree) {

    // We compute the number of degrees across the heater that we seek,
    // then compute watts from that, then change the heater
    // to aim at that many watts.
    float delta_C = desired_C - current_C;
    float delta_watts = watts_per_degree * delta_C;
    float current_watts = current_V * current_V / _resistance;
    CogCore::Debug<const char *>("current watts ");
    CogCore::DebugLn<float>(current_watts);
    float new_watts = current_watts + delta_watts;
    float new_volts = sqrt( new_watts * _resistance);
    CogCore::Debug<const char *>("delta_C ");
    CogCore::DebugLn<float>(delta_C);

    return new_volts;
  }

}
