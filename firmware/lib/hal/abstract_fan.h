// Copyright (C) 2022 Lawrence Kincheloe

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

#ifndef ABSTRACT_FAN_H
#define ABSTRACT_FAN_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <math.h>
#endif
//#include <core_defines.h>
//#include <core.h>
#include <machine.h>


//class MachineConfig;  // Forward declaration for MachineConfig

class AbstractFAN {
private:
  const char * name;
  uint8_t id;

public:
  AbstractFAN(){};
  AbstractFAN(const char * name, uint8_t id){
    this->name = name;
    this->id = id;
  };
  virtual int init() { return -1; };
  virtual ~AbstractFAN(){};
  const char * getName();
  uint8_t getID();
  virtual void updatePWM(float normal_PWM, MachineConfig *config);
  virtual float getRPM( MachineConfig *config);
};


#endif
