// IMPORTANT NOTE ON RESOURCE USAGE (FLASH & SRAM) FOR MQTTS:
//
// Enabling MQTTS (TLS over TCP) involves using cryptographic libraries (like BearSSL,
// which is utilized by EthernetWebServer_SSL) and storing certificates in memory
// (currently PROGMEM/Flash for certificates defined in certificates.h).
// These functionalities can be resource-intensive, consuming both Flash memory
// (for the library code and stored certificates) and SRAM (for TLS buffers,
// session state, and stack usage during cryptographic operations).
//
// Arduino Due (SAM3X8E) Specifics:
// The Arduino Due, with its ARM Cortex-M3 processor, has significantly more Flash
// (512 KB) and SRAM (96 KB) than traditional AVR-based Arduinos (like Uno/Mega).
// This makes it more capable of handling TLS. However, resources are not unlimited.
//
// Why it's Important to Check:
// 1.  Flash Overflow: If the compiled sketch size (including libraries and
//     certificates) exceeds the available Flash memory, the upload will fail.
// 2.  SRAM Exhaustion: TLS operations, especially the handshake, can require
//     several kilobytes of SRAM for buffers and state. If SRAM is exhausted at
//     runtime, the device will likely crash or behave erratically (e.g., random
//     resets, failed connections). This can be hard to debug.
//     The size of the read/write buffers for the MQTT client (e.g., MQTTClient(1024, 256))
//     also contributes to SRAM usage.
//
// How to Check Resource Usage:
// -   PlatformIO: After a successful build (e.g., `pio run`), PlatformIO provides
//     a summary of Flash and SRAM usage (e.g., "RAM: [==        ]  15.0% (used 12288 bytes from 81920 bytes)"
//     and "Flash: [===       ]  30.0% (used 157286 bytes from 524288 bytes)").
//     Running `pio run -v` (verbose build) can sometimes show more details about
//     memory sections.
// -   Arduino IDE: After compiling/uploading, the Arduino IDE's output console
//     typically shows the sketch size (Flash) and global variable memory usage (SRAM).
//     It might state something like: "Sketch uses XXXXX bytes (Y%) of program storage space. Maximum is ZZZZZ bytes."
//     and "Global variables use AAAA bytes (B%) of dynamic memory, leaving CCCC bytes for local variables. Maximum is DDDD bytes."
//
// Recommendations:
// -   Always check the resource usage reported by your build system after enabling MQTTS
//     and adding your actual certificates.
// -   Pay close attention to SRAM usage. Leave a healthy margin for runtime stack
//     and heap allocations. For TLS, a margin of 20-30% free SRAM might be a
//     safe starting point, but this depends on other application needs.
// -   If you encounter stability issues, especially crashes during or after TLS
//     connection attempts, SRAM exhaustion is a common culprit.
// -   Monitor resource usage as you add more features to your application, as
//     they will also consume Flash and SRAM.
// -   Certificate sizes also contribute to Flash usage. Ensure your certificates
//     (especially CA chains) are as minimal as necessary.
//
// This `NetworkMQTT` class is configured with MQTTClient(1024, 256) buffers,
// which themselves will use over 1.25KB of SRAM.

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
#include <EthernetClient.h>
// Assuming EthernetSSLClient.h and EthernetWebSocketClient.h also include necessary base classes
// and potentially SSLClientParameters.h or similar helpers for certificate handling.
// SSLClientParameters.h is typically part of the EthernetWebServer_SSL library structure.

// Initialize static members
NetworkMQTT* NetworkMQTT::_instance = nullptr;

// Helper for URI parsing (basic)
struct ParsedUri {
    String scheme;
    String host;
    uint16_t port;
    String path;
    bool valid = false;
};

void parseWebSocketUri(const String& uri_str, ParsedUri& result) {
    result.valid = false;
    if (uri_str.isEmpty()) return;

    int scheme_end = uri_str.indexOf("://");
    if (scheme_end == -1) {
        CogCore::Debug<const char*>("parseUri: Invalid URI, missing '://'.\n");
        return;
    }
    result.scheme = uri_str.substring(0, scheme_end);
    if (result.scheme != "ws" && result.scheme != "wss") {
        CogCore::Debug<const char*>("parseUri: Invalid scheme, must be 'ws' or 'wss'. Scheme: ");
        // CogCore::Debug<const char*>(result.scheme.c_str()); // Requires String.c_str()
        CogCore::Debug<const char*>("
");
        return;
    }

    String remaining = uri_str.substring(scheme_end + 3);
    int path_start = remaining.indexOf('/');
    if (path_start == -1) {
        result.path = "/";
    } else {
        result.path = remaining.substring(path_start);
    }

    String host_port_str = (path_start == -1) ? remaining : remaining.substring(0, path_start);
    int port_colon = host_port_str.indexOf(':');
    if (port_colon != -1) {
        result.host = host_port_str.substring(0, port_colon);
        String port_str = host_port_str.substring(port_colon + 1);
        if (port_str.length() > 0) result.port = port_str.toInt();
    } else {
        result.host = host_port_str;
    }
    if (result.port == 0) { // Set default port if not parsed or zero
        result.port = (result.scheme == "wss") ? 443 : 80;
    }

    if (result.host.isEmpty() || result.port == 0) {
        CogCore::Debug<const char*>("parseUri: Failed to parse host or port.\n");
        return;
    }
    result.valid = true;
}


NetworkMQTT::NetworkMQTT(const char* macAddress)
    : _macAddressStr(macAddress),
      _ssl_client(nullptr),
      _ws_client(nullptr),
      _mqttClient(1024, 256) { // Read:1024, Write:256
    _instance = this;
    _clientIdStr = MQTT_CLIENT_ID_PREFIX;
    _clientIdStr += _macAddressStr;
    _network_client_for_mqtt = &_ethernet_client;
    _mqttClient.onMessage(NetworkMQTT::_internalMessageReceived);
    CogCore::Debug<const char*>("NetworkMQTT: Instance created. Certificate setup occurs at connect time.\n");
}

NetworkMQTT::~NetworkMQTT() {
    if (_mqttClient.connected()) {
        _mqttClient.disconnect();
    }
    delete _ws_client; _ws_client = nullptr;
    delete _ssl_client; _ssl_client = nullptr;
}

void NetworkMQTT::_internalMessageReceived(String &topic, String &payload) {
    if (_instance && _instance->_onMessageCallback) {
        _instance->_onMessageCallback(topic, payload);
    }
}

void NetworkMQTT::onMessage(std::function<void(String &topic, String &payload)> callback) {
    _onMessageCallback = callback;
}

void NetworkMQTT::_setupSecureClient(const ParsedUri* ws_uri_details) {
    delete _ws_client; _ws_client = nullptr; // Ensure clean state
    delete _ssl_client; _ssl_client = nullptr;
    _network_client_for_mqtt = &_ethernet_client; // Default to plain

    bool needs_tls = false;
    bool use_websocket = (ws_uri_details && ws_uri_details->valid);

    if (use_websocket) {
        if (ws_uri_details->scheme == "wss") {
            needs_tls = true;
            CogCore::Debug<const char*>("WSS: TLS will be used for WebSocket.\n");
        } else { // "ws"
            CogCore::Debug<const char*>("WS: Plain TCP will be used for WebSocket (not secure).\n");
        }
    } else { // Not using WebSockets, check for MQTTS (secure TCP)
        uint16_t configured_tcp_port = (uint16_t)atoi(MQTT_BROKER_PORT); // current port for direct TCP
        uint16_t secure_mqtt_tcp_port = (uint16_t)atoi(MQTT_BROKER_SECURE_PORT);
        // If user set MQTT_BROKER_PORT to the same as MQTT_BROKER_SECURE_PORT, assume MQTTS
        if (configured_tcp_port == secure_mqtt_tcp_port && secure_mqtt_tcp_port != 0) {
            needs_tls = true;
            CogCore::Debug<const char*>("MQTTS: TLS will be used for TCP connection on port ");
            CogCore::Debug<uint16_t>(secure_mqtt_tcp_port); CogCore::Debug<const char*>(".\n");
        }
    }

    Client* base_for_websocket = &_ethernet_client; // Client that WebSocket will wrap

    if (needs_tls) {
        CogCore::Debug<const char*>("Setting up TLS layer (EthernetSSLClient)...\n");
        _ssl_client = new EthernetSSLClient(_ethernet_client); // Instantiate with base EthernetClient

        if (strlen(ca_cert_pem) > 100) { // Basic check for actual cert vs placeholder
            // Set CA certificate for server verification
            // Note: setCACert might return bool or void depending on library version.
            // Add error checking if possible/needed.
            _ssl_client->setCACert(ca_cert_pem);
            CogCore::Debug<const char*>("TLS: CA certificate set.\n");
        } else {
            CogCore::Debug<const char*>("TLS Warning: CA certificate (ca_cert_pem) is placeholder or empty. Server authentication may fail or be insecure.\n");
            // For development, you might allow insecure connections:
            // _ssl_client->setInsecure(); // This skips CA validation - NOT FOR PRODUCTION!
        }

        if (strlen(client_cert_pem) > 100 && strlen(client_key_pem) > 100) { // Basic check
            CogCore::Debug<const char*>("Configuring mTLS with client certificate and key...\n");
            SSLClientParameters mTLSParams = SSLClientParameters::fromPEM(client_cert_pem, strlen(client_cert_pem), client_key_pem, strlen(client_key_pem));
            // It's good to check if fromPEM succeeded, e.g. if mTLSParams.isValid() or similar exists
            if (!_ssl_client->setMutualAuthParams(mTLSParams)) {
                 CogCore::Debug<const char*>("TLS ERROR: Failed to set mTLS parameters on EthernetSSLClient.\n");
                 // This could be a fatal error for the connection if mTLS is required.
                 delete _ssl_client; _ssl_client = nullptr;
                 _network_client_for_mqtt = &_ethernet_client; // Fallback or error state
                 return; // Stop further setup
            } else {
                 CogCore::Debug<const char*>("TLS: mTLS parameters set successfully.\n");
            }
        } else {
            CogCore::Debug<const char*>("TLS: Client certificate/key not provided or placeholders. Skipping mTLS configuration.\n");
        }
        _network_client_for_mqtt = _ssl_client;
        base_for_websocket = _ssl_client; // If WSS, WebSocket will wrap this SSL client
    }

    if (use_websocket) {
        CogCore::Debug<const char*>("Setting up WebSocket layer (conceptual)...\n");
        // _ws_client = new EthernetWebSocketClient(*base_for_websocket); // Instantiate with the underlying client (plain or SSL)

        // Placeholder for actual WebSocket handshake:
        // The following steps are conceptual and need specific API calls from EthernetWebSocketClient:
        // 1. Connect the underlying client (base_for_websocket) to the host/port from ws_uri_details.
        //    This is usually handled by _mqttClient.connect() which calls _network_client_for_mqtt->connect().
        //    If _ws_client is _network_client_for_mqtt, then _ws_client->connect() must first ensure
        //    base_for_websocket is connected, then do WS handshake.
        //
        // 2. Perform WebSocket handshake:
        //    bool handshake_ok = _ws_client->handshake(ws_uri_details->host, ws_uri_details->port, ws_uri_details->path);
        //    OR _ws_client->begin(ws_uri_details->host, ws_uri_details->port, ws_uri_details->path, "mqtt"); // Some libs combine
        //
        // if (handshake_ok) {
        //    _network_client_for_mqtt = _ws_client; // Final client for MQTT
        //    CogCore::Debug<const char*>("WebSocket handshake successful.\n");
        // } else {
        //    CogCore::Debug<const char*>("WebSocket handshake FAILED.\n");
        //    delete _ws_client; _ws_client = nullptr;
        //    if (needs_tls && _ssl_client) _ssl_client->stop(); // Stop/cleanup underlying SSL if WS failed over it
        //    // Revert _network_client_for_mqtt or handle error
        //    _network_client_for_mqtt = (needs_tls) ? (Client*)_ssl_client : (Client*)&_ethernet_client; // Fallback
        // }
        CogCore::Debug<const char*>("WebSocket client setup and handshake is a PLACEHOLDER and not functionally implemented.\n");
        CogCore::Debug<const char*>("MQTT over WS/WSS will likely fail until this is implemented using specific WebSocket library APIs.\n");
        // For now, _network_client_for_mqtt remains what it was set to in TLS section or plain ethernet.
        // To actually use WebSockets, the line below would be uncommented after ws_client is fully set up and handshaked:
        // _network_client_for_mqtt = _ws_client;
    }

    // Final client assignment report
    if (_network_client_for_mqtt == _ws_client && use_websocket) {
         CogCore::Debug<const char*>("Network client intended for WebSocket (WS/WSS) - (Handshake PLACEHOLDER).\n");
    } else if (_network_client_for_mqtt == _ssl_client && needs_tls) {
        CogCore::Debug<const char*>("Network client configured for MQTTS (TLS over TCP).\n");
    } else {
        CogCore::Debug<const char*>("Network client configured for plain MQTT (TCP).\n");
    }
}

bool NetworkMQTT::connect() {
    if (_mqttClient.connected()) {
        return true;
    }
    _lastReconnectAttemptMillis = millis();

    String target_host_str = MQTT_BROKER_IP;
    uint16_t target_port_val = (uint16_t)atoi(MQTT_BROKER_PORT);

    ParsedUri uri_details;
    String ws_uri_str = MQTT_BROKER_WEBSOCKET_URI;
    bool is_websocket_flow = false;

    if (!ws_uri_str.isEmpty()) {
        parseWebSocketUri(ws_uri_str, uri_details);
        if (uri_details.valid) {
            is_websocket_flow = true;
            target_host_str = uri_details.host;
            target_port_val = uri_details.port;
            CogCore::Debug<const char*>("Config: WebSocket URI. Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
            CogCore::Debug<const char*>(", Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
        } else {
            CogCore::Debug<const char*>("Config: Invalid WebSocket URI, falling back to TCP IP/Port.\n");
        }
    }

    _setupSecureClient(is_websocket_flow ? &uri_details : nullptr);

    if (!is_websocket_flow && _network_client_for_mqtt == _ssl_client && _ssl_client != nullptr) {
        target_port_val = (uint16_t)atoi(MQTT_BROKER_SECURE_PORT); // Use secure port for MQTTS
        CogCore::Debug<const char*>("Config: MQTTS mode. Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
        CogCore::Debug<const char*>(", Secure Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
    } else if (!is_websocket_flow) {
         CogCore::Debug<const char*>("Config: Plain MQTT TCP mode. Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
         CogCore::Debug<const char*>(", Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
    }
    // If is_websocket_flow, host/port are already from URI. _network_client_for_mqtt is (conceptually) _ws_client.

    CogCore::Debug<const char*>("MQTTClient.begin with Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
    CogCore::Debug<const char*>(", Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
    _mqttClient.begin(target_host_str.c_str(), target_port_val, *_network_client_for_mqtt);

    String lwtTopicStr = MQTT_DEVICE_BASE_TOPIC;
    lwtTopicStr += _macAddressStr;
    lwtTopicStr += LWT_TOPIC_SUFFIX;
    _mqttClient.setWill(lwtTopicStr.c_str(), LWT_MESSAGE_OFFLINE, LWT_RETAIN, LWT_QOS);

    CogCore::Debug<const char*>("Attempting MQTT connect (_mqttClient.connect). ClientID: ");
    CogCore::Debug<const char*>(_clientIdStr.c_str()); CogCore::Debug<const char*>("
");

    bool success;
    if (strlen(MQTT_USERNAME) > 0) {
        success = _mqttClient.connect(_clientIdStr.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
    } else {
        success = _mqttClient.connect(_clientIdStr.c_str());
    }

    if (success) {
        CogCore::Debug<const char*>("MQTT connected (arduino-mqtt).\n");
        publish(lwtTopicStr.c_str(), LWT_MESSAGE_ONLINE, LWT_RETAIN, LWT_QOS);
    } else {
        CogCore::Debug<const char*>("MQTT connection failed (arduino-mqtt), error code: ");
        CogCore::Debug<int>(_mqttClient.lastError()); CogCore::Debug<const char*>("
");
    }
    return _mqttClient.connected();
}

void NetworkMQTT::_reconnect() {
    if (millis() - _lastReconnectAttemptMillis > _reconnectIntervalMillis) {
        CogCore::Debug<const char*>("Attempting MQTT reconnection (non-blocking)...\n");
        connect();
    }
}

bool NetworkMQTT::publish(const char* topic, const char* payload, bool retained, uint8_t qos) {
    if (!isConnected()) {
        CogCore::Debug<const char*>("MQTT publish failed: Not connected. Will attempt reconnect in loop.\n");
        return false;
    }
    if (_mqttClient.publish(topic, payload, retained, qos)) {
        CogCore::Debug<const char*>("MQTT message published (arduino-mqtt). Topic: ");
        CogCore::Debug<const char*>(topic); CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("MQTT message publish failed (arduino-mqtt). Topic: ");
        CogCore::Debug<const char*>(topic); CogCore::Debug<const char*>(" Error: ");
        CogCore::Debug<int>(_mqttClient.lastError()); CogCore::Debug<const char*>("
");
        return false;
    }
}

bool NetworkMQTT::subscribe(const char* topic, uint8_t qos) {
    if (!isConnected()) {
        CogCore::Debug<const char*>("MQTT subscribe failed: Not connected. Will attempt reconnect in loop.\n");
        return false;
    }
    if (_mqttClient.subscribe(topic, qos)) {
        CogCore::Debug<const char*>("Subscribed to MQTT topic (arduino-mqtt): ");
        CogCore::Debug<const char*>(topic); CogCore::Debug<const char*>("
");
        return true;
    } else {
        CogCore::Debug<const char*>("Failed to subscribe to MQTT topic (arduino-mqtt): ");
        CogCore::Debug<const char*>(topic); CogCore::Debug<const char*>(" Error: ");
        CogCore::Debug<int>(_mqttClient.lastError()); CogCore::Debug<const char*>("
");
        return false;
    }
}

void NetworkMQTT::loop() {
    if (!_mqttClient.connected()) {
        _reconnect();
    }
    _mqttClient.loop();
}

bool NetworkMQTT::isConnected() {
    return _mqttClient.connected();
}
