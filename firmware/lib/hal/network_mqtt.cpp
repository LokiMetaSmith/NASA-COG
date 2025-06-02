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

    _lastReconnectAttemptMillis = millis(); // Set initial attempt time for _reconnect logic

    CogCore::Debug<const char*>("Attempting MQTT connection (initial)...
");

    String clientIdStr = MQTT_CLIENT_ID_PREFIX;
    clientIdStr += _macAddressStr;

    String lwtTopicStr = MQTT_DEVICE_BASE_TOPIC;
    lwtTopicStr += _macAddressStr;
    lwtTopicStr += LWT_TOPIC_SUFFIX;

    const char* mqttUser = MQTT_USERNAME;
    const char* mqttPass = MQTT_PASSWORD;
    bool useCredentials = (strlen(mqttUser) > 0);

    bool success;
    if (useCredentials) {
        CogCore::Debug<const char*>("Connecting with MQTT credentials. User: ");
        CogCore::Debug<const char*>(mqttUser);
        CogCore::Debug<const char*>("...
");
        success = _mqttClient.connect(clientIdStr.c_str(), mqttUser, mqttPass,
                                      lwtTopicStr.c_str(), LWT_QOS, LWT_RETAIN, LWT_MESSAGE_OFFLINE);
    } else {
        CogCore::Debug<const char*>("Connecting without MQTT credentials.
");
        success = _mqttClient.connect(clientIdStr.c_str(), 
                                      lwtTopicStr.c_str(), LWT_QOS, LWT_RETAIN, LWT_MESSAGE_OFFLINE);
    }

    if (success) {
        CogCore::Debug<const char*>("MQTT connected.
");
        publish(lwtTopicStr.c_str(), LWT_MESSAGE_ONLINE, LWT_RETAIN);
        // Resubscribe logic here if needed
    } else {
        CogCore::Debug<const char*>("MQTT connection failed, rc=");
        CogCore::Debug<int>(_mqttClient.state());
        CogCore::Debug<const char*>("
");
    }
    return _mqttClient.connected();
}

// Handles reconnection logic (non-blocking)
void NetworkMQTT::_reconnect() {
    // Check if it's time to try connecting again
    if (millis() - _lastReconnectAttemptMillis > _reconnectIntervalMillis) {
        _lastReconnectAttemptMillis = millis(); // Update the last attempt time

        CogCore::Debug<const char*>("Attempting MQTT reconnection (non-blocking)...
");
        
        // Construct client ID and LWT topic strings (as they are not members)
        String clientIdStr = MQTT_CLIENT_ID_PREFIX;
        clientIdStr += _macAddressStr; 

        String lwtTopicStr = MQTT_DEVICE_BASE_TOPIC;
        lwtTopicStr += _macAddressStr;
        lwtTopicStr += LWT_TOPIC_SUFFIX;

        // Determine if username and password are set
        const char* mqttUser = MQTT_USERNAME;
        const char* mqttPass = MQTT_PASSWORD;
        bool useCredentials = (strlen(mqttUser) > 0); // Connect with credentials if username is present

        bool connected;
        if (useCredentials) {
            CogCore::Debug<const char*>("Reconnecting with MQTT credentials. User: ");
            CogCore::Debug<const char*>(mqttUser);
            CogCore::Debug<const char*>("...
");
            connected = _mqttClient.connect(clientIdStr.c_str(), mqttUser, mqttPass,
                                            lwtTopicStr.c_str(), LWT_QOS, LWT_RETAIN, LWT_MESSAGE_OFFLINE);
        } else {
            CogCore::Debug<const char*>("Reconnecting without MQTT credentials.
");
            connected = _mqttClient.connect(clientIdStr.c_str(), 
                                            lwtTopicStr.c_str(), LWT_QOS, LWT_RETAIN, LWT_MESSAGE_OFFLINE);
        }

        if (connected) {
            CogCore::Debug<const char*>("MQTT reconnected.
");
            // Publish "online" message to LWT topic
            publish(lwtTopicStr.c_str(), LWT_MESSAGE_ONLINE, LWT_RETAIN);
            // Resubscribe logic might be needed here if subscriptions are used
        } else {
            CogCore::Debug<const char*>("MQTT reconnection failed, rc=");
            CogCore::Debug<int>(_mqttClient.state());
            CogCore::Debug<const char*>(". Will retry later.
");
        }
    }
    // If not enough time has passed, this function does nothing, allowing other code to run.
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
