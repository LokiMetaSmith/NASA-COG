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
    if (path_start == -1) { // Path is required for WebSockets. Default to "/" if not like "host.com:port"
        result.path = "/"; 
        // If path_start is -1, remaining is host:port or host
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
        // Default port if not specified
    }
    // Set default port if not parsed or zero
    if (result.port == 0) {
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
    CogCore::Debug<const char*>("NetworkMQTT: Instance created.\n");
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
    delete _ws_client; _ws_client = nullptr;
    delete _ssl_client; _ssl_client = nullptr;
    _network_client_for_mqtt = &_ethernet_client; // Default

    bool needs_tls = false;
    bool use_websocket = (ws_uri_details && ws_uri_details->valid);

    if (use_websocket) {
        if (ws_uri_details->scheme == "wss") {
            needs_tls = true;
            CogCore::Debug<const char*>("WSS: TLS will be used for WebSocket.\n");
        } else {
            CogCore::Debug<const char*>("WS: Plain TCP will be used for WebSocket.\n");
        }
    } else { // Not using WebSockets, check for MQTTS (secure TCP)
        uint16_t configured_tcp_port = (uint16_t)atoi(MQTT_BROKER_PORT);
        uint16_t secure_mqtt_tcp_port = (uint16_t)atoi(MQTT_BROKER_SECURE_PORT);
        if (configured_tcp_port == secure_mqtt_tcp_port && secure_mqtt_tcp_port != 0) {
            needs_tls = true;
            CogCore::Debug<const char*>("MQTTS: TLS will be used for TCP connection.\n");
        }
    }

    Client* base_client_for_ws_or_final_client = &_ethernet_client;

    if (needs_tls) {
        CogCore::Debug<const char*>("Setting up TLS layer...\n");
        if (strlen(ca_cert_pem) < 100) { // Basic check
            CogCore::Debug<const char*>("TLS Error: CA certificate missing/placeholder. Server auth may fail.\n");
            // Potentially allow insecure if a flag is set - NOT RECOMMENDED. For now, proceed.
        }
        _ssl_client = new EthernetSSLClient(_ethernet_client, ca_cert_pem, strlen(ca_cert_pem));
        // Example: _ssl_client->setInsecure(); // To skip server validation (NOT FOR PRODUCTION)

        if (strlen(client_cert_pem) > 100 && strlen(client_key_pem) > 100) { // Basic check
            CogCore::Debug<const char*>("Configuring mTLS with client certificate and key...\n");
            SSLClientParameters mTLSParams = SSLClientParameters::fromPEM(client_cert_pem, strlen(client_cert_pem), client_key_pem, strlen(client_key_pem));
            if (!_ssl_client->setMutualAuthParams(mTLSParams)) {
                 CogCore::Debug<const char*>("TLS Warning: Failed to set mTLS parameters.\n");
            } else {
                 CogCore::Debug<const char*>("TLS: mTLS parameters set.\n");
            }
        } else {
            CogCore::Debug<const char*>("TLS: Client certificate/key not provided or placeholders. Skipping mTLS.\n");
        }
        _network_client_for_mqtt = _ssl_client;
        base_client_for_ws_or_final_client = _ssl_client; // WebSocket would wrap this
    }

    if (use_websocket) {
        CogCore::Debug<const char*>("Setting up WebSocket layer...\n");
        // _ws_client = new EthernetWebSocketClient(*base_client_for_ws_or_final_client);
        // Placeholder for actual WebSocket handshake:
        // bool handshake_success = _ws_client->handshake(ws_uri_details->host, ws_uri_details->path);
        // if (handshake_success) {
        //    _network_client_for_mqtt = _ws_client;
        //    CogCore::Debug<const char*>("WebSocket handshake successful.\n");
        // } else {
        //    CogCore::Debug<const char*>("WebSocket handshake FAILED.\n");
        //    delete _ws_client; _ws_client = nullptr;
        //    if (needs_tls && _ssl_client) _ssl_client->stop(); else _ethernet_client.stop();
        //    // Fallback to what _network_client_for_mqtt was before attempting WS or just fail by not changing it
        //    // For robustness, ensure _network_client_for_mqtt is valid or connect() fails.
        // }
        CogCore::Debug<const char*>("WebSocket client handshake is a PLACEHOLDER. Full WS/WSS MQTT will likely fail or use plain TCP/TLS if WS handshake isn't integrated.\n");
        // If handshake fails, _network_client_for_mqtt should ideally be reset or connection should abort.
        // For now, if ws_uri is set, we expect _network_client_for_mqtt to be a WS client.
        // This part requires actual WebSocketClient API usage.
    }
    
    // Final client assignment report
    if (_network_client_for_mqtt == _ws_client && use_websocket) {
         CogCore::Debug<const char*>("Network client configured for WebSocket (WS/WSS) - (Placeholder Handshake).\n");
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

    String target_host_str = MQTT_BROKER_IP; // Default
    uint16_t target_port_val = (uint16_t)atoi(MQTT_BROKER_PORT);  // Default
    
    ParsedUri uri_details;
    String ws_uri_str = MQTT_BROKER_WEBSOCKET_URI;
    bool is_websocket_configured = false;

    if (!ws_uri_str.isEmpty()) {
        parseWebSocketUri(ws_uri_str, uri_details);
        if (uri_details.valid) {
            is_websocket_configured = true;
            target_host_str = uri_details.host;
            target_port_val = uri_details.port;
            CogCore::Debug<const char*>("WebSocket URI will be used. Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
            CogCore::Debug<const char*>(" Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
        } else {
            CogCore::Debug<const char*>("Invalid WebSocket URI configured, falling back to TCP IP/Port.\n");
        }
    }
    
    _setupSecureClient(is_websocket_configured ? &uri_details : nullptr);

    // Re-check port if not WebSocket, as _setupSecureClient might have decided on MQTTS
    if (!is_websocket_configured && _network_client_for_mqtt == _ssl_client && _ssl_client != nullptr) {
        target_port_val = (uint16_t)atoi(MQTT_BROKER_SECURE_PORT);
        CogCore::Debug<const char*>("MQTTS mode: Using secure port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
");
    }

    // For arduino-mqtt (256dpi/MQTT):
    // If using WebSockets, the `Client` instance passed to `_mqttClient.begin()` must already be
    // a fully connected and handshaked WebSocket stream. `arduino-mqtt` itself does not
    // perform the WebSocket handshake or handle HTTP upgrade/paths.
    // The `_network_client_for_mqtt` (which would be `_ws_client` if WS is functional)
    // must therefore manage the WebSocket connection state and path internally.
    // The `target_host_str` and `target_port_val` are for the MQTT broker, which the
    // WebSocket tunnel established by `_ws_client` would connect to.
    // If `_ws_client` needs the path (e.g. "/mqtt") for its handshake, that's part of its setup.
    CogCore::Debug<const char*>("MQTTClient.begin with Host: "); CogCore::Debug<const char*>(target_host_str.c_str());
    CogCore::Debug<const char*>(" Port: "); CogCore::Debug<int>(target_port_val); CogCore::Debug<const char*>("
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
