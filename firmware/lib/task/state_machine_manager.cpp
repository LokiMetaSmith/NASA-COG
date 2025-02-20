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
    unsigned long now = millis();
    MachineState rms = ms;
    for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++) {
      if (getConfig()->errors[i].fault_present) {
        if (!MachineConfig::IsAShutdownState(getConfig()->ms)) {
          CogCore::Debug<const char *>("WILL AUTOMATICALLY SHUTDOWN IF NOT RESTORED IN ");
          unsigned long now = millis();
          CogCore::Debug<float>((((float) getConfig()->errors[i].toleration_ms) -
                                 ((float) now - (float) getConfig()->errors[i].begin_condition_ms)) / (float) 1000);
          CogCore::Debug<const char *>(" SECONDS DUE TO : ");
          CogCore::DebugLn<const char *>(CriticalErrorNames[i]);
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
  // There is significant COG dependent logic here.
  // At the expense of extra lines of code, I'm
  // going to keep this mostly simple by making it look
  // "table-driven"
  MachineState StateMachineManager::_executeBasedOnState(MachineState ms) {
    MachineState new_ms;

    if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("\nMachine State: ");
      CogCore::Debug<const char *>(getConfig()->MachineStateNames[ms]);
      CogCore::Debug<const char *>(" : ");
      CogCore::Debug<const char *>(getConfig()->MachineSubStateNames[getConfig()->idleOrOperate]);
      CogCore::Debug<const char *>("\n");
    }

    new_ms = checkCriticalFaults(ms);

    switch(new_ms) {
    case Off:
      new_ms = _updatePowerComponentsOff();
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
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_UP = (ms - begin_up_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_UP * getConfig()->RAMP_UP_TARGET_D_MIN;
    tt = min(tt,getConfig()->TARGET_TEMP_C);
    tt = min(tt,getConfig()->BOUND_MAX_TEMP);
    return tt;
  }
  float StateMachineManager::computeRampDnSetpointTemp(float t,float recent_t,unsigned long begin_dn_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_DN = (ms - begin_dn_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_DN * getConfig()->RAMP_DN_TARGET_D_MIN;
    tt = max(tt,getConfig()->TARGET_TEMP_C);
    tt = max(tt,getConfig()->BOUND_MIN_TEMP);
    return tt;
  }

  void StateMachineManager::turnOff() {
    //    heaterPIDTask->shutHeaterDown();
    Serial.println("StateMachineManager::turnOff() called which does nothing.");
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
    getConfig()->BEGIN_UP_TIME_MS = millis();
  }

  void StateMachineManager::transitionToCooldown(float recent) {
    getConfig()->previous_ms = getConfig()->ms;
    getConfig()->ms = Cooldown;
    getConfig()->COOL_DOWN_BEGIN_TEMP = recent;
    getConfig()->SETPOINT_TEMP_C = recent;
    getConfig()->BEGIN_DN_TIME_MS = millis();
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
  }
  MachineState StateMachineManager::_updatePowerComponentsEmergencyShutdown() {
	if (SM_DEBUG_LEVEL > 0) {
      CogCore::Debug<const char *>("Emergency Shutdown!\n");
    }
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

}
