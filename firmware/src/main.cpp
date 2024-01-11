/* Copyright (C) 2023 Robert Read, Ben Coombs.

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

// Program information
#define COMPANY_NAME "pubinv.org "
//#define PROG_NAME "main.cpp"
#define PROG_NAME "OEDCS"
#define VERSION "; Rev: 0.3.14"  // Fan controlled version
#define DEVICE_UNDER_TEST "Hardware: Due"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#ifdef ARDUINO
#include <Arduino.h>
//#include <display.h>
//
#endif

#include <core.h>
#include <cog_hal.h>

#include <flash.h>
#include <network_task.h>
#include <cog_task.h>
#include <serial_input_task.h>
#include <fault_task.h>
#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <serialReportTask.h>
#include <OEDCSNetworkTask.h>
#include <heartbeat_task.h>
#include <log_recorder_task.h>
#include <display_task.h>
#include <shutdown_button_task.h>

#ifdef TEST_FANS_ONLY
#include <fanTEST_task.h>
#endif

using namespace CogCore;
static Core core;

/***** Declare your tasks here *****/
DisplayTask displayTask;
CogApp::OEDCSNetworkTask OEDCSNetworkTask;
CogApp::CogTask cogTask;
CogApp::OEDCSSerialInputTask oedcsSerialInputTask;
CogApp::FaultTask faultTask;

CogApp::HeartbeatTask heartbeatTask;
CogApp::Log_Recorder_Task logRecorderTask;


HeaterPIDTask heaterPIDTask;
DutyCycleTask dutyCycleTask;
ReadTempsTask readTempsTask;
SerialReportTask serialReportTask;
ShutdownButtonTask shutdownButtonTask;
#include <machine.h>

MachineConfig machineConfig;
/***********************************/

#define ETHERNET_REQUIRED 1
// #define ETHERNET_REQUIRED 0 //No ethernet.


// This is to allow a code idiom compatible with the way
// the machine config is found  inside the Tasks
MachineConfig *getConfig() {
  return &machineConfig;
}

// TODO: we need to have setups for individual pieces
// of the Hardware Abstraction Layer
// I don't know why this didn't work inside the core.cpp file!!!


void setup()
{
  CogCore::serialBegin(115200UL);
  delay(500);

   // WARNING! need 5 second delay for pio compiler it seems
  // DO NOT REMOVE THIS STATEMENT!
  delay(5000);


  // We're doing this here because the Core may not be initialized
  watchdogReset();


  // TODO: consider doing this....
    // Serial.begin(BAUDRATE);
    // while (!Serial) {
    //   watchdogReset();
    // }
    // Debug<const char *>("starting\n");

  //Debug<const char *>("Starting COG...\n");
  Debug<const char *>("Starting ");
  Debug<const char *>(PROG_NAME);
  Debug<const char *>(VERSION);
  Debug<const char *>("\n");

  Debug<const char *>("Build: ");
  Debug<const char *>((__DATE__ " " __TIME__));
  Debug<const char *>("\n");

   //Print out the reset reason
  Debug<const char *>("=================\n");
  Debug<const char *>("ResetCause: ");
  switch(getResetCause()) {
  case 0: Debug<const char *>("general\n"); break;
  case 1: Debug<const char *>("backup\n"); break;
  case 2: Debug<const char *>("watchdog\n"); break;
  case 3: Debug<const char *>("software\n"); break;
  case 4: Debug<const char *>("user\n"); break;
  }
  Debug<const char *>("=================\n");

  // TODO: consider doing this....
    // Serial.begin(BAUDRATE);
    // while (!Serial) {
    //   watchdogReset();
    // }
    // Debug<const char *>("starting\n");

  delay(100);
  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }
  Debug<const char *>("Core booted...\n");
  delay(100);

  core.ResetAllWatchdogs();

  machineConfig.init();
  //  Eventually we will migrate all hardware to the COG_HAL..
  COG_HAL* hal = new COG_HAL();
  machineConfig.hal = hal;

  machineConfig.hal->DEBUG_HAL = 0;
  bool initSuccess  = machineConfig.hal->init();
  if (!initSuccess) {
    Debug<const char *>("Could not init Hardware Abstraction Layer Properly!\n");
    delay(50);
    abort();
  } else {
    Debug<const char *>("Successful init of Hardware Abstraction Layer!\n");
  }

  // Now we will set the machine state to "Off"
  getConfig()->ms = Off;

  /***** Configure and add your tasks here *****/


  CogCore::TaskProperties DisplayProperties;
  DisplayProperties.name = "Display";
  DisplayProperties.id = 50;
  DisplayProperties.period = MachineConfig::DISPLAY_UPDATE_MS;
  DisplayProperties.priority = CogCore::TaskPriority::Low;
  DisplayProperties.state_and_config = (void *) &machineConfig;
  bool displayAdd = core.AddTask(&displayTask, &DisplayProperties);

  if (!displayAdd) {
    CogCore::Debug<const char *>("displayAdd Failed\n");
    abort();
  }

  CogCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 19;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = CogCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) &machineConfig;
  bool readAdd = core.AddTask(&readTempsTask, &readTempsProperties);
  if (!readAdd) {
    CogCore::Debug<const char *>("ReadTemps Task add failed\n");
    abort();
  }

  CogCore::TaskProperties serialReportProperties;
  serialReportProperties.name = "serialReportTemps";
  serialReportProperties.id = 20;
  serialReportProperties.period = serialReportTask.PERIOD_MS;
  serialReportProperties.priority = CogCore::TaskPriority::High;
  serialReportProperties.state_and_config = (void *) &machineConfig;
  bool serialReportAdd = core.AddTask(&serialReportTask, &serialReportProperties);
  if (!serialReportAdd) {
    CogCore::Debug<const char *>("serialReport Task add failed\n");
    abort();
  }
  CogCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 21;
  cogProperties.period = cogTask.PERIOD_MS;
  cogProperties.priority = CogCore::TaskPriority::High;
  cogProperties.state_and_config = (void *) &machineConfig;
  bool cogAdd = core.AddTask(&cogTask, &cogProperties);
  if (!cogAdd) {
    CogCore::Debug<const char *>("Cognitive Task add failed\n");
    abort();
  }

  getConfig()->ms = Off;
  cogTask.heaterPIDTask = &heaterPIDTask;


  CogCore::TaskProperties oedcsSerialProperties;
  oedcsSerialProperties.name = "oedcsSerial";
  oedcsSerialProperties.id = 22;
  oedcsSerialProperties.period = oedcsSerialInputTask.PERIOD_MS;
  oedcsSerialProperties.priority = CogCore::TaskPriority::High;
  oedcsSerialProperties.state_and_config = (void *) &machineConfig;
   bool oedcsSerialAdd = core.AddTask(&oedcsSerialInputTask, &oedcsSerialProperties);
  if (!oedcsSerialAdd) {
    CogCore::Debug<const char *>("SerialInputProperties add failed\n");
    abort();
  }
  oedcsSerialInputTask.cogTask = &cogTask;

  if (ETHERNET_REQUIRED) {
    CogCore::TaskProperties OEDCSNetworkProperties;
    OEDCSNetworkProperties.name = "OEDCSNetwork";
    OEDCSNetworkProperties.id = 24;
    OEDCSNetworkProperties.period = 5000;
    OEDCSNetworkProperties.priority = CogCore::TaskPriority::High;
    OEDCSNetworkProperties.state_and_config = (void *) &machineConfig;

    bool OEDCSNetwork = core.AddTask(&OEDCSNetworkTask, &OEDCSNetworkProperties);
    if (!OEDCSNetwork) {
      CogCore::Debug<const char *>("Retrieve Script UDP\n");
      abort();
    }
  }

  dutyCycleTask.whichHeater = (Stage2Heater) 0;

  CogCore::Debug<const char *>("Duty Cycle Setup\n");
  CogCore::TaskProperties dutyCycleProperties;
  dutyCycleProperties.name = "dutyCycle";
  dutyCycleProperties.id = 25;
  dutyCycleProperties.period = dutyCycleTask.PERIOD_MS;
  dutyCycleProperties.priority = CogCore::TaskPriority::Low;
  dutyCycleProperties.state_and_config = (void *) &machineConfig;
  bool dutyCycleAdd = core.AddTask(&dutyCycleTask, &dutyCycleProperties);
  if (!dutyCycleAdd) {
    CogCore::Debug<const char *>("dutyCycleAdd Failed\n");
    abort();
  }
  dutyCycleTask.one_pin_heater = getConfig()->hal->_ac_heaters[0];

  CogCore::TaskProperties HeaterPIDProperties;
  HeaterPIDProperties.name = "HeaterPID";
  HeaterPIDProperties.id = 26;
  HeaterPIDProperties.period = MachineConfig::INIT_PID_PERIOD_MS;
  HeaterPIDProperties.priority = CogCore::TaskPriority::High;
  HeaterPIDProperties.state_and_config = (void *) &machineConfig;
  bool heaterPIDAdd = core.AddTask(&heaterPIDTask, &HeaterPIDProperties);

  if (!heaterPIDAdd) {
    CogCore::Debug<const char *>("heaterPIDAdd Failed\n");
    abort();
  }

  CogCore::TaskProperties HeartbeatProperties;
  HeartbeatProperties.name = "Heartbeat";
  HeartbeatProperties.id = 27;
  HeartbeatProperties.period = MachineConfig::INIT_HEARTBEAT_PERIOD_MS;
  HeartbeatProperties.priority = CogCore::TaskPriority::High;
  HeartbeatProperties.state_and_config = (void *) &machineConfig;
  bool heartbeatAdd = core.AddTask(&heartbeatTask, &HeartbeatProperties);

  if (!heartbeatAdd) {
    CogCore::Debug<const char *>("heartbeatAdd Failed\n");
    abort();
  }

  CogCore::TaskProperties Log_RecorderProperties;
  Log_RecorderProperties.name = "Log_Recorder";
  Log_RecorderProperties.id = 28;
  Log_RecorderProperties.period = MachineConfig::INIT_LOG_RECORDER_PERIOD_MS;
  Log_RecorderProperties.priority = CogCore::TaskPriority::High;
  Log_RecorderProperties.state_and_config = (void *) &machineConfig;
  cogTask.logRecorderTask = &logRecorderTask;
  bool Log_RecorderAdd = core.AddTask(&logRecorderTask, &Log_RecorderProperties);

  if (!Log_RecorderAdd) {
    CogCore::Debug<const char *>("Log_RecorderAdd Failed\n");
    abort();
  }
  CogCore::TaskProperties ShutdownButtonProperties;
  ShutdownButtonProperties.name = "ShutdownButton";
  ShutdownButtonProperties.id = 29;
  ShutdownButtonProperties.period = MachineConfig::INIT_SHUTDOWN_BUTTON_PERIOD_MS;
  ShutdownButtonProperties.priority = CogCore::TaskPriority::High;
  ShutdownButtonProperties.state_and_config = (void *) &machineConfig;
  bool shutdownButtonAdd = core.AddTask(&shutdownButtonTask, &ShutdownButtonProperties);

  if (!shutdownButtonAdd) {
    CogCore::Debug<const char *>("ShutdownButtonAdd Failed\n");
    abort();
  }

  core.ResetAllWatchdogs();

  heaterPIDTask.whichHeater = (Stage2Heater) 0;

  cogTask.dutyCycleTask = &dutyCycleTask;

  cogTask.heaterPIDTask = &heaterPIDTask;

  logRecorderTask.oedcsNetworkTask = &OEDCSNetworkTask;
  // We need the core on logRecorderTask (and, indeed, any long-running task
  // so that we can "feed the dog" for the software watchdog there
  logRecorderTask.core = &core;

  // Now we will check for anything that causes us to enter a critical error, such as missing hardware
  // components. We could not have done this earlier, because we want this to have our CogTask and
  // our logger set up...

  // NOTHING HERE YET...

  // now set up debugging levels...
  logRecorderTask.DEBUG_LOG_RECORDER = 0;
  core.DEBUG_CORE = 0;
  core._scheduler.DEBUG_SCHEDULER = 0;
  core._scheduler._idleTask.DEBUG_IDLETASK = 0;
  dutyCycleTask.DEBUG_DUTY_CYCLE = 0;
  heaterPIDTask.DEBUG_PID = 0;
  cogTask.DEBUG_FAN = 0;
  cogTask.DEBUG_LEVEL = 0;
  cogTask.SM_DEBUG_LEVEL = 0;
  cogTask.DEBUG_LEVEL_OBA = 0;
  cogTask.wattagePIDObject->DEBUG_PID = 0;
  OEDCSNetworkTask.DEBUG_UDP = 0;
  OEDCSNetworkTask.net_udp.DEBUG_UDP = 0;
  readTempsTask.DEBUG_READ_TEMPS = 0;
  oedcsSerialInputTask.DEBUG_SERIAL = 0;
  heartbeatTask.DEBUG_HEARTBEAT = 0;
  heartbeatTask.debug_number_of_heartbeats = millis() / 500;
  getConfig()->script->DEBUG_MS = 0;


  CogCore::Debug<const char *>("Added tasks\n");

  // Now we will set the initial tunings for the heater_pid tasks
  // This is a place where one could change the settings for
  // one of the heaters but not another.

  heaterPIDTask.SetTunings(hal->INIT_Kp, hal->INIT_Ki, hal->INIT_Kd);


  // We want to make sure we have run the temps before we start up.

  readTempsTask._run();
  readTempsTask._run();
  readTempsTask._run();
  getConfig()->GLOBAL_RECENT_TEMP = getConfig()->report->post_heater_C;
  Debug<const char *>("starting temp is: ");
  Debug<uint32_t>(getConfig()->GLOBAL_RECENT_TEMP);
  Debug<const char *>("\n");
  CogCore::Debug<const char *>("Starting\n");
  /*********************************************/
}


void loop() {
  CogCore::Debug<const char *>("Loop starting...\n");

  // Blocking call
  if (core.Run() == false) {
      CogCore::ErrorHandler::Log(CogCore::ErrorLevel::Critical, CogCore::ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
      // make sure we print anything needed!
      Debug<const char *>("Critical error!\n");
      delay(100);
      // Loop endlessly to stop the program from running
      Debug<const char *>("INTERNAL ERROR (CORE RETURNED)!\n");
      delay(1000);
      abort();
#endif
      return;
  } else {
    Debug<const char *>("INTERNAL ERROR (CORE DID NOT START)!\n");
    delay(300000);
    abort();
  }
}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  while (true) {
    loop();
  }
  return 0;
}
#endif
