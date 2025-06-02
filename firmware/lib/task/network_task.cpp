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
#include <util.h>
#include <debug.h>
#include <network_task.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>

#include <SPI.h>         // needed for Arduino versions later than 0018. Kept for Ethernet.
#include <Ethernet.h>    // Kept as Ethernet hardware might still be used by derived classes (like OEDCSNetworkTask via NetworkMQTT).
// #include <EthernetUdp.h>      // Removed
// #include "utility/w5100.h"   // Removed
// #include <network_udp.h>      // Removed
#include <flash.h>       // Kept as it might be used for other non-UDP purposes.

// extern byte packetBuffer[buffMax]; // Removed

using namespace CogCore;

namespace CogApp
{

  bool NetworkTask::_init() {
    // All NetworkUDP specific initialization has been removed.
    // This includes net_udp.networkStart() and the initial "MachineStart" UDP message.
    // OEDCSNetworkTask now handles its own initialization for MQTT including
    // Ethernet setup and sending its own "MachineStart" message via MQTT.
    
    // If there's any generic, non-UDP network initialization that ALL network tasks
    // might need, it could go here. Otherwise, this method might become trivial
    // or be entirely handled by derived tasks.

    // For now, it does nothing and returns true.
    // CogCore::Debug<const char *>("NetworkTask::_init() called. Was previously UDP network init.\n");
    return true;
  }

  bool NetworkTask::_run()  {
    // All NetworkUDP specific runtime checks (net_udp.networkCheck()) have been removed.
    // Derived tasks like OEDCSNetworkTask are responsible for their own runtime loop logic
    // (e.g., mqtt_client.loop()).

    if (DEBUG_UDP > 1) { // This flag's name (DEBUG_UDP) is now potentially misleading.
      Debug<const char *>("NetworkTask::_run() called. Was previously UDP network check.\n");
    }
    
    // This base method is now very minimal.
    // It could be used for common periodic checks if any apply to all network tasks.
    return true;
  }
}
