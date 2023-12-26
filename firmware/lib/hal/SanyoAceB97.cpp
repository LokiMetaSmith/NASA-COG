// SanyoAceB97.cpp - Copyright (C) 2021
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



#include "SanyoAceB97.h"
#include <math.h>
#include <debug.h>

#define PERIOD 1000


//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
//namespace tach_data {

  unsigned long volatile tach_data_ts[NUMBER_OF_FANS];
  unsigned long volatile tach_data_cnt[NUMBER_OF_FANS];
  unsigned long volatile tach_data_ocnt[NUMBER_OF_FANS];
  unsigned long volatile tach_data_duration[NUMBER_OF_FANS];

// This can be used to add time to the interrupt for testing;
// so far I have not seen that that creates a problem
//unsigned long temporary_test_variable;

  void tachISR(uint8_t i) {
    tach_data_cnt[i]++;
    //    for(int i = 0; i < 1000; i++) {
    //      temporary_test_variable  = (temporary_test_variable * 2 ) % 37;
    //    }
  }
  void tachISR0() {
    tachISR(0);
  };

  void refresh_tach_data(uint8_t i) {
    unsigned long m = millis();
    if (tach_data_ts[i] + PERIOD < m) {
      tach_data_ocnt[i] = tach_data_cnt[i];
      tach_data_duration[i] = m - tach_data_ts[i];
      tach_data_ts[i] = m;
      tach_data_cnt[i] = 0;
    }
  }
//}

SanyoAceB97::SanyoAceB97()  {
    init();
  };

SanyoAceB97::SanyoAceB97(const char * name, uint8_t id){
    init();
    this->name = name;
    this->id = id;
};



// using namespace tach_data;

unsigned long SanyoAceB97::_calcRPM(uint8_t i){
  refresh_tach_data(i);
  if (DEBUG_FAN > 1) {
    CogCore::Debug<const char *>("CALC: " );
    CogCore::Debug<uint8_t>(i);
    CogCore::Debug<const char *>("\n" );
    CogCore::Debug<uint32_t>(tach_data_ocnt[i]);
    CogCore::Debug<uint32_t>(tach_data_duration[i]);
    CogCore::Debug<const char *>("\n" );
  }
  if (tach_data_duration[i] != 0) {
    // This calculation does not match the documentaiton.
    // https://docs.rs-online.com/1c09/0900766b816e68bb.pdf
    float num_revolutions = (float) tach_data_ocnt[0] / 2.0;
    float one_revolution_time_ms = (float) tach_data_duration[0];
    long rpm = (long) (60000.0 * ( num_revolutions / one_revolution_time_ms));
    return rpm;
  } else {
    return 0;
  }
}

float SanyoAceB97::getRPM(){
  return (float) _calcRPM(0);
}


bool SanyoAceB97::evaluateFan(float pwm_ratio,float rpms) {
  if(pwm_ratio >=0.2){
    if(abs((pwm_ratio*SanyoAceB97::APPROXIMATE_PWM_TO_RPMS) - rpms)
       > SanyoAceB97::ABSOLUTE_RPM_TOLERANCE) {
      return false;
    }
  }
  return true;
}

void SanyoAceB97::printRPMS() {
  CogCore::Debug<const char *>("RPMS:\n");
  for(uint8_t i = 0; i < NUMBER_OF_FANS; i++) {
    long rpm = _calcRPM(i);
    CogCore::Debug<const char *>("rpm: ");
    CogCore::Debug<uint32_t>(rpm);
    CogCore::Debug<const char *>("\n");
  }
}

// Note: I don't believe this should ever be used
// other than to completely turn off the motors.
// Using less than full power for these Delta fans
// prevents the PWM signal from working properly.
// Nevertheless I'm leaving it in place in case it
// is needed for some other purpose.
void SanyoAceB97::fanSpeedPerCentage(int s)
{

#ifdef FAN_LOCKOUT
	int q = map(s, SPEED_MIN, SPEED_MAX, OPERATING_PWM_THROTTLE, 0); // inverted PWM for Control v1 pcb's
#else
	int q = map(s, SPEED_MIN, SPEED_MAX, 0, OPERATING_PWM_THROTTLE);
#endif


  if (DEBUG_FAN > 0 ) {
    CogCore::Debug<const char *>("Putting out speed to fan control board:");
    CogCore::Debug<int>(s);
    CogCore::Debug<uint32_t>(q);
    CogCore::Debug<const char *>("\n");
  }

  analogWrite(PWM_PIN[0], q);


}

// This would be clearer in the the .h!! or in the machine hal for the specific device
bool SanyoAceB97::init() {

  PWM_PIN[0] = 9;
  TACH_PIN[0] = A0;
  fan_Enable = 22;

  #ifdef FAN_LOCKOUT
      pinMode(fan_Enable, OUTPUT);
	  digitalWrite(fan_Enable, HIGH);
  #endif


  for(int i = 0; i < NUMBER_OF_FANS; i++) {
    tach_data_ts[i] = 0;
    tach_data_cnt[i] = 0;
    tach_data_ocnt[i] = 0;
    tach_data_duration[i] = 0;
    pinMode(PWM_PIN[i], OUTPUT);
#ifdef FAN_LOCKOUT
	digitalWrite(PWM_PIN[i], HIGH);
#else
	digitalWrite(PWM_PIN[i], LOW);
#endif
    pinMode(TACH_PIN[i],INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[0]),tachISR0,FALLING);
  return true;
}

// void SanyoAceB97::E_STOP() {
// #ifdef FAN_LOCKOUT
//   digitalWrite(fan_Enable, LOW);


//   for(int i = 0; i < NUMBER_OF_FANS; i++) {
//     pinMode(PWM_PIN[i], OUTPUT);
// 		  digitalWrite(PWM_PIN[i], HIGH);
//   }
// #else
//     for(int i = 0; i < NUMBER_OF_FANS; i++) {
//     pinMode(PWM_PIN[i], OUTPUT);
// 		  digitalWrite(PWM_PIN[i], LOW);
//   }

// #endif
// }


// At present, we will use the same ratio for all fans;
// this is an oversimplification
void SanyoAceB97::updatePWM(float pwm_ratio) {

  //  evaluateFan( BLOWER_UNRESPONSIVE, pwm_ratio);

  fanSpeedPerCentage((int)( pwm_ratio * 100));
  _pwm_ratio[0] = pwm_ratio;

  if (DEBUG_FAN > 0 ) {
    CogCore::Debug<const char *>("PWM ratio:  num / ratio : ");
    CogCore::Debug<uint32_t>(pwm_ratio);
    CogCore::Debug<const char *>("\n");
    printRPMS();
  }
}
