// Copyright (C) 2023 Lawrence Kincheloe

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

#include "abstract_fan.h"

#define DEBUG_LEVEL 0

const char * AbstractFAN::getName(){
  return name;
}
uint8_t AbstractFAN::getID(){
  return id;
}
// void Stack::update(float voltage) {
// #ifdef RIBBONFISH
//   if (DEBUG_LEVEL > 0) {
//     CogCore::Debug<const char *>(name);
//     CogCore::Debug<const char *>(" Voltage: ");
//     CogCore::Debug<float>(voltage);
//     CogCore::Debug<const char *>("\n");
//   }
// #else
//   CogCore::Debug<const char *>("STACK update: (NOT IMPLEMENTED)");
//   CogCore::Debug<const char *>("   ");
//   CogCore::Debug<const char *>(name);
//   CogCore::Debug<const char *>(" pwm_ratio: ");
//   CogCore::Debug<float>(_pwm_ratio);
//   CogCore::Debug<const char *>("\n");
// #endif
// }
