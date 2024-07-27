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

#include "util.h"

namespace CogCore {

// TODO: make template
bool WithinArrayBounds(int index, int length) {
  return ((index >= 0) && (index < length));
}

}

#ifdef ARDUINO

// If you want to test the millisecond rollover mechanism,
// set this is a non-zero value with an extern declaration
unsigned long ROLLOVER_FOR_TESTING = 120*1000;


unsigned long fake_addition_to_millis = 0;
// make this 0 to test.

// Note: m is the "now" time presumables set by calling t_millis();
// It is provided to allow "dependency injection" (informally), to
// allos the value to be set very close to the Rollover.
unsigned long set_t_millis(unsigned long new_millis_value_ms, unsigned long m) {

  if (ROLLOVER_FOR_TESTING) {
    if (new_millis_value_ms > m) {
      fake_addition_to_millis = (new_millis_value_ms-m) % ROLLOVER_FOR_TESTING;
    } else if (new_millis_value_ms < m) {
      fake_addition_to_millis = (ROLLOVER_FOR_TESTING - (m - new_millis_value_ms)) % ROLLOVER_FOR_TESTING;
    } else {
      fake_addition_to_millis = 0;
    }
  } else {
    fake_addition_to_millis = (m - new_millis_value_ms);
  }
  return t_millis();
}
// A testable version of millis, which can be used to
// test the rollover problem.
unsigned long x_millis() {
  return t_millis();
}
unsigned long t_millis() {
  unsigned long m = millis();
  if (fake_addition_to_millis) {
    if (ROLLOVER_FOR_TESTING) {
     return (fake_addition_to_millis + m) % ROLLOVER_FOR_TESTING;
    } else {
      return fake_addition_to_millis + m;
    }
  } else {
    if (ROLLOVER_FOR_TESTING) {
     return m % ROLLOVER_FOR_TESTING;
    } else {
     return m;
    }
  }
}
#endif
