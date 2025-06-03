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

#ifndef OEDCS_NETWORK_TASK_H
#define OEDCS_NETWORK_TASK_H

#include <Arduino.h>

#include <core.h>
#include <machine.h>
// #include <network_udp.h> // Replaced by network_mqtt.h
#include "network_mqtt.h" // Use quotes for local lib includes
#include <network_task.h> // Assuming this base class might still be used or refactored later

class EthernetClient; // Forward declaration

namespace CogApp
{
  class OEDCSNetworkTask : public NetworkTask {
  public:
    // int DEBUG_UDP = 0; // Remove or comment out - This was in NetworkTask.h
    int DEBUG_MQTT = 0; // Add this


    bool _init() override; // Added declaration as per subtask instructions
    bool logReport(MachineStatusReport* report);
    bool _run() override;

  private:
    EthernetClient* _eth_client = nullptr;
    NetworkMQTT* _net_mqtt = nullptr;
    // Note: The original net_udp was likely in the NetworkTask base class.
    // This change assumes OEDCSNetworkTask will now manage its own network client (MQTT).
  };

}

#endif
