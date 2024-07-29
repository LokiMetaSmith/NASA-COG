/*
test_SanyoAceB97 -- A program to test the SanyoAceB97 blower

Copyright 2023, Robert L. Read

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

#include <unity.h>

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <debug.h>
#include <util.h>

using namespace CogCore;

/*
  We have already done live testing of our DC power supply
  at high resistances. But we want a way to "smoke test"
  our power supply and wiring at high amperage. We have
  made a custom resistor of low ohmage (maybe about 0.3 or 0.4 ohms)
  which can survive high temperature because it is made or
  nichrome wire. Our goal here is remarkably simple, though
  it doesn't really corresponde to a typical "unit" test.
  Our basic goal is to read from the serial port a simple
  number and apply that many volts. Then we can walk the
  voltage up by hand, reporting the amperage, and checking
  that we can acheive high amperage without anything going
  wrong in our wiring (like getting too hot.)
 */

#include "core_defines.h"
#include <machine.h>

const unsigned long REPORT_PERIOD_MS = 3000;

MachineConfig *machineConfig;
TaskProperties _properties;


void report(MachineConfig *machineConfig) {
  Serial.print("F : ");
  Serial.println(machineConfig->report->fan_rpm);
}
unsigned long time_of_last_report = 0;
void test_rollover_test(){

  Serial.println("Started test_rollover_test()!");

  for(int i = 0; i < 1000; i++) {
    delay(3000);
    unsigned long tm = t_millis();
    unsigned long m = millis();
    Serial.print(m);
    Serial.print(", ");
    Serial.print(tm);
    Serial.println();
  }

  TEST_ASSERT_TRUE(true);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_rollover_test);
  UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>

extern unsigned long ROLLOVER_FOR_TESTING;

void setup() {
  CogCore::serialBegin(115200UL);
  delay(1000); // delay to make sure it's ready

  //  machineConfig = new MachineConfig();
  //  machineConfig->hal = new MachineHAL();

  ROLLOVER_FOR_TESTING = 20000;
  unsigned long m = millis();
  unsigned long start_point = ROLLOVER_FOR_TESTING - 2000;
  set_t_millis(start_point,ROLLOVER_FOR_TESTING);
  Serial.print("start_point:");
  Serial.println(start_point);


  // bool initSuccess  = machineConfig->hal->init();
  bool initSuccess = true;
  if (initSuccess) {
    process();
  } else {
    Serial.println("HAL failed to init!");
    delay(1000);
  }
}

void loop() {
    //
    Serial.println("Erroneously got to loop");
    abort();
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif
