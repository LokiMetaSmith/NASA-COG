/*
Copyright (C) 2023 Robert Read, Geoff Mulligan.

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

#include <Arduino.h>

#include <debug.h>
#include <network_task.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "utility/w5100.h"
#include <network_udp.h>
#include <OEDCSNetworkTask.h>


// This is defined in network_udp.h. It is true global;
// hopefully it is only referenced here.
extern byte packetBuffer[buffMax];

using namespace CogCore;

// TODO: Move this on to the network_udp object
// uint8_t networkDown = 1;

namespace CogApp
{
  bool OEDCSNetworkTask::_run()  {
    NetworkTask::_run();

    // This is the (currently unused) retrieval of scripts to set parameters
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("Seeking Params\n");
      CogCore::Debug<const char *>("ms\n");
      CogCore::Debug<uint8_t>(getConfig()->report->ms);
      CogCore::Debug<const char *>("\n");
      delay(50);
    }

    // We are currently not using this, due to changes in the
    // control algorithm to the "5 knob" system.
#ifdef RETREIVE_PARAMS_FOR_SCRIPTING
    bool new_packet = NetworkTask::net_udp.getParams(3000);

    bool new_packet = false;
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("Done with Params!\n");
      delay(50);
    }
    if (new_packet) {
      if (DEBUG_UDP > 1) {
	CogCore::Debug<const char *>("Got a Param Packet!\n");
        delay(50);
      }
      if (DEBUG_UDP > 1) {
	CogCore::Debug<const char *>("ms\n");
	CogCore::Debug<uint32_t>(getConfig()->report->ms);
	CogCore::Debug<const char *>("\n");
        delay(50);
      }
      // This would be better done with a static member
      MachineScript *old = getConfig()->script;
      MachineScript *ms = old->parse_buffer_into_new_script((char *) packetBuffer,old->DEBUG_MS);
      if (DEBUG_UDP > 1) {
	CogCore::Debug<const char *>("Done with parse_buffer_into_new_script\n");
	CogCore::Debug<const char *>("ms\n");
	CogCore::Debug<uint32_t>(getConfig()->report->ms);
	CogCore::Debug<const char *>("\n");
        delay(50);
      }
      getConfig()->script = ms;
      delete old;
      if (DEBUG_UDP > 1) {
	CogCore::Debug<const char *>("old Script deleted.\n");
        delay(50);
      }
    }
#endif
    return logReport(getConfig()->report);
  }

  bool OEDCSNetworkTask::logReportList(MachineStatusReport* report, uint8_t size)  {
	 if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("outputReportList\n");
      //delay(50);
    }
    char buffer[4096];
	char payload[65536]; //65536
    // we need to make sure we start with a null string...
	buffer[0] = 0;
	payload[0] = 0;
	for(int i =0; i++; i<size)
	{
		getConfig()->createJSONReport(&report[i],buffer);
		if (DEBUG_UDP > 0) {
			CogCore::Debug<const char *>("Sending buffer:\n");
			CogCore::Debug<const char *>(buffer);
			CogCore::Debug<const char *>("\n");
			//delay(50);
		}
		if( strlen(strncat (payload, buffer, strlen(buffer))) < ( strlen(buffer)+strlen(payload )) ){
			CogCore::Debug<const char *>("Buffer overflow error in UDP:\n");
			return false;
		}
			
	}
    // Conjecture: This should use the report time stamp
    //
    unsigned long current_epoch_time = net_udp.epoch + millis() / 1000;
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("About to Send Data at: ");
	  CogCore::DebugLn<unsigned long>(millis());
     // delay(50);
    }
    net_udp.sendData(payload,current_epoch_time, 2000);
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("Data Sent at: ");
	  CogCore::DebugLn<unsigned long>(millis());
     // delay(50);
    }
    return true;
  }	
	  
  bool OEDCSNetworkTask::logReport(MachineStatusReport* report)  {
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("outputReport\n");
     // delay(50);
    }
    char buffer[4096];
    // we need to make sure we start with a null string...
    buffer[0] = 0;
    getConfig()->createJSONReport(report,buffer);
    if (DEBUG_UDP > 0) {
      CogCore::Debug<const char *>("Sending buffer:\n");
      CogCore::Debug<const char *>(buffer);
      CogCore::Debug<const char *>("\n");
    //  delay(50);
    }
    // Conjecture: This should use the report timestamp
    //
    unsigned long current_epoch_time = net_udp.epoch + millis() / 1000;
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("About to Send Data!\n");
     // delay(50);
    }
    net_udp.sendData(buffer,current_epoch_time, 2000);
    if (DEBUG_UDP > 1) {
      CogCore::Debug<const char *>("Data Sent!\n");
    //  delay(50);
    }
    return true;
  }
}
