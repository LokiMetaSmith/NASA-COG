/*
Public Invention's COG Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#ifndef DEBUG_H
#define DEBUG_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

namespace CogCore {

  // For example, call Debug<char*>("Some text") or Debug<bool>(myBoolVar)
  // to get a debug output on Arduino or native environments.
  // Beware that this uses RAM!
  template <class myType>
  void Debug (myType a) {
  #ifdef ARDUINO
    Serial.print(a);
    Serial.flush();
  #else
    std::cout << a;
  #endif
  }

  template <class myType>
  void DebugLn (myType a) {
  #ifdef ARDUINO
    Serial.print(a);
    Serial.print("\n");
    Serial.flush();
  #else
    std::cout << a << std::endl;
  #endif
  }

  // void DebugF(const char * msg) {
  //   #ifdef ARDUINO
  //   Serial.print(F(msg));
  //   #else
  //   std::cout << msg;
  //   #endif
  // }

  void serialBegin(unsigned long baud);
}

#endif
