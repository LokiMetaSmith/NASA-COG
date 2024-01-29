/*
  cog_task.cpp -- main control algorithm or ceramic oxygen generator

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

#include "cog_task.h"
#include <cmath>
#include <abstract_temperature.h>
#include <TF800A12K.h>

// from: https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
// This should be made into a separte task,
// this is just for debugging...
// TODO: Move this into the core, and invoke it within a DEBUG_LEVEL guard.
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


using namespace std;


namespace CogApp
{
  // TODO: This class should probably be rewritten with specific state transition functions.
  // This would allow us to log transition events elegantly. As it is, we have to be idempotent...
  // in the "off" state we have to keep turning everything off because we don't know if we were
  // just turned off or have been off for 24 hours.

  // TODO: Most of this should be moved into the machine definition
  bool CogTask::_init()
  {
    CogCore::Debug<const char *>("CogTask init\n");
    getConfig()->fanDutyCycle = 0.0;
    const float MAXIMUM_TOTAL_WATTAGE = MachineConfig::HEATER_MAXIMUM_WATTAGE + getConfig()->MAX_STACK_WATTAGE;
    wattagePIDObject = new WattagePIDObject(MAXIMUM_TOTAL_WATTAGE);
	time_last_temp_changed_ms = millis(); //initialize time_last_temp_changed_ms when the task starts
    return true;
  }

  void CogTask::printGenericInstructions() {
    CogCore::Debug<const char *>("Enter s:0 to Turn Off, s:1 to Enter Troubleshooting Mode (NOT SUPPORTED FOR PRODUCTION),\n");
    CogCore::Debug<const char *>("Enter s:2 to Turn Enter Automatic Control.\n");
    CogCore::Debug<const char *>("Enter a:XX.X to set (a)mperage limit, (w)attage limit, (h)eater set p.\n");
  }

  COG_HAL* CogTask::getHAL() {
    return (COG_HAL *) (getConfig()->hal);
  }

  float CogTask::getTemperatureReadingA_C() {
    return getConfig()->report->post_heater_C;
  }
  float CogTask::getTemperatureReadingB_C() {
    return getConfig()->report->post_getter_C;
  }
  float CogTask::getTemperatureReadingC_C() {
    return getConfig()->report->post_stack_C;
  }

  // I am currently in the process of implementing the "One-Button Algorithm".
  // I will first implement the helper functions. -rlr
  float CogTask::computeTotalWattage(float controlTemp) {
    float totalWattage = wattagePIDObject->compute(controlTemp);
    return totalWattage;
  }
  float CogTask::computeTargetStackWattage(float targetTotalWattage, float heaterWatts, float currentTemp, float B, float C, float targetStackWatts) {
    float BC = (B + C) / 2.0;
    // if the operating temp is higher than the current setpoint temp and and the heater is off,
    // we have not choice but to decrease the stack watts...this is a bit of "magic"
    // that has no good rationale.
    if ((BC > currentTemp) && (heaterWatts <= 0.0)) {
      return max(targetStackWatts - getConfig()->DECREASE_STACK_WATTAGE_INCREMENT_W,0);
    }
    // here we implement a straight-line decrease in statck wattage proportional
    // to the difference C - B
    float y;
    float M = getConfig()->p.M_w;
    if (C > B) {
      float d = abs(C - B);
      float Q = getConfig()->p.Q_c;
      y = d * -M / Q + M;
    } else {
      y = M;
    }
    float L = getConfig()->MAX_STACK_WATTAGE;
    float w = max(0.0,min(L,y));
    return min(w,targetTotalWattage);
  }

  float CogTask::computeFanSpeedTarget(float currentTargetTemp, float temp, float heaterWatts,float A, float B, float C) {
    // const bool NEW_STRATEGY = true;
    // The NEW_STRATEGY is based on learnings that we have to adjust the fan dynamically.
    // The idea is to adjust the fan between three values: MIN, MAX, and PREFERRED.
    // We test for three boolean conditions, and compute fan speed based on
    // the 6 conditions (2 are impossible.) We rely on the changeRamps
    // and a slowing change fan speed to insure that we don't oscillate
    // in weird way or make drastic changes.
    //    if (NEW_STRATEGY) {
    enum ACTION { ABORT, INCREASE, DECREASE, NORMALIZE };
    ACTION a;
    bool LOW_TEMP = false;
    bool HI_DELTA = false;
    bool VERY_HI_DELTA = false;
    float delta = abs(B-C);
    LOW_TEMP = ((temp  + getConfig()->LOW_TEMP_TRIGGER) < currentTargetTemp);
    HI_DELTA = (delta > c.DELTA_LIMIT_K);
    VERY_HI_DELTA = (delta > c.DT_MAX_LIMIT_K);

    if (DEBUG_FAN > 0) {
      if (LOW_TEMP > 0) {
        CogCore::Debug<const char *>("XXXX LOW_TEMP: ");
        CogCore::DebugLn<int>(LOW_TEMP);
        CogCore::DebugLn<float>(temp);
        CogCore::DebugLn<float>(currentTargetTemp);
      }
      if (HI_DELTA > 0) {
        CogCore::Debug<const char *>("XXXX HI_DELTA: ");
        CogCore::DebugLn<int>(HI_DELTA);
      }
      if (VERY_HI_DELTA > 0) {
        CogCore::Debug<const char *>("XXXX VERY_HI_DELTA: ");
        CogCore::DebugLn<int>(VERY_HI_DELTA);
      }
    }
    switch (HI_DELTA) {
    case false:
      switch (LOW_TEMP) {
      case false:
        a = NORMALIZE;
        break;
      case true:
        a = DECREASE;
        break;
      }
      break;
    case true:
      switch (VERY_HI_DELTA) {
      case false:
        switch (LOW_TEMP) {
        case false:
          a = INCREASE;
          break;
        case true:
          a = INCREASE;
          break;
        }
        break;
      case true:
        switch (LOW_TEMP) {
        case false:
          a = INCREASE;
          break;
        case true:
          a = ABORT;
          break;
        }
        break;
      }
      break;
    }

    if (DEBUG_FAN > 0) {
      CogCore::Debug<const char *>("XXXX Mode: ");
      CogCore::DebugLn<int>(a);
    }
    if (a != ABORT) {
      if (getConfig()->errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].fault_present) {
        getConfig()->errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].fault_present = false;
      }
    }
    switch (a) {
    case INCREASE:
      if (DEBUG_FAN > 0) {
        CogCore::DebugLn<const char *>("XXXX Fan Action: Increase\n");
      }
      return getConfig()->FAN_SPEED_MAX_p;
      break;
    case DECREASE:
      if (DEBUG_FAN > 0) {
        CogCore::DebugLn<const char *>("XXXX Fan Action: Decrease\n");
      }
      return getConfig()->FAN_SPEED_MIN_p;
      break;
    case NORMALIZE:
      if (DEBUG_FAN > 0) {
        CogCore::DebugLn<const char *>("XXXX ACTION: Normalize\n");
      }
      return MachineConfig::FAN_SPEED_PREFERRED_p;
      break;
    case ABORT:
      if (DEBUG_FAN > 0) {
        CogCore::DebugLn<const char *>("XXXX ACTION: ABORT!\n");
        CogCore::DebugLn<const char *>("It is very unclear what action can be taken!\n");
      }
      // This is a major problem....we need to scream and croak.
      CogCore::DebugLn<const char *>("ACTION: ABORT DUE TO INABILITY TO PROGESS SAFELY\n");
      CogCore::DebugLn<const char *>("WARNING: AT PRESENT NO ACTION WILL BE TAKEN!!\n");
      if (!getConfig()->errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].fault_present) {
        getConfig()->errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].fault_present = true;
        getConfig()->errors[UNABLE_TO_RAISE_TEMPERATURE_SECURELY].begin_condition_ms = millis();
      }
      return getConfig()->FAN_SPEED_MAX_p;
      break;
    }
    CogCore::DebugLn<const char *>("WARNING:SHOULD NEVER GET HERE, FELL OUT OF LOOP!!\n");
    return -1;
  }
  bool CogTask::isStackWattageGood()
  {
	  return isStackWattageGood(getConfig()->report->stack_watts);
  }
  bool CogTask::isStackWattageGood(float testWattage)
  {
    float measured_wattage = getConfig()->report->stack_amps * getConfig()->report->stack_voltage;
      if (DEBUG_LEVEL_OBA > 0) {
        CogCore::Debug<const char *>("\nMeasure Wattage, TestWattage : ");
        CogCore::Debug<float>(measured_wattage);
        CogCore::Debug<const char *>(", ");
        CogCore::Debug<float>(testWattage);
        CogCore::Debug<const char *>("\n");
      }
      // We know our programmable PSU is inaccurate at low amperages...
      if (measured_wattage <= getConfig()->MINIMUM_ACCURATE_WATTAGE_W) {
        return true;
      }
  if ((measured_wattage > (((100.0 + getConfig()->MAXIMUM_STACK_OVER_WATTAGE_PC) / 100.0) * testWattage))
      ||
        (measured_wattage > (testWattage + getConfig()->MAXIMUM_STACK_OVER_WATTAGE_W))
        ) {
	  return false;
    }
	return true;
  }

  bool CogTask::evaluateHeaterEnvelope(double current_input_temperature,
                                       double goal_temperature,
                                       double value_PID)
  {

    return !(((value_PID >=1.0) || (value_PID<=0.0))
&&
             (abs(goal_temperature - current_input_temperature) > getConfig()->BOUND_MAX_TEMP_TRANSITION));
  }



  float CogTask::computeNernstVoltage(float T_K) {
    // P2 is the pressureized side.
    // P2O2 is the partial pressure of O2 on the pressurized size.
    const float ambient_psi = 15.0;
    // There is Pure O2 on the pressure side
    const float P2O2_psi = 1.0*ambient_psi;
    // P1 is the process air side
    const float P1O2_psi = 0.21*ambient_psi;
    const float F_Cdmol = 9.64853321233100184 * (10*1000.0);
    const float R_JdKmol = 8.31446261815324;
    const float V = R_JdKmol * T_K * log(P2O2_psi/P1O2_psi) / (4.0 * F_Cdmol);
    return V;
  }
  float CogTask::computePumpingWork(float T_K,float V,float R_O, float I_A) {
    // If the amperage is negative, which may happend at startup of the PSU,
    // then we are off the charts and punt here....
    if (I_A <= 0.0) return 0.0;
    const float Nernst_V = computeNernstVoltage(T_K);
    const float Pumping_V = Nernst_V * getConfig()->NUM_WAFERS;
    const float effectivePumping_V = max(0,Pumping_V);
    const float Pumping_Work_W = effectivePumping_V * I_A;
    return Pumping_Work_W;
  }

  void CogTask::oneButtonAlgorithm(float &totalWattage_w,float &stackWattage_w,float &heaterWattage_w,float &fanSpeed_p) {
    const float A = getTemperatureReadingA_C();
    const float B = getTemperatureReadingB_C();
    const float C = getTemperatureReadingC_C();

    const float T_c = (B+C) / 2.0;
    const float T_k = T_c + 273.15;

    unsigned long time = millis();
    if (USE_PAUSING) {
        const float DT_K = abs(B - C);
        if (DEBUG_LEVEL_OBA > 2) {
          CogCore::Debug<const char *>("abs(B-C): ");
          CogCore::Debug<float>(DT_K);
        }
        if (DT_K > c.DT_PAUSE_LIMIT_K) {
            if (c.pause_substate == 0) {
                c.pause_substate = 1;
                c.current_pause_began = time;
                CogCore::Debug<const char *>("PAUSING! X and temp:");
                CogCore::Debug<float>(A);
            } else {
                if (time > (c.current_pause_began + c.PAUSE_TIME_S * 1000)) {
                    c.pause_substate++;
                    c.current_pause_began = time;
                    CogCore::Debug<const char *>("PAUSING! Y");
                    CogCore::Debug<int>(c.pause_substate);
                }
            }
        } else {
            if ((c.pause_substate != 0) && (time > (c.current_pause_began + c.PAUSE_TIME_S * 1000)))
            {
              // Note: In the JavaScript simulation, we use this
              // to simulate a decrease in stack temperature over time, but that has no meaning on a real system,
              // so we set this to 0 instead of decrementing it.
              c.pause_substate = 0;
              c.current_pause_began = time;
            }
        }
    }


    totalWattage_w = computeTotalWattage(A);
    const float cur_heater_w = getConfig()->CURRENT_HEATER_WATTAGE_W;
    const float sw = computeTargetStackWattage(totalWattage_w,
                                               cur_heater_w,
                                               A,B,C,
                                               getConfig()->CURRENT_STACK_WATTAGE_W);


    // This is sometimes reported as negative at startup,
    // which is not physical.
    float R_O = getConfig()->report->stack_ohms;

    //const float actualWattage = getConfig()->report->stack_watts; //doesn't appear to be used LRK

    const float presetLimitedWattage = getConfig()->MAX_STACK_WATTAGE;
    getConfig()->report->max_stack_watts_W = getConfig()->MAX_STACK_WATTAGE;

    // Now we want to limit this with amps

    const float wattsLimitedByAmperage =
      (R_O > 0.0) ? // if the resistance is not positive, we can't compute this.
      getConfig()->MAX_AMPERAGE * getConfig()->MAX_AMPERAGE * R_O :
      0.0;
    float limitedWattage = min(presetLimitedWattage,wattsLimitedByAmperage);
    // proposed code:
    limitedWattage = min(limitedWattage,sw);

    // This is actually a constant in our program, so it makes little sense
    getConfig()->report->max_stack_amps_A = getConfig()->MAX_AMPERAGE;

    //    c.W_w = actualWattage;
    // Now we want to compute the part of the limitedWattage that adds heat
    // to the system...
    // We compute the stack amperage from the limitedWattage....
    const float I_A = getConfig()->report->stack_amps;
    const float V = getConfig()->report->stack_voltage;
    // The fact that the working wattage can be computed as higher than
    // limitedWattage is clearly a sign that the physical model here is wrong,
    // but it seems to happen only at low wattages...
    const float PW_W = min(limitedWattage,computePumpingWork(T_k,V,R_O,I_A));

    c.PW_w = PW_W;
//    console.assert(PW_W <= limitedWattage);
    const float input_heat = limitedWattage - PW_W;
    c.H_w = min(totalWattage_w - input_heat,MachineConfig::HEATER_MAXIMUM_WATTAGE);
    c.SIH_w = input_heat;
    c.H_w = max(0,c.H_w);

    // This is the most important action! This is used by change ramps
    // to set the actual wattage at a given moment.
    c.tW_w = limitedWattage;
    c.TW_w = totalWattage_w;

    // We could simulate the stack wattage as a function of T,
    // or we could just use the most recently measured stack wattage...
    // I believe doing the latter is more accurate.
    // Possibly we need to fudge this by adding 1 watt to it
    // so that we don't get stuck at the present value.
    const float FUDGE_STACK_WATTS = 1.0;

    // This will be used to set the actual stack watts in the
    // One Button algorithm. I'm not sure why this should not be just sw!
    // TODO: understand why I am taking this min here!
    if (DEBUG_LEVEL_OBA > 0) {
        CogCore::Debug<const char *>("Computed Stack Wattage: ");
        CogCore::Debug<float>(sw);
        CogCore::Debug<const char *>(" ");
        CogCore::Debug<float>(limitedWattage);
        CogCore::Debug<const char *>("\n");
    }

    // I'm not sure why I was performing the action below that is commented!
    //    stackWattage_w = sw;
    // This is needed because the stack especially when cold, cannot obtain
    // all of the wattage that we want...we limit it to what we have actually
    // achieved, plus a fudge factor. Without this action, we never put enough
    // into the heater.
    stackWattage_w = min(getConfig()->report->stack_watts + FUDGE_STACK_WATTS,
                                  limitedWattage);
    heaterWattage_w = max(0,
                          min(totalWattage_w - stackWattage_w,
                              getConfig()->HEATER_MAXIMUM_WATTAGE));

    fanSpeed_p = computeFanSpeedTarget(getConfig()->SETPOINT_TEMP_C, A, heaterWattage_w,A,B,C);

  }

  // Note: This is ms since the LAST TIME
  void CogTask::changeRamps(unsigned long delta_ms) {

    // delta_ms it the number of change that we want to do...
    // but all of our ramp rates are in terms of minutes
    // However, if for some reason delta_ms is very long, we don't want
    // to jump rapidly, so we will camp it at one minute!

    const float minutes = ((float)((delta_ms > 60 * 1000.0) ?
                                   60 * 1000.0 :
                                   delta_ms)
                           ) / (60.0 * 1000.0);
      if (DEBUG_LEVEL_OBA > 2) {
        CogCore::Debug<const char *>("Change Ramps Run! Minutes: ");
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<float>(minutes);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<int>(c.pause_substate);
        CogCore::Debug<const char *>("\n");
      }
    c.W_w += (((c.tW_w - c.W_w) > 0) ? 1.0 : -1.0) * c.Wr_Wdm * minutes;

    c.S_p += (((c.tS_p - c.S_p) > 0) ? 1.0 : -1.0) * c.Sr_Pdm * minutes;
    c.S_p = min(max(0.0,c.S_p),100.0);

    MachineState ms = getConfig()->ms;
    if (ms != NormalOperation) {
      if (c.pause_substate == 0) {
        // Is this using the correct variables?
        float diff = getConfig()->TARGET_TEMP_C - getConfig()->SETPOINT_TEMP_C;
        // Here I am trying to make sure we don't raise the SETPOINT_TEMP_C past our target
        // or lower it past our target.
        float change_c = c.Hr_Cdm * minutes;

        if (diff > 0.0) {
          getConfig()->SETPOINT_TEMP_C += change_c;
          getConfig()->SETPOINT_TEMP_C = min( getConfig()->SETPOINT_TEMP_C,
                                              getConfig()->TARGET_TEMP_C);
        } else {
          getConfig()->SETPOINT_TEMP_C -= change_c;
          getConfig()->SETPOINT_TEMP_C = max( getConfig()->SETPOINT_TEMP_C,
                                              getConfig()->TARGET_TEMP_C);
        }
      } else {
        CogCore::DebugLn<const char *>("PAUSED!");
      }
    } else {
      getConfig()->SETPOINT_TEMP_C = getConfig()->TARGET_TEMP_C;
    }
    c.W_w = max(c.W_w,0);
  }

  bool CogTask::evaluateErrorConditions() {
    bool retval = true;
    //Check for AC power, ie for +24V
    bool powerIsOK = is24VPowerGood();
    if (!powerIsOK){
      CogCore::Debug<const char *>("Probable AC Power (+24V) FAIL.\n");
      if (!getConfig()->errors[PWR_24V_BAD].fault_present) {
        getConfig()->errors[PWR_24V_BAD].fault_present = true;
        getConfig()->errors[PWR_24V_BAD].begin_condition_ms = millis();
        retval = false;
      }
    }
    else {
      if (getConfig()->errors[PWR_24V_BAD].fault_present) {
        getConfig()->errors[PWR_24V_BAD].fault_present = false;
        CogCore::Debug<const char *>("Probable AC Power (+24V) RESTORED!!!\n");
      }
    }//evaluate24v power

    if (!is12VPowerGood()){
      CogCore::Debug<const char *>("+12V out of tolerance.\n");
      if (!getConfig()->errors[PWR_12V_BAD].fault_present) {
        getConfig()->errors[PWR_12V_BAD].fault_present = true;
        getConfig()->errors[PWR_12V_BAD].begin_condition_ms = millis();
        retval = false;
      }
    }
    else {
      if (getConfig()->errors[PWR_12V_BAD].fault_present) {
        getConfig()->errors[PWR_12V_BAD].fault_present = false;
        CogCore::Debug<const char *>("Probable AC Power (+12V) RESTORED!!!\n");

      }
    }//evaluate12vPower

    if (!isStackWattageGood(getConfig()->CURRENT_STACK_WATTAGE_W)){
      CogCore::Debug<const char *>("Stack Wattage out of tolerance.\n");
      if (!getConfig()->errors[STACK_LOSS_CTL].fault_present) {
        getConfig()->errors[STACK_LOSS_CTL].fault_present = true;
        getConfig()->errors[STACK_LOSS_CTL].begin_condition_ms = millis();
        retval = false;
      }
    }
    else {
      if (getConfig()->errors[STACK_LOSS_CTL].fault_present) {
        getConfig()->errors[STACK_LOSS_CTL].fault_present = false;
        CogCore::Debug<const char *>("Stack Wattage now in tolerance!!!\n");
      }
    }//evaluate Stack Wattage
    // check fan speed...
    float fan_pwm_ratio = getConfig()->report->fan_pwm;
    float fan_rpm = getConfig()->report->fan_rpm;

    //    CogCore::Debug<const char *>("XXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    // CogCore::Debug<bool>(
    //                         getConfig()->errors[FAN_UNRESPONSIVE].fault_present);
    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Fan Inputs : ");
      CogCore::DebugLn<float>(fan_pwm_ratio);
      CogCore::DebugLn<float>(fan_rpm);
      //debug block
      if (DEBUG_FAN > 1 || DEBUG_LEVEL > 0) {
        CogCore::Debug<const char *>("Fan Fault FAN_UNRESPONSIVE Present: ");
        CogCore::Debug<bool>(getConfig()->errors[FAN_UNRESPONSIVE].fault_present);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("fan_pwm_ratio: ");
        CogCore::DebugLn<float>(fan_pwm_ratio);
        CogCore::Debug<const char *>("rpms: ");
        CogCore::DebugLn<float>(fan_rpm);
        CogCore::Debug<const char *>("rpm_actual: ");
        CogCore::DebugLn<float>((306.709 + (12306.7*fan_pwm_ratio) + (-6070*fan_pwm_ratio*fan_pwm_ratio)));
        CogCore::Debug<const char *>("rpm_difference: ");
        CogCore::DebugLn<float>((306.709 + (12306.7*fan_pwm_ratio) + (-6070*fan_pwm_ratio*fan_pwm_ratio))-fan_rpm);// 346.749 + 11888.545x + -5944.272x^2
        CogCore::Debug<const char *>("rpm_tested: ");
        CogCore::DebugLn<float>(abs((fan_pwm_ratio*7300.0) - fan_rpm));
      }
    }//end debug block
#ifndef DISABLE_FAN_EVAL//ADDED SO FAN CAN BE DISABLED !!! DO NOT LET THIS BE COMMENTED OUT IN production

    if (!getHAL()->_fans[0]->evaluateFan(fan_pwm_ratio,fan_rpm)) {
      CogCore::DebugLn<const char *>("Fan Fault Present");
      if (!getConfig()->errors[FAN_UNRESPONSIVE].fault_present) {
        getConfig()->errors[FAN_UNRESPONSIVE].fault_present = true;
        getConfig()->errors[FAN_UNRESPONSIVE].begin_condition_ms = millis();
        retval = false;
      }
    }
    else {
      if (getConfig()->errors[FAN_UNRESPONSIVE].fault_present) {
        getConfig()->errors[FAN_UNRESPONSIVE].fault_present = false;
      }
    }//evaluateFan
#endif

    // We only want to test this condition when we are in an on state,
    // because only then does the SETPOINT have meaning.
    if (!(MachineConfig::IsAShutdownState(getConfig()->ms) || (Off == getConfig()->ms))) {
      unsigned long time_now = millis();
      if (abs(time_now - time_last_temp_changed_ms) > getConfig()->BOUND_MAX_TEMP_TRANSITION_TIME_MS){
        time_last_temp_changed_ms = time_now;
        if (!evaluateHeaterEnvelope(getTemperatureReadingA_C(),
                                    getConfig()->SETPOINT_TEMP_C,
                                    getConfig()->report->heater_duty_cycle)){
          if (DEBUG_LEVEL > 1) CogCore::Debug<const char *>("TESTING ENVELOPE\n");
          if (DEBUG_LEVEL > 1) {
            CogCore::Debug<const char *>("TEMP BOUND EXCEEDED\n");
            CogCore::Debug<float>(abs(getConfig()->SETPOINT_TEMP_C - getTemperatureReadingA_C()));
            CogCore::Debug<const char *>("\n");
          }
          CogCore::DebugLn<const char *>("Heater Fault Present");
          if (!getConfig()->errors[HEATER_OUT_OF_BOUNDS].fault_present) {
            getConfig()->errors[HEATER_OUT_OF_BOUNDS].fault_present = true;
            getConfig()->errors[HEATER_OUT_OF_BOUNDS].begin_condition_ms = millis();
            retval = false;
          }
        } else {
          if (getConfig()->errors[HEATER_OUT_OF_BOUNDS].fault_present) {
            getConfig()->errors[HEATER_OUT_OF_BOUNDS].fault_present = false;
          }
        }
      }
    }//evaluateHeaterEnvelope

    if (!getHAL()->_stacks[0]->evaluatePS()){
      CogCore::DebugLn<const char *>("PSU Fault Present");
      if (!getConfig()->errors[PSU_UNRESPONSIVE].fault_present) {
        getConfig()->errors[PSU_UNRESPONSIVE].fault_present = true;
        getConfig()->errors[PSU_UNRESPONSIVE].begin_condition_ms = millis();
        retval = false;
      }
    } else {
      if (getConfig()->errors[PSU_UNRESPONSIVE].fault_present) {
        getConfig()->errors[PSU_UNRESPONSIVE].fault_present = false;
        getHAL()->_stacks[0]->reInit();
      }
    }//evaluatePSU

    return retval;
  }

  bool CogTask::_run() {

    // Report fan speed
    float calculated_fan_speed_rpms = getHAL()->_fans[0]->getRPM();

    getConfig()->report->fan_rpm = calculated_fan_speed_rpms;

    evaluateErrorConditions();

    if (DEBUG_LEVEL > 0) {
      CogCore::DebugLn<const char *>("BEFORE RUN GENERIC!");
    }

    this->StateMachineManager::run_generic();

    if (DEBUG_LEVEL > 0) {
      CogCore::DebugLn<const char *>("AFTER RUN GENERIC!");
    }

    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Free Memory: ");
      CogCore::Debug<int>(freeMemory());
      CogCore::Debug<const char *>("\n");
    }
  }

  // We believe someday an automatic algorithm will be needed here.
  float CogTask::getFanSpeed(float t) {
    return getConfig()->FAN_SPEED;
  }

  // Here is where we attempt to bring in both the amperage
  // and the wattage limitation (but amperage is the "plant"
  // variable that we can control.
  float CogTask::computeAmperage(float t) {
    float max_a_from_raw = getConfig()->MAX_AMPERAGE;
    float max_a_from_wattage =
      sqrt(
           getConfig()->MAX_STACK_WATTAGE /
           getConfig()->report->stack_ohms);

    if (DEBUG_LEVEL > 2) {
      CogCore::Debug<const char *>("max_a_from_raw , max_a_from_wattage");
      CogCore::Debug<uint32_t>(max_a_from_raw);
      CogCore::Debug<const char *>(" ");
      CogCore::Debug<uint32_t>(max_a_from_wattage);
      CogCore::Debug<const char *>("\n");
    }
    return min(max_a_from_raw,max_a_from_wattage);
  }

  void CogTask::turnOn() {
    if (DEBUG_LEVEL > 1) {
      CogCore::Debug<const char *>("TURNING ON  -- TURNING ON -- TURNING ON\n");
    }
    last_time_ramp_changed_ms = 0;
  }

  void CogTask::turnOff() {
    if (DEBUG_LEVEL > 1) {
      CogCore::Debug<const char *>("TURNING OFF  -- TURNING OFF -- TURNING OFF\n");
    }
    float fs = 0.0;
    getConfig()->fanDutyCycle = fs;
    getConfig()->FAN_SPEED = 0.0;
    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    dutyCycleTask->dutyCycle = 0;
    getConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;
    //    _updateStackVoltage(getConfig()->MIN_OPERATING_STACK_VOLTAGE);
    _updateStackAmperage(MachineConfig::MIN_OPERATING_STACK_AMPERAGE);
    // Although debatable, we want to clear all the erorrs when
    // turning off to avoid overreporting.
    getConfig()->clearErrors();
    // Although after a minute this should turn off, we want
    // to do it immediately
    StateMachineManager::turnOff();
  }

  MachineState CogTask::_updatePowerComponentsOff() {
    if (DEBUG_LEVEL > 2) {
      CogCore::Debug<const char *>("TURN OFF BEGINNING\n");
    }
    turnOff();
    if (DEBUG_LEVEL > 2) {
      CogCore::Debug<const char *>("TURN OFF END \n");
    }
    return Off;
  }

  bool CogTask::is12VPowerGood()
  {
    if (DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("PowerMonitorTask run\n");

    //Analog read of the +12V expected about 3.25V at ADC input.
    // SENSE_24V on A1.
    // Full scale is 1023, ten bits for 3.3V.
    //40K into 10000
    const long FullScale = 1023;
    const float percentOK = 0.25;
    const float R1=40000;
    const float R2=10000;
    const float Vcc = 3.3;
#ifdef DISABLE_12V_EVAL
    const int highThreshold12V = 1024;//930 ; //(12*(R2/(R1+R2))/Vcc)*FullScale *(1 + percentOK);
	const int lowThreshold12V = 434; //(12*(R2/(R1+R2))/)*FullScale *(1 - percentOK);
#else
    const int highThreshold12V = 930;//930 ; //(12*(R2/(R1+R2))/Vcc)*FullScale *(1 + percentOK);
	const int lowThreshold12V = 558; //(12*(R2/(R1+R2))/)*FullScale *(1 - percentOK);
#endif
/*
#ifdef
    const int highThreshold12V = 1024;
    const int lowThreshold12V = 558; //(12*(R2/(R1+R2))/)*FullScale *(1 - percentOK);
#endif */





    int _v12read = analogRead(SENSE_12V);

    if (DEBUG_LEVEL >0 ) {
      CogCore::Debug<const char *>("analogRead(SENSE_12V)= ");
      CogCore::DebugLn<uint32_t>(_v12read);
      CogCore::Debug<float>((float) _v12read * ((Vcc * (R1+R2))/(1023.0 * R2)));
      CogCore::Debug<const char *>("\n");
    }

    if (( _v12read > lowThreshold12V) && ( _v12read < highThreshold12V) ) {
      if (DEBUG_LEVEL >0 )  CogCore::Debug<const char *>("+12V power monitor reports good.\n");
      return true;
    } else{
      if (DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("+12V power monitor reports bad.\n");
      CogCore::Debug<const char *>("lowThreshold12V: ");
      CogCore::Debug<int32_t>(lowThreshold12V);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("highThreshold12V: ");
      CogCore::Debug<int32_t>(highThreshold12V);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("_v12read: ");
      CogCore::Debug<int32_t>(_v12read);
      CogCore::Debug<const char *>("\n");
      return false;
    }
  }

  bool CogTask::is24VPowerGood()
  {
    if (DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("PowerMonitorTask run\n");

    //Analog read of the +24V expected about 3.25V at ADC input.
    // SENSE_24V on A1.
    // Full scale is 1023, ten bits for 3.3V.
    //30K into 4K7
    const long FullScale = 1023;
    const float percentOK = 0.25;
    const float R1=40000;
    const float R2=4700;
    const float Vcc = 3.3;
    const int lowThreshold24V = 587; //(24*(R2/(R1+R2))/)*FullScale *(1 - percentOK); 782.28
    // Note: Rob proposes that this should simply by 1024, if only because this makes it easier to test.
    // I can't see any value in having a highThreshold---are we genuninely attempting to test that
    // our 24V value is too high? -- rlr
    // Switch this on control V1.1 C-pre processor flag
#ifdef CTL_V_1_1
    const int highThreshold24V = 978; //(24*(R2/(R1+R2))/Vcc)*FullScale *(1 + percentOK);
#else
    const int highThreshold24V = 1024;
#endif

    int _v24read = analogRead(SENSE_24V);

    if (DEBUG_LEVEL >0 ) {
      CogCore::Debug<const char *>("analogRead(SENSE_24V)= ");
      CogCore::DebugLn<uint32_t>(_v24read);
      CogCore::Debug<float>((float) _v24read * ((Vcc * (R1+R2))/(1023.0 * R2)));
      CogCore::Debug<const char *>("\n");
    }

    if (( _v24read > lowThreshold24V) && ( _v24read < highThreshold24V) ) {
      if (DEBUG_LEVEL >0 )  CogCore::Debug<const char *>("+24V power monitor reports good.\n");
      return true;
    } else{
      if (DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("+24V power monitor reports bad.\n");
      CogCore::Debug<const char *>("lowThreshold24V: ");
      CogCore::Debug<int32_t>(lowThreshold24V);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("highThreshold24V: ");
      CogCore::Debug<int32_t>(highThreshold24V);
      CogCore::Debug<const char *>("\n");
      CogCore::Debug<const char *>("_v24read: ");
      CogCore::Debug<int32_t>(_v24read);
      CogCore::Debug<const char *>("\n");
      return false;
    }
  }

  float CogTask::computeHeaterDutyCycleFromWattage(float heaterWattage_w) {
    return (heaterWattage_w < 0.0) ?
      0.0 :
      min(1.0,heaterWattage_w/getConfig()->HEATER_MAX_WATTAGE_FOR_DC_CALC);
  }

  void CogTask::runOneButtonAlgorithm() {
      if (DEBUG_LEVEL_OBA > 2) {
        CogCore::Debug<const char *>("Run One Button XXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
      }

      unsigned long now_ms = millis();
      unsigned long delta_ms = now_ms - last_time_ramp_changed_ms;
      changeRamps(delta_ms);
      getConfig()->report->target_fan_pc = c.tS_p;
      // This should never be above 100, but it is!
      _updateFanSpeed(c.S_p);

      last_time_ramp_changed_ms = now_ms;

      float totalWattage_w;
      float stackWattage_w;
      float heaterWattage_w;
      float tFanSpeed_p;

      oneButtonAlgorithm(totalWattage_w,stackWattage_w,heaterWattage_w,tFanSpeed_p);
      float dc = computeHeaterDutyCycleFromWattage(heaterWattage_w);
      if (DEBUG_LEVEL_OBA > 0) {
        CogCore::Debug<const char *>("One Button Summary\n");
        CogCore::Debug<const char *>("Total Wattage : ");
        CogCore::Debug<float>(totalWattage_w);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Desired Stack Wattage : ");
        CogCore::Debug<float>(stackWattage_w);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Heater Wattage: ");
        CogCore::Debug<float>(heaterWattage_w);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Fan Speed   % : ");
        CogCore::Debug<float>(tFanSpeed_p);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("DC          % : ");
        CogCore::Debug<float>(dc * 100.0);
        CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("SetPoint: ");
        CogCore::Debug<float>(getConfig()->SETPOINT_TEMP_C);
        CogCore::Debug<const char *>("\n");
      }

      // This is setting the target...
      wattagePIDObject->temperatureSetPoint_C = getConfig()->SETPOINT_TEMP_C;
      CogCore::Debug<const char *>("SetPoint: ");
      CogCore::Debug<float>(getConfig()->SETPOINT_TEMP_C);
      CogCore::Debug<const char *>("\n");

      getConfig()->report->total_wattage_W = totalWattage_w;

      getConfig()->CURRENT_TOTAL_WATTAGE_W = totalWattage_w;

      getConfig()->CURRENT_STACK_WATTAGE_W = stackWattage_w;
      _updateStackWattage(stackWattage_w);
      // if(!(isStackWattageGood(stackWattage_w))){
      //   	   CogCore::Debug<const char *>("Stack Wattage out of tolerance.\n");
      //   if (!getConfig()->errors[STACK_LOSS_CTL].fault_present) {
      //     getConfig()->errors[STACK_LOSS_CTL].fault_present = true;
      //     getConfig()->errors[STACK_LOSS_CTL].begin_condition_ms = millis();
      //   }
      // }
      //     else {
      //   if (getConfig()->errors[STACK_LOSS_CTL].fault_present) {
      //     getConfig()->errors[STACK_LOSS_CTL].fault_present = false;
      //   	  		  CogCore::Debug<const char *>("Stack Wattage now in tolerance!!!\n");
      //   }
      // }//evaluate Stack Wattage
      c.tS_p = tFanSpeed_p;

      getConfig()->CURRENT_HEATER_WATTAGE_W = heaterWattage_w;

      dutyCycleTask->dutyCycle = dc;
      getConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;
  }

  void CogTask::_updateCOGSpecificComponents() {

      float t = getTemperatureReadingA_C();
      float a = computeAmperage(t);
      // TODO: t is not used here, and it should be removed to clarify it...
      float fs = getFanSpeed(t);

      if (DEBUG_LEVEL > 2) {
        CogCore::Debug<const char *>("fan speed, amperage\n");
        CogCore::Debug<float>(fs);
        CogCore::Debug<const char *>(", ");
        CogCore::Debug<float>(a);
        CogCore::Debug<const char *>("\n");
      }
      getHAL()->_updateFanPWM(fs);
      getConfig()->report->fan_pwm = fs;

      _updateStackAmperage(a);
      _updateStackVoltage(getConfig()->MAX_STACK_VOLTAGE);
      //     if(!(isStackWattageGood(a * getConfig()->MAX_STACK_VOLTAGE))){
      //   	   CogCore::Debug<const char *>("Stack Wattage out of tolerance.\n");
      //   if (!getConfig()->errors[STACK_LOSS_CTL].fault_present) {
      //     getConfig()->errors[STACK_LOSS_CTL].fault_present = true;
      //     getConfig()->errors[STACK_LOSS_CTL].begin_condition_ms = millis();
      //   }
      // }else {
      //   if (getConfig()->errors[STACK_LOSS_CTL].fault_present) {
      //     getConfig()->errors[STACK_LOSS_CTL].fault_present = false;
      //   	  		  CogCore::Debug<const char *>("Stack Wattage now in tolerance!!!\n");
      //   }
      // }//evaluate Stack Wattage

  }
  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;

    float t = getTemperatureReadingA_C();
    getConfig()->GLOBAL_RECENT_TEMP = t;

    // if we've reached operating temperature, we switch
    // states
    if (t >= getConfig()->TARGET_TEMP_C) {
      new_ms = NormalOperation;
      return new_ms;
    }

    //    new_ms = StateMachineManager::_updatePowerComponentsWarmup();
    if (getConfig()->USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {
      if (new_ms == Warmup) {
        _updateCOGSpecificComponents();
      }
    }
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;

    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Cooldown Mode!\n");
    }
    float t = getTemperatureReadingA_C();
    getConfig()->GLOBAL_RECENT_TEMP = t;

    if (t <= getConfig()->TARGET_TEMP_C) {
      new_ms = NormalOperation;
      return new_ms;
    }
    if (getConfig()->USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {
      if (new_ms == Cooldown) {
        _updateCOGSpecificComponents();
      }
    }
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsIdle() {
    CogCore::Debug<const char *>("IN IDLE FUNCTION\n ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;
    //    _updateStackVoltage(MachineConfig::IDLE_STACK_VOLTAGE);
    _updateStackAmperage(MachineConfig::MIN_OPERATING_STACK_AMPERAGE);
    return new_ms;
  }

  // After being sure that we've logged this
  // I think we can shift to the off conditions.
  MachineState CogTask::_updatePowerComponentsCriticalFault() {
    MachineState new_ms = OffUserAck;
    //    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    //    _updateStackAmperage(MachineConfig::MIN_OPERATING_STACK_AMPERAGE);
    turnOff();
    logRecorderTask->dumpRecords();
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
  CogCore:Debug<const char *>("GOT EMERGENCY SHUTDOWN!");
    MachineState new_ms = OffUserAck;
    turnOff();
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    //    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    //    _updateStackAmperage(MachineConfig::MIN_OPERATING_STACK_AMPERAGE);

    // We need to be sure we are off in all of the Off states...
    turnOff();
    return new_ms;
  }

  // TODO: These would go better in the HAL
  void CogTask::_updateFanSpeed(float percentage) {
    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("calling update Fan Speed!\n");
      CogCore::DebugLn<float>(percentage);
    }
    float unitInterval = percentage / 100.0;
    getConfig()->FAN_SPEED = unitInterval;
    getHAL()->_updateFanPWM(unitInterval);
    getConfig()->report->fan_pwm = unitInterval;
  }
  void CogTask::_updateStackWattage(float wattage) {
    // We will set the amperage based on the resistance that we measure
    // via the formula A = sqrt(W/R)
    float r = getConfig()->report->stack_ohms;
    float w = wattage;
    float a = (r <= 0.0) ? 0.0 : sqrt(w/r);
    a = max(a,getConfig()->MIN_OPERATING_STACK_AMPERAGE);
    _updateStackVoltage(getConfig()->MAX_STACK_VOLTAGE);
    _updateStackAmperage(a);
    getConfig()->CURRENT_STACK_WATTAGE_W = wattage;
  }
  void CogTask::_updateStackVoltage(float voltage) {
    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Updating Stack Voltage (BBB): ");
      CogCore::DebugLn<float>(voltage);
      CogCore::DebugLn<unsigned long>(millis());
    }

    for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
      getHAL()->_stacks[i]->updateVoltage(voltage,getConfig());
    }
    if (DEBUG_LEVEL > 0) {
      CogCore::DebugLn<const char *>("Done (BBB): ");
      CogCore::DebugLn<unsigned long>(millis());
    }

  }

  void CogTask::_updateStackAmperage(float amperage) {
    if (amperage < 0.0) {
      // This is an internal error which should not occur..
       CogCore::Debug<const char *>("Internal Error, negative amperage\n");
      return;
    }
    if (DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Updating Stack Amperage (CCC):\n");
      CogCore::DebugLn<float>(amperage);
    }

    for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
      getHAL()->_stacks[i]->updateAmperage(amperage,getConfig());
    }
  }

  MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
    MachineState new_ms = NormalOperation;
    //    StateMachineManager::_updatePowerComponentsOperation(i_or_o);
    runOneButtonAlgorithm();
    return new_ms;
  }
}
