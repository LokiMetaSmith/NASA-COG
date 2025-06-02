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

#include "network_mqtt.h"
// mqtt_config.h is already included via network_mqtt.h
#include <util.h> // For macString, though ideally pass MAC more directly. Keep for now if CogCore::Debug needs it.
#include <debug.h> // For CogCore::Debug

// Constructor updated to use mqtt_config.h
NetworkMQTT::NetworkMQTT(Client& netClient, const char* macAddress)
    : _mqttClient(netClient), _macAddressStr(macAddress) { // Store macAddress
    
    // Server and Port are now from mqtt_config.h
    _mqttClient.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);

    // Client ID and LWT topic are constructed dynamically when needed (e.g., in connect())
    // using _macAddressStr and defines from mqtt_config.h.
    // No need to store them as members if they are reconstructed each time in connect/reconnect.
    // PubSubClient itself doesn't store the clientID string pointer after connect call returns,
    // so it must be valid during the call.
    
    // If MQTT_MAX_PACKET_SIZE needs to be set on _mqttClient, it would be done here,
    // e.g. _mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE_FROM_CONFIG);
    // However, PubSubClient's buffer size is typically set by a #define before including PubSubClient.h
    // or by modifying PubSubClient.h itself. The setBufferSize may not be standard for all versions or always effective.
    // For now, assume default or globally defined MQTT_MAX_PACKET_SIZE.
}

void NetworkMQTT::setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
    _mqttClient.setCallback(callback);
}

bool NetworkMQTT::connect() {
    if (_mqttClient.connected()) {
        return true;
    }

    // Construct Client ID
    String clientIdStr = MQTT_CLIENT_ID_PREFIX;
    clientIdStr += _macAddressStr;

    // Construct LWT Topic
    String lwtTopicStr = MQTT_DEVICE_BASE_TOPIC;
    lwtTopicStr += _macAddressStr;
    lwtTopicStr += LWT_TOPIC_SUFFIX;

    CogCore::Debug<const char*>("Attempting MQTT connection with Client ID: ");
    CogCore::Debug<const char*>(clientIdStr.c_str());
    CogCore::Debug<const char*>(" LWT Topic: ");
    CogCore::Debug<const char*>(lwtTopicStr.c_str());
    CogCore::Debug<const char*>("...
");

    if (_mqttClient.connect(clientIdStr.c_str(), lwtTopicStr.c_str(), LWT_QOS, LWT_RETAIN, LWT_MESSAGE_OFFLINE)) {
        CogCore::Debug<const char*>("MQTT connected.
");
        // Publish online message to LWT topic
        publish(lwtTopicStr.c_str(), LWT_MESSAGE_ONLINE, LWT_RETAIN);
        
        // Resubscribe to any necessary topics if needed upon reconnection
        // This part would require knowledge of topics this specific client instance needs to be subscribed to.
        // Example: 
        // if (_subscribedTopic && strlen(_subscribedTopic) > 0) { 
        //    subscribe(_subscribedTopic); 
        // }
    } else {
        CogCore::Debug<const char*>("MQTT connection failed, rc=");
        CogCore::Debug<int>(_mqttClient.state());
        CogCore::Debug<const char*>("
");
    }
    return _mqttClient.connected();
}

// Handles reconnection logic
void NetworkMQTT::_reconnect() {
    // Loop until we're reconnected
    // TODO: Add a timeout or max retries to avoid blocking indefinitely
    unsigned long startAttemptTime = millis();
    while (!_mqttClient.connected()) {
        if (millis() - startAttemptTime > 30000) { // Try for 30 seconds
             CogCore::Debug<const char*>("MQTT reconnection timed out.
");
             return; // Give up after 30 seconds
        }
        CogCore::Debug<const char*>("Attempting MQTT reconnection...
");
        if (connect()) { // connect() already prints messages
            CogCore::Debug<const char*>("MQTT reconnected.
");
            break; 
        } else {
            CogCore::Debug<const char*>("MQTT reconnection failed, rc=");
            CogCore::Debug<int>(_mqttClient.state());
            CogCore::Debug<const char*>(". Retrying in 5 seconds...
");
            // Wait 5 seconds before retrying
            delay(5000); 
        }
    }
}


bool NetworkMQTT::publish(const char* topic, const char* payload, bool retained) {
    if (!isConnected()) {
        _reconnect(); // Try to reconnect if not connected
        if (!isConnected()) {
             CogCore::Debug<const char*>("MQTT publish failed: Not connected.
");
            return false;
        }
    }
    if (_mqttClient.publish(topic, payload, retained)) {
        CogCore::Debug<const char*>("MQTT message published. Topic: ");
        CogCore::Debug<const char*>(topic);
        // CogCore::Debug<const char*>(" Payload: "); // Be careful with large payloads
        // CogCore::Debug<const char*>(payload);
        CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("MQTT message publish failed. Topic: ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>("
");
        return false;
    }
}

bool NetworkMQTT::subscribe(const char* topic, uint8_t qos) {
    if (!isConnected()) {
        _reconnect(); // Try to reconnect if not connected
         if (!isConnected()) {
            CogCore::Debug<const char*>("MQTT subscribe failed: Not connected.
");
            return false;
        }
    }
    if (_mqttClient.subscribe(topic, qos)) {
        CogCore::Debug<const char*>("Subscribed to MQTT topic: ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("Failed to subscribe to MQTT topic: ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>("
");
        return false;
    }
}

void NetworkMQTT::loop() {
    if (!_mqttClient.connected()) {
        _reconnect();
    }
    _mqttClient.loop(); // Essential for PubSubClient to process messages and keepalive
}

bool NetworkMQTT::isConnected() {
    return _mqttClient.connected();
}
