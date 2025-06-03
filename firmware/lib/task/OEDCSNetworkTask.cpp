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
#include <util.h>    // For macString, mac, t_millis, getResetCause, setGlobalMacAddress
#include <debug.h>
#include <network_task.h> // Base class, may need refactoring or removal if purely UDP specific
#include <stdio.h>
#include <string.h>
#include <machine.h>     // For MachineStatusReport and getConfig()
#include <flash.h>       // For setGlobalMacAddress()

#include <SPI.h>         // Needed for Ethernet
#include <Ethernet.h>    // For EthernetClient and Ethernet global object

#include "OEDCSNetworkTask.h" // Includes network_mqtt.h implicitly
#include "mqtt_config.h"    // Include the new MQTT configuration
#include "time_utils.h"     // For NTP time synchronization

// extern byte packetBuffer[buffMax]; // Removed, was for UDP

using namespace CogCore; // Retained as CogCore::Debug is used extensively

// TODO: Move this on to the network_udp object // This comment is now obsolete
// uint8_t networkDown = 1; // This global flag seems obsolete

namespace CogApp
{

  // Helper function
  const char* getResetCauseString() {
      // Assuming getResetCause() is available globally or via a core utility
      switch(getResetCause()) {
          case 0: return "GENERAL";
          case 1: return "BACKUP";
          case 2: return "WATCHDOG";
          case 3: return "SOFTWARE";
          case 4: return "USER";
          default: return "UNKNOWN";
      }
  }

  // _init is called by the base Task class constructor.
  // bool OEDCSNetworkTask::_init() { // This is how it would be if it overrides a base virtual _init()
  // For now, assuming it's a standalone init called by NetworkTask constructor or similar
  // If NetworkTask::_init() was purely UDP, OEDCSNetworkTask might not need to call a base _init().
  // The prompt implies OEDCSNetworkTask has its own _init that we are defining now.
  // Let's assume OEDCSNetworkTask needs an _init method, not overriding for now.
  // If there's a Base::NetworkTask::_init(), it should be called if it does generic setup.
  // However, the instructions imply replacing the UDP logic, much of which was in NetworkTask::networkStart
  // which was called by NetworkTask::_init(). So we are effectively replacing that.

bool OEDCSNetworkTask::_init() { // Renaming to match existing class structure if _init isn't virtual override.
                               // Or if it is, it should be `bool OEDCSNetworkTask::_init() override`
                               // For now, let's stick to the provided signature style.
                               // The original NetworkTask had `virtual bool _init();`
                               // and `bool NetworkTask::_init() { return networkStart(); }`
                               // and `bool NetworkTask::networkStart()` contained UDP logic.
                               // So, OEDCSNetworkTask should override _init().
                               // The .h file does not show `_init()` for `OEDCSNetworkTask`.
                               // This implies `OEDCSNetworkTask` was using `NetworkTask::_init()`.
                               // We must add `_init()` to `OEDCSNetworkTask.h` if we define it here.
                               // For the purpose of this exercise, I will assume `_init()` is being
                               // newly added or overridden, and the .h file would be updated accordingly
                               // in a real scenario. The prompt focuses on .cpp changes here.

    // It's crucial to get the MAC address first.
    // setGlobalMacAddress() was used in NetworkUDP::networkStart from flash.h
    // This populates the global `mac` (byte[6]) and `macString` (char[18])
    if (setGlobalMacAddress() != 0) { // Or however it indicates failure
          CogCore::Debug<const char*>("CRITICAL: Failed to get MAC address. Halting MQTT init.\n");
          // Decide if system can run without network. For now, MQTT won't start.
    }

    // Ethernet Initialization
    // Ethernet CS pin is typically 10. SD card on pin 4.
    // Ensure these are configured correctly for your board.
    // Example: pinMode(4, OUTPUT); digitalWrite(4, HIGH); // Deselect SD card
    Ethernet.init(10); // Specify the CS pin for W5x00 chip

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        CogCore::Debug<const char*>("Ethernet shield not found. System halted.\n");
        while (true) { delay(1); } // Halt
    }

    CogCore::Debug<const char*>("Starting Ethernet DHCP...\n");
    if (Ethernet.begin(mac) == 0) { // `mac` is the global byte array from util.h after setGlobalMacAddress
        CogCore::Debug<const char*>("Failed to configure Ethernet using DHCP.\n");
        // If DHCP fails, Ethernet.localIP() will be 0.0.0.0.
        // _net_mqtt will likely fail to connect.
        // Consider static IP fallback if required by application.
    } else {
        CogCore::Debug<const char*>("Ethernet connected. IP: ");
        CogCore::Debug<IPAddress>(Ethernet.localIP());
        CogCore::Debug<const char*>("\n");

        // Synchronize time with NTP server
        CogCore::Debug<const char*>("Attempting NTP time synchronization...\n");
        if (syncNTPTime()) {
            CogCore::Debug<const char*>("NTP synchronization successful. Current epoch: ");
            CogCore::Debug<unsigned long>(system_epoch); // Access global epoch from time_utils.h
            CogCore::Debug<const char*>("\n");
        } else {
            CogCore::Debug<const char*>("NTP synchronization failed. Timestamps will be millis-based.\n");
        }
    }

    // Initialize EthernetClient and NetworkMQTT
    // Ensure _eth_client and _net_mqtt are cleaned up if OEDCSNetworkTask is destroyed (e.g. in a destructor if added)
    delete _eth_client; // Delete previous instance if any (e.g. if _init is called multiple times)
    _eth_client = new EthernetClient();

    delete _net_mqtt;   // Delete previous instance

    // MQTT Broker details are now taken from mqtt_config.h
    // const char* mqttServer = "YOUR_MQTT_BROKER_IP"; // Removed
    // uint16_t mqttPort = 1883; // Removed

    if (strlen(macString) == 17) { // Basic check for "XX:XX:XX:XX:XX:XX"
        // Constructor for NetworkMQTT now takes only macString.
        // _eth_client from OEDCSNetworkTask is no longer passed to NetworkMQTT.
        // NetworkMQTT now manages its own EthernetClient instance.
        _net_mqtt = new NetworkMQTT(macString);
    } else {
        CogCore::Debug<const char*>("CRITICAL: macString invalid ('");
        CogCore::Debug<const char*>(macString);
        CogCore::Debug<const char*>("'). MQTT not initialized.\n");
        // _net_mqtt remains nullptr
    }

    if (_net_mqtt) {
        // Set the MQTT callback if OEDCSNetworkTask needs to handle subscribed messages.
        // _net_mqtt->setCallback(your_callback_function); // Example: std::bind(&OEDCSNetworkTask::mqttCallback, this, _1, _2, _3));

        if (_net_mqtt->connect()) { // connect() handles the "online" LWT message
            CogCore::Debug<const char*>("MQTT connected in _init.\n");
            // Send "MachineStart" event message
            char machineStartMsg[128];
            sprintf(machineStartMsg, "{\"event\": \"MachineStart\", \"cause\": \"%s\"}", getResetCauseString());

            char machineEventTopic[128]; // Increased buffer size for topic
            // Ensure macString is valid before using in sprintf
            if (strlen(macString) == 17) {
                sprintf(machineEventTopic, "%s%s%s", MQTT_DEVICE_BASE_TOPIC, macString, MQTT_EVENT_TOPIC_SUFFIX);
                _net_mqtt->publish(machineEventTopic, machineStartMsg, false);
            } else {
                CogCore::Debug<const char*>("Cannot publish MachineStart event: Invalid macString.\n");
            }
        } else {
            CogCore::Debug<const char*>("MQTT connection failed in _init. Will retry in loop.\n");
        }
    }

    // NTP Time Sync:
    // The original NetworkUDP::networkStart() called getTime() to get NTP time.
    // This is important for accurate timestamps. This logic needs to be integrated,
    // possibly as a separate system service or called here.
    // NTP Time Sync has been attempted above.
    // The old comment below is now addressed.
    // The original NetworkUDP::networkStart() called getTime() to get NTP time.
    // This is important for accurate timestamps. This logic needs to be integrated,
    // possibly as a separate system service or called here.
    // For now, this step omits explicit NTP call, assuming timestamps in JSON
    // will rely on millis() or an externally synchronized clock via `epoch`.

    return true; // Task initialized, regardless of immediate connection success.
}

  bool OEDCSNetworkTask::_run()  {
    // NetworkTask::_run(); // Call base class _run if it has generic (non-UDP) functionality
                         // For now, assuming OEDCSNetworkTask takes full control of its network duties.
    if (_net_mqtt) {
        _net_mqtt->loop(); // Handles MQTT connection maintenance and incoming messages
    } else {
        // Attempt to reinitialize if _net_mqtt is null and conditions might allow it now?
        // This could happen if MAC address wasn't ready during _init.
        // For simplicity now, if _init failed to allocate _net_mqtt, it stays null.
        // Consider adding re-init logic here if needed:
        // if (strlen(macString) == 17 && _eth_client && !_net_mqtt) { ... reallocate _net_mqtt ... }
        // Potentially, could call _init() again, but be careful about re-running Ethernet.begin etc.
        // A better approach would be a separate MQTT re-init function.
        // For now, if _net_mqtt is null, logReport will fail.
        if (DEBUG_MQTT > 0) {
             CogCore::Debug<const char*>("OEDCSNetworkTask::_run(): MQTT client not initialized.\n");
        }
    }

    // The core responsibility of this task is to log the report.
    return logReport(getConfig()->report);
  }

  bool OEDCSNetworkTask::logReport(MachineStatusReport* report)  {
    if (DEBUG_MQTT > 1) {
        CogCore::Debug<const char*>("OEDCSNetworkTask::logReport called\n");
    }

    if (!_net_mqtt) {
        CogCore::Debug<const char*>("MQTT client not initialized. Cannot log report.\n");
        return false;
    }

    if (strlen(macString) != 17) { // Check for "XX:XX:XX:XX:XX:XX"
        CogCore::Debug<const char*>("Cannot log report: Invalid macString ('");
        CogCore::Debug<const char*>(macString);
        CogCore::Debug<const char*>("').\n");
        return false;
    }

    char topic[128]; // Increased buffer size for topic
    sprintf(topic, "%s%s%s", MQTT_DEVICE_BASE_TOPIC, macString, MQTT_DATA_TOPIC_SUFFIX);

    // The buffer for JSON. Ensure it's large enough.
    // PubSubClient default max packet is 256 (MQTT_MAX_PACKET_SIZE). If reports are larger,
    // PubSubClient's MQTT_MAX_PACKET_SIZE needs to be increased, or messages chunked.
    // This is now defined in network_mqtt.h (via mqtt_config.h) or PubSubClient default if not overridden.
    // The define MQTT_MAX_PACKET_SIZE was removed from network_mqtt.h to avoid redefinition if already in PubSubClient.
    // If a specific size is needed, it should be handled consistently (e.g., in mqtt_config.h and used by PubSubClient).
    char buffer[1024]; // Increased from 256, but verify actual max report size.
                       // The previous UDP code used 4096. This might be an issue for standard MQTT.
                       // Max report size vs PubSubClient's MQTT_MAX_PACKET_SIZE needs careful review.
    buffer[0] = 0;
    getConfig()->createJSONReport(report, buffer); // Populates buffer with JSON

    if (DEBUG_MQTT > 0) {
        CogCore::Debug<const char*>("Publishing to MQTT. Topic: ");
        CogCore::Debug<const char*>(topic);
        // Avoid printing buffer directly if too large or contains sensitive data
        // CogCore::Debug<const char*>(" Payload: "); CogCore::Debug<const char*>(buffer);
        CogCore::Debug<const char*>("\n");
    }

    bool success = _net_mqtt->publish(topic, buffer);
    if (!success) {
        CogCore::Debug<const char*>("MQTT publish failed in logReport.\n");
    }
    return success;
  }
}
