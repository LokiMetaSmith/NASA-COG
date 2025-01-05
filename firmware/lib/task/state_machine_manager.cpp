/*
  state_machine_manager.cpp

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

#include <state_machine_manager.h>
#include <util.h>

using namespace std;

namespace CogApp
{

  bool StateMachineManager::run_generic()
  {
    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("starting run generic: ");
    }

    MachineState ms = getConfig()->ms;

    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("ms : ");
      CogCore::Debug<int>(ms);
      CogCore::Debug<const char *>("\n");
    }
    printOffWarnings(ms);

    printGenericInstructions();

    MachineState new_state = _executeBasedOnState(ms);
    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("finished execute\n");
    }
    // if the state really changes, we want to log that and take some action!
    if (new_state != ms) {
      getConfig()->ms = new_state;
      CogCore::Debug<const char *>("CHANGING STATE TO: ");
      CogCore::Debug<const char *>(getConfig()->MachineStateNames[getConfig()->ms]);
      CogCore::Debug<const char *>("\n");
    }
    return true;
  }

  void StateMachineManager::printOffWarnings(MachineState ms) {
    // If we are in the off state there is nothing to do!
    if (ms == OffUserAck) {
      CogCore::Debug<const char *>("AN ERROR OCCURRED. WILL NOW ENTER OFF STATE\n");
      CogCore::Debug<const char *>("UNTIL ACKNOWLEDGED. ENTER A SINGLE 'K' TO ACKNOWLEDGE:\n");
    }
  }

  MachineState StateMachineManager::checkCriticalFaults(MachineState ms) {
    MachineState rms = ms;
    unsigned long now = t_millis();

    for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++) {
      if (getConfig()->errors[i].fault_present) {
        if (!MachineConfig::IsAShutdownState(getConfig()->ms)) {
          CogCore::Debug<const char *>("WILL AUTOMATICALLY SHUTDOWN IF NOT RESTORED IN ");
          unsigned long now = t_millis();
          if (now < (float) getConfig()->errors[i].begin_condition_ms) { // ROLLOVER EVENT
            getConfig()->errors[i].begin_condition_ms = 0;
          }
          CogCore::Debug<float>((((float) getConfig()->errors[i].toleration_ms) -
                                 ((float) now - (float) getConfig()->errors[i].begin_condition_ms)) / (float) 1000);
          CogCore::Debug<const char *>(" SECONDS DUE TO : ");
          CogCore::DebugLn<const char *>(CriticalErrorNames[i]);
        }
        if (now < (float) getConfig()->errors[i].begin_condition_ms) { // ROLLOVER EVENT
          getConfig()->errors[i].begin_condition_ms = 0;
        }
        if ((((float) now) - ((float) getConfig()->errors[i].begin_condition_ms))
            > (float) getConfig()->errors[i].toleration_ms) {
          CogCore::Debug<const char *>("ENTERING CRITICAL FAULT : ");
          CogCore::DebugLn<const char *>(CriticalErrorNames[i]);
          turnOff();
          //logRecorderTask->dumpRecords();  // FLE 20240213
          rms = CriticalFault;
          logRecorderTask->dumpRecords();    // FLE 20240213
        }
      }
    }
    return rms;
  }

  bool StateMachineManager::isInBlackout() {
    return !(is12VPowerGood() && is24VPowerGood());
  }
  // There is significant COG dependent logic here.
  // At the expense of extra lines of code, I'm
  // going to keep this mostly simple by making it look
  // "table-driven"
  MachineState StateMachineManager::_executeBasedOnState(MachineState ms) {
    MachineState new_ms = ms;

    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("\nMachine State: ");
      CogCore::Debug<const char *>(getConfig()->MachineStateNames[ms]);
      CogCore::Debug<const char *>(" : ");
      CogCore::Debug<const char *>(getConfig()->MachineSubStateNames[getConfig()->idleOrOperate]);
      CogCore::Debug<const char *>("\n");
    }


    // WARNING: REVISIT -- This might not be the best place to do this:
    bool inBlackout = isInBlackout();
    if (inBlackout &&
        ((new_ms == Warmup) ||
         (new_ms == Cooldown) ||
         (new_ms == NormalOperation))) {
      new_ms = AwaitingPower;
    }

     if (inBlackout) {
      // If we are in the blackout condition, we clear certain errors,
      // because we don't want to shutdown because these conditions are
      // recoverable.
      // You could argue it would be stylistically superior not to
      // produce the errors in the first place, but doing that would
      // be fragile and likely lead to bugs. - rlr
      getConfig()->clearErrorsInducedByBlackouts();
    }

    new_ms = checkCriticalFaults(new_ms);

    switch(new_ms) {
    case Off:
      new_ms = _updatePowerComponentsOff();
      break;
    case AwaitingPower:
      new_ms = _updateAwaitingPower();
      break;
    case Warmup:
      new_ms = _updatePowerComponentsWarmup();
      break;
    case NormalOperation:
      new_ms = _updatePowerComponentsOperation(getConfig()->idleOrOperate);
      break;
    case Cooldown:
      new_ms = _updatePowerComponentsCooldown();
      break;
    case CriticalFault:
      new_ms = _updatePowerComponentsCriticalFault();
      break;
    case EmergencyShutdown:
      new_ms = _updatePowerComponentsEmergencyShutdown();
      break;
    case OffUserAck:
      new_ms = _updatePowerComponentsOffUserAck();
      break;
    default:
      CogCore::Debug<const char *>("INTERNAL ERROR: UNKOWN MACHINE STATE\n");
      // This is not really enough information; we need a way to
      // record what the fault is, but it will do for now.
      new_ms = CriticalFault;
    }
    getConfig()->previous_ms = ms;
    getConfig()->ms = new_ms;
    getConfig()->report->ms = new_ms;

    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("end_of_run_generic \n");
    }
    return new_ms;
  }


  float StateMachineManager::computeRampUpSetpointTemp(float t,float recent_t,unsigned long begin_up_time_ms) {
    bool error = false;
    unsigned long ms = t_millis_assert_no_rollover(begin_up_time_ms,
                                                   error);
    if (error) {
      // This is a fairly weird situation --- a rollover during ramp up.
      // By setting begin_up_time_ms to zero, we are effectively just starting over---which seems like the
      // safest thing to do in therms of temperature shock. Since this is a Ramp temperature,
      // The rest of our algorithm should prevent any radical thermal changes.
      begin_up_time_ms = 0;
    }
    const unsigned long MINUTES_RAMPING_UP = (ms - begin_up_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_UP * getConfig()->RAMP_UP_TARGET_D_MIN;
    tt = min(tt,getConfig()->TARGET_TEMP_C);
    tt = min(tt,getConfig()->BOUND_MAX_TEMP);
    return tt;
  }
  float StateMachineManager::computeRampDnSetpointTemp(float t,float recent_t,unsigned long begin_dn_time_ms) {
    bool error = false;
    unsigned long ms = t_millis_assert_no_rollover(begin_dn_time_ms,
                                                   error);
    if (error) {
      // This is a fairly weird situation --- a rollover during ramp up.
      // By setting begin_up_time_ms to zero, we are effectively just starting over---which seems like the
      // safest thing to do in therms of temperature shock. Since this is a Ramp temperature,
      // The rest of our algorithm should prevent any radical thermal changes.
      begin_dn_time_ms = 0;
    }
    const unsigned long MINUTES_RAMPING_DN = (ms - begin_dn_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_DN * getConfig()->RAMP_DN_TARGET_D_MIN;
    tt = max(tt,getConfig()->TARGET_TEMP_C);
    tt = max(tt,getConfig()->BOUND_MIN_TEMP);
    return tt;
  }

  void StateMachineManager::turnOff() {
    //    heaterPIDTask->shutHeaterDown();
  }

  void StateMachineManager::turnOffPowerButDoNotChangeState() {
  }

  void StateMachineManager::turnOn() {
  }

  // if we change the targetTemp, we will enter either
  // Warmup or Cooldown, with new values.
  void StateMachineManager::transitionToWarmup(float recent) {
    getConfig()->previous_ms = getConfig()->ms;
    getConfig()->ms = Warmup;
    getConfig()->WARM_UP_BEGIN_TEMP = recent;
    getConfig()->SETPOINT_TEMP_C = recent;
    getConfig()->BEGIN_UP_TIME_MS = t_millis();
  }

  void StateMachineManager::transitionToCooldown(float recent) {
    getConfig()->previous_ms = getConfig()->ms;
    getConfig()->ms = Cooldown;
    getConfig()->COOL_DOWN_BEGIN_TEMP = recent;
    getConfig()->SETPOINT_TEMP_C = recent;
    getConfig()->BEGIN_DN_TIME_MS = t_millis();
  }

  void StateMachineManager::changeTargetTemp(float t) {
    MachineConfig *mc = getConfig();

    float tt = min(mc->BOUND_MAX_TEMP,t);
    tt = max(mc->BOUND_MIN_TEMP,tt);

    mc->TARGET_TEMP_C = tt;
    mc->report->target_temp_C = tt;
    float current = mc->GLOBAL_RECENT_TEMP;
    if (tt > current) {
      transitionToWarmup(current);
    } else if (tt < current) {
      transitionToCooldown(current);
    } else {
      // no change needed
    }
  }


  MachineState StateMachineManager::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Warmup Mode!\n");
    }

    float t = getTemperatureReadingA_C();
    getConfig()->GLOBAL_RECENT_TEMP = t;

    // if we've reached operating temperature, we switch
    // states
    if (t >= getConfig()->TARGET_TEMP_C) {
      new_ms = NormalOperation;
      return new_ms;
    }

    CogCore::Debug<const char *>("Run One Button YYYY : ");
    CogCore::DebugLn<bool>(getConfig()->USE_ONE_BUTTON);

    // TODO: this should really be in the cog_task, not here.
    if (getConfig()->USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {
      // These also are dependent on which heater we are using
      float tt = computeRampUpSetpointTemp(t,
                                           getConfig()->WARM_UP_BEGIN_TEMP,
                                           getConfig()->BEGIN_UP_TIME_MS);
      if (SM_DEBUG_LEVEL > 0) {
        CogCore::Debug<const char *>("Warmup tt for :");
	CogCore::Debug<uint32_t>(getConfig()->s2heater);
        CogCore::Debug<float>(tt);
        CogCore::Debug<const char *>("Global Recent temp\n");
        CogCore::Debug<float>(getConfig()->GLOBAL_RECENT_TEMP);
	CogCore::Debug<const char *>("\n");
        CogCore::Debug<float>(getConfig()->BEGIN_UP_TIME_MS);
	CogCore::Debug<const char *>("\n");
      }

      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;
    }
    return new_ms;
  }

  MachineState StateMachineManager::_updatePowerComponentsCriticalFault() {
	if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Critical Fault!\n");
    }
    return CriticalFault;
  }
  MachineState StateMachineManager::_updatePowerComponentsEmergencyShutdown() {
	if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Emergency Shutdown!\n");
    }
    return EmergencyShutdown;
  }

  MachineState StateMachineManager::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    if (SM_DEBUG_LEVEL > 0) {
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

      float tt = computeRampDnSetpointTemp(t,
                                           getConfig()->COOL_DOWN_BEGIN_TEMP,
                                           getConfig()->BEGIN_DN_TIME_MS);
      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;

      if (SM_DEBUG_LEVEL > 0) {
        CogCore::Debug<const char *>("CoolDown tt for :");
	CogCore::Debug<uint32_t>((unsigned long) heaterPIDTask);
	CogCore::Debug<const char *>("\n");
        CogCore::Debug<float>(tt);
	CogCore::Debug<const char *>("\n");
        CogCore::Debug<const char *>("Global Recent temp\n");
        CogCore::Debug<float>(getConfig()->GLOBAL_RECENT_TEMP);
	CogCore::Debug<const char *>("\n");
        CogCore::Debug<float>(getConfig()->BEGIN_UP_TIME_MS);
	CogCore::Debug<const char *>("\n");
      }
    }
    return new_ms;
  }


  MachineState StateMachineManager::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
      if (SM_DEBUG_LEVEL > 0) {
        CogCore::Debug<const char *>("SetPeriod Done!\n");
      }

    MachineState new_ms = NormalOperation;
    if (getConfig()->USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
      if (SM_DEBUG_LEVEL > 0) {
        CogCore::Debug<const char *>("Run One Button Algorithm Done!");
      }

    } else {

      float tt = getConfig()->TARGET_TEMP_C;
      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;
    }
    return new_ms;
  }

  float StateMachineManager::read12V_busVoltage() {
    int _v12read = analogRead(SENSE_12V);

    // Note: Presente in the V1.1 Control board, the resistors
    // for this are R104 (ground side) and R103, R107, and R108
    // (+12V) side. These are all marked at 10K, but there is a note to:
    // REWORK: Change R108 from 10K to 20K
    // If this rework is done, that would give us 40K high
    // side and 10K low side.
    // This should give us a voltage of:
    // Vout = Vs * 10000 / (40000)
    // Vout = Vs / 4.

    float v12BusVoltage = (float) _v12read * ((Vcc * (R1+R2))/(1023.0 * R2));
    if (SM_DEBUG_LEVEL > -1) {
      CogCore::Debug<const char *>("analogRead(SENSE_12V)= ");
      CogCore::DebugLn<uint32_t>(_v12read);
      CogCore::Debug<float>((float) v12BusVoltage);
      CogCore::Debug<const char *>("\n");
    }
    return v12BusVoltage ;
  }
  bool StateMachineManager::is12VPowerGood()
  {
    if (SM_DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("PowerMonitorTask run\n");

    //Analog read of the +12V expected about 3.25V at ADC input.
    // SENSE_24V on A1.
    // Full scale is 1023, ten bits for 3.3V.
    //40K into 10000
    // const long FullScale = 1023;
    // const float percentOK = 0.25;
    // const float R1=40000;
    // const float R2=10000;
    // const float Vcc = 3.3;
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

    if (SM_DEBUG_LEVEL >0 ) {
      CogCore::Debug<const char *>("analogRead(SENSE_12V)= ");
      CogCore::DebugLn<uint32_t>(_v12read);
      CogCore::Debug<float>((float) _v12read * ((Vcc * (R1+R2))/(1023.0 * R2)));
      CogCore::Debug<const char *>("\n");
    }

    if (( _v12read > lowThreshold12V) && ( _v12read < highThreshold12V) ) {
      if (SM_DEBUG_LEVEL >0 )  CogCore::Debug<const char *>("+12V power monitor reports good.\n");
      return true;
    } else{
      if (SM_DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("+12V power monitor reports bad.\n");
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

  bool StateMachineManager::is24VPowerGood()
  {
    if (SM_DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("PowerMonitorTask run\n");

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

    if (SM_DEBUG_LEVEL >0 ) {
      CogCore::Debug<const char *>("analogRead(SENSE_24V)= ");
      CogCore::DebugLn<uint32_t>(_v24read);
      CogCore::Debug<float>((float) _v24read * ((Vcc * (R1+R2))/(1023.0 * R2)));
      CogCore::Debug<const char *>("\n");
    }

    if (( _v24read > lowThreshold24V) && ( _v24read < highThreshold24V) ) {
      if (SM_DEBUG_LEVEL >0 )  CogCore::Debug<const char *>("+24V power monitor reports good.\n");
      return true;
    } else{
      if (SM_DEBUG_LEVEL >0 ) CogCore::Debug<const char *>("+24V power monitor reports bad.\n");
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
}
