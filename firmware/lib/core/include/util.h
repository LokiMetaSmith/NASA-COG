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

#ifndef UTIL_H
#define UTIL_H




namespace CogCore {




bool WithinArrayBounds(int index, int length);

}


#ifdef ARDUINO
#include <Arduino.h>




  // Cause millis to act as if you have set the
  // the system millisecond function (t_millis) to
  // a new value given by the argument. This function
  // can be used to test the rollover of code which
  // uses the the t_millis() wrapper.
// Note: m is the "now" time presumables set by calling t_millis();
// It is provided to allow "dependency injection" (informally), to
// allos the value to be set very close to the Rollover.


unsigned long set_t_millis(unsigned long new_millis_value_ms,
                           unsigned long m);

  // A testable version of millis, which can be used to
  // test the rollover problem.
unsigned long t_millis();

// x_millis() is just a marker to show what code has been considered.
// It should be replaced with t_millis whereever the code has been
// properly reviewed.
unsigned long x_millis();

#endif

#endif
