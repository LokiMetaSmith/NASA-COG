// Copyright (C) 2021
// Robert Read, Ben Coombs, Lawrence Kincheloe.

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

#ifndef STACK_TEMPERATURE_SENSOR
#define STACK_TEMPERATURE_SENSOR

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <model.h>
#include "abstract_temperature.h"

namespace Temperature {
  class StackTemperatureSensor : public AbstractTemperature {
  private:
    SensorConfig _config;
    float _temperature;
  public:
    OxApp::Model _m;
    StackTemperatureSensor();

    // A mock sensor requires a "model" of the machine for mocking.
    StackTemperatureSensor(OxApp::Model& m,SensorConfig &config);
    StackTemperatureSensor(SensorConfig &config);
    ~StackTemperatureSensor() {};
    void Config(SensorConfig &config);
    float ReadTemperature();
    float GetTemperature();
    float GetTemperature(int idx);

    SensorConfig GetConfig() const;
  };

}

#endif
