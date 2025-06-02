// Copyright (C) 2024 Public Invention.

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

#ifndef NETWORK_MQTT_H
#define NETWORK_MQTT_H

#include <Arduino.h>
#include <Client.h> // PubSubClient dependency
#include <PubSubClient.h>
#include "mqtt_config.h" // Include the new MQTT configuration

// #define MQTT_MAX_PACKET_SIZE 512 // This can be defined in mqtt_config.h or PubSubClient config if needed
                                 // For now, relying on PubSubClient's default or external define.

class NetworkMQTT {
public:
    // Constructor changed to only take client and MAC address
    NetworkMQTT(Client& netClient, const char* macAddress);

    bool connect();
    bool publish(const char* topic, const char* payload, bool retained = false);
    bool subscribe(const char* topic, uint8_t qos = 0); // QoS 0 for now, can be parameterized
    void loop();
    bool isConnected();
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback); // For incoming messages

private:
    PubSubClient _mqttClient;
    // Removed _mqttServer, _mqttPort, _clientId, _lwtTopic, _lwtMessage, _lwtQos, _lwtRetain
    // These will now come from mqtt_config.h or be constructed dynamically.

    // Keep macAddress if needed for reconnect logic, or pass to _reconnect if it needs it
    // For now, assuming macAddress given in constructor is used to set things up and connect() uses it.
    // If _reconnect needs to rebuild clientId or lwtTopic, it might need access to macAddress.
    // Let's add macAddress as a private member to be stored from constructor.
    String _macAddressStr; 


    void _reconnect();
};

#endif // NETWORK_MQTT_H
