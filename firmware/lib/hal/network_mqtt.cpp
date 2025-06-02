// Copyright (C) 2024 Public Invention.
// This file is part of the OEDCS project.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include "network_mqtt.h"
#include <debug.h> // For CogCore::Debug
// util.h might not be needed here if macString is handled via _macAddressStr

// Initialize static members
NetworkMQTT* NetworkMQTT::_instance = nullptr;

NetworkMQTT::NetworkMQTT(const char* macAddress)
    : _macAddressStr(macAddress),
      // Initialize MQTTClient with buffer sizes.
      // Read buffer 1024 for incoming messages, write buffer 256 (default) or 1024 for outgoing.
      // Let's use 1024 for read based on previous findings about report size, and 256 for write.
      _mqttClient(1024, 256) {
    _instance = this; // Store instance for static callback

    // Construct client ID
    _clientIdStr = MQTT_CLIENT_ID_PREFIX;
    _clientIdStr += _macAddressStr;

    // For now, network_client_for_mqtt points to the plain EthernetClient.
    // TLS setup would happen in _setupSecureClient and potentially reassign this pointer
    // or configure _ethernet_client to become a TLS client itself (less likely with stock Ethernet library).
    _network_client_for_mqtt = &_ethernet_client; 

    _mqttClient.onMessage(NetworkMQTT::_internalMessageReceived);
}

NetworkMQTT::~NetworkMQTT() {
    if (_mqttClient.connected()) {
        _mqttClient.disconnect();
    }
    // If _network_client_for_mqtt was dynamically allocated (e.g. a TLS client wrapper), delete it here.
    // Since it currently points to _ethernet_client (a member), no explicit deletion here.
}

void NetworkMQTT::_internalMessageReceived(String &topic, String &payload) {
    if (_instance && _instance->_onMessageCallback) {
        _instance->_onMessageCallback(topic, payload);
    } else {
        CogCore::Debug<const char*>("MQTT message received but no callback set or instance not found. Topic: ");
        // CogCore::Debug<const char*>(topic.c_str()); // String.c_str() if needed by Debug
        // Payload might be large, consider logging only a part or its length
    }
}

void NetworkMQTT::onMessage(std::function<void(String &topic, String &payload)> callback) {
    _onMessageCallback = callback;
}

void NetworkMQTT::_setupSecureClient() {
    // Placeholder: Actual TLS and WebSocket setup for EthernetClient (W5x00) is complex
    // and requires a TLS library (e.g., BearSSL) and potentially a WebSocket client wrapper.
    // 
    // If MQTT_BROKER_WEBSOCKET_URI is configured in mqtt_config.h and is not empty:
    // 1. Parse the URI to extract hostname, port, path, and scheme (ws or wss).
    // 2. If "wss" (secure WebSocket):
    //    - Initialize the TLS client (e.g., BearSSLClient) with CA cert, client cert, client key.
    //    - Wrap the TLS client with a WebSocket client, configuring it with the hostname, port, and path.
    //    - _network_client_for_mqtt would point to this WebSocket client.
    // 3. If "ws" (non-secure WebSocket):
    //    - Wrap the _ethernet_client with a WebSocket client.
    //    - _network_client_for_mqtt would point to this WebSocket client.
    //
    // If MQTT_BROKER_WEBSOCKET_URI is blank, and a secure TCP connection is desired (e.g., port 8883):
    // 1. Initialize the TLS client with certs.
    //    - _network_client_for_mqtt would point to this TLS client.
    //
    // Otherwise (defaulting to non-secure TCP):
    //    _network_client_for_mqtt = &_ethernet_client; // Already done by default
    CogCore::Debug<const char*>("_setupSecureClient: Placeholder. Using plain EthernetClient for now. Secure connection and WebSockets require further implementation.\n");
    _network_client_for_mqtt = &_ethernet_client; // Default to plain Ethernet

    // The following is the original placeholder content, kept for reference during development if needed.
    // CogCore::Debug<const char*>("Attempting to set up secure client (TLS/Certificates)...\n");
    // This is where platform-specific TLS setup would occur.
    // For Arduino Due with W5x00 Ethernet, this typically involves:
    // 1. A TLS library like BearSSL.
    // 2. A wrapper client (e.g., BearSSLClient) that uses EthernetClient for network I/O
    //    and BearSSL for TLS processing.
    // 3. Loading CA certificate, client certificate, and client private key into the TLS client.

    // Example placeholder steps (actual implementation depends heavily on chosen TLS library):
    // BearSSLClient* _tls_client = new BearSSLClient(&_ethernet_client); // Hypothetical
    // _tls_client->setCACert(ca_cert_pem);
    // _tls_client->setCertificate(client_cert_pem);
    // _tls_client->setPrivateKey(client_key_pem);
    // _network_client_for_mqtt = _tls_client; // MQTTClient will use this TLS client

    // For WebSockets (MQTTS over WSS):
    // The _tls_client would also need to be configured for WebSockets.
    // e.g., _tls_client->setWebSocketPath("/mqtt"); // Or similar, if supported
    // Or _mqttClient.setUseWebSocket(true); // If arduino-mqtt has such an option

    CogCore::Debug<const char*>("Secure client setup (TLS/Certs) is currently a PLACEHOLDER.\n");
    CogCore::Debug<const char*>("Plain Ethernet will be used if MQTT_BROKER_PORT is 1883.\n");
    CogCore::Debug<const char*>("For TLS, ensure MQTT_BROKER_PORT is the secure port (e.g., 8883) and _setupSecureClient is implemented.\n");

    // If MQTT_BROKER_PORT implies TLS (e.g., 8883), but TLS is not implemented, connection will fail.
    // For now, _network_client_for_mqtt remains &_ethernet_client.
    // If you have a BearSSLClient or similar, you would assign it to _network_client_for_mqtt here.
    // CogCore::Debug<const char*>("Secure client setup (TLS/Certs) is currently a PLACEHOLDER.\n");
    // CogCore::Debug<const char*>("Plain Ethernet will be used if MQTT_BROKER_PORT is 1883.\n");
    // CogCore::Debug<const char*>("For TLS, ensure MQTT_BROKER_PORT is the secure port (e.g., 8883) and _setupSecureClient is implemented.\n");
}


bool NetworkMQTT::connect() {
    if (_mqttClient.connected()) {
        return true;
    }

    _lastReconnectAttemptMillis = millis(); // Update attempt time

    // Call placeholder for TLS setup. In a real scenario, this would configure _network_client_for_mqtt.
    _setupSecureClient(); 

    // Determine host, port, and client based on configuration (WebSocket, MQTTS, or plain MQTT)
    // This logic would ideally be in _setupSecureClient() which prepares _network_client_for_mqtt
    // and potentially extracts host/port if not using IP/Port directly.
    const char* targetHost = MQTT_BROKER_IP; // Default
    uint16_t targetPort = MQTT_BROKER_PORT;  // Default

    // Basic logic sketch (would be more robust in full implementation):
    if (strlen(MQTT_BROKER_WEBSOCKET_URI) > 0) {
        // TODO: Parse MQTT_BROKER_WEBSOCKET_URI to get host, port, path, and scheme (ws/wss)
        // For now, this part is conceptual. Assume _setupSecureClient prepares the _network_client_for_mqtt
        // and _mqttClient.begin() would use a host/port derived from the URI or specific settings.
        // Example: if using WSS, targetPort would be extracted WSS port or default 443.
        // The Client passed to _mqttClient.begin() must be a WebSocket client (secure or not).
        // arduino-mqtt's begin() might need a path for WebSockets, or the WebSocket client handles it.
        // e.g. _mqttClient.setPath("/ws"); or similar if the library supports it directly.
        // This often means the _network_client_for_mqtt itself is a WebSocketClient instance.
        CogCore::Debug<const char*>("Connecting (WebSocket URI configured - conceptual, host/port may need parsing from URI)...\n");
        // targetHost and targetPort would be parsed from MQTT_BROKER_WEBSOCKET_URI
        // _mqttClient.setPath("/mqtt"); // Example if arduino-mqtt client has a path setter for websockets
    } else if (targetPort == MQTT_BROKER_SECURE_PORT && targetPort != MQTT_BROKER_PORT) { 
        // Heuristic: if MQTT_BROKER_PORT is set to the secure port, assume MQTTS.
        // MQTT_BROKER_SECURE_PORT is defined, so use it if current port matches it
        // (and it's not the same as the non-secure port, implying deliberate secure choice).
        CogCore::Debug<const char*>("Connecting (Secure TCP MQTTS)...\n");
    } else {
        CogCore::Debug<const char*>("Connecting (Plain TCP MQTT)...\n");
    }
    
    // Set MQTT broker, port. The client used here (_network_client_for_mqtt) must be ready.
    _mqttClient.begin(targetHost, targetPort, *_network_client_for_mqtt);
    
    // Set Last Will and Testament (LWT)
    String lwtTopicStr = MQTT_DEVICE_BASE_TOPIC;
    lwtTopicStr += _macAddressStr;
    lwtTopicStr += LWT_TOPIC_SUFFIX;
    _mqttClient.setWill(lwtTopicStr.c_str(), LWT_MESSAGE_OFFLINE, LWT_RETAIN, LWT_QOS);

    CogCore::Debug<const char*>("Attempting MQTT connection (arduino-mqtt). ClientID: ");
    CogCore::Debug<const char*>(_clientIdStr.c_str()); // Use member _clientIdStr
    CogCore::Debug<const char*>("...
");

    bool success;
    if (strlen(MQTT_USERNAME) > 0) {
        CogCore::Debug<const char*>("Connecting with MQTT credentials. User: ");
        CogCore::Debug<const char*>(MQTT_USERNAME);
        CogCore::Debug<const char*>("...
");
        success = _mqttClient.connect(_clientIdStr.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
    } else {
        CogCore::Debug<const char*>("Connecting without MQTT credentials.
");
        success = _mqttClient.connect(_clientIdStr.c_str());
    }

    if (success) {
        CogCore::Debug<const char*>("MQTT connected (arduino-mqtt).
");
        // Publish "online" status to LWT topic
        publish(lwtTopicStr.c_str(), LWT_MESSAGE_ONLINE, LWT_RETAIN, LWT_QOS);
        // Resubscribe logic might be needed here if subscriptions are used
    } else {
        CogCore::Debug<const char*>("MQTT connection failed (arduino-mqtt), error code: ");
        CogCore::Debug<int>(_mqttClient.lastError()); // arduino-mqtt uses lastError()
        CogCore::Debug<const char*>("
");
    }
    return _mqttClient.connected();
}

void NetworkMQTT::_reconnect() {
    if (millis() - _lastReconnectAttemptMillis > _reconnectIntervalMillis) {
        CogCore::Debug<const char*>("Attempting MQTT reconnection (arduino-mqtt, non-blocking)...
");
        // No need to call _setupSecureClient() again if client object is persistent and already configured.
        // connect() will handle broker details and actual connection.
        connect(); // connect() updates _lastReconnectAttemptMillis and handles all logic
    }
}

bool NetworkMQTT::publish(const char* topic, const char* payload, bool retained, uint8_t qos) {
    if (!isConnected()) {
        // _reconnect(); // Don't call directly, loop() will handle it to maintain non-blocking
        CogCore::Debug<const char*>("MQTT publish failed: Not connected. Will attempt reconnect in loop.
");
        return false;
    }
    // arduino-mqtt publish API: topic, message, retained, qos
    if (_mqttClient.publish(topic, payload, retained, qos)) {
        CogCore::Debug<const char*>("MQTT message published (arduino-mqtt). Topic: ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("MQTT message publish failed (arduino-mqtt). Topic: ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>(" Error: ");
        CogCore::Debug<int>(_mqttClient.lastError());
        CogCore::Debug<const char*>("
");
        return false;
    }
}

bool NetworkMQTT::subscribe(const char* topic, uint8_t qos) {
    if (!isConnected()) {
        // _reconnect(); // Let loop() handle reconnection
        CogCore::Debug<const char*>("MQTT subscribe failed: Not connected. Will attempt reconnect in loop.
");
        return false;
    }
    // arduino-mqtt subscribe API: topic, qos
    if (_mqttClient.subscribe(topic, qos)) {
        CogCore::Debug<const char*>("Subscribed to MQTT topic (arduino-mqtt): ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("Failed to subscribe to MQTT topic (arduino-mqtt): ");
        CogCore::Debug<const char*>(topic);
        CogCore::Debug<const char*>(" Error: ");
        CogCore::Debug<int>(_mqttClient.lastError());
        CogCore::Debug<const char*>("
");
        return false;
    }
}

void NetworkMQTT::loop() {
    if (!_mqttClient.connected()) {
        _reconnect(); // Attempt non-blocking reconnection
    }
    _mqttClient.loop(); // Essential for arduino-mqtt to process messages and keepalives
}

bool NetworkMQTT::isConnected() {
    return _mqttClient.connected();
}
