#ifndef NETWORK_MQTT_H
#define NETWORK_MQTT_H

#include <MQTT.h> // From arduino-mqtt
#include "mqtt_config.h"
#include "certificates.h" // For CA, client cert, client key
#include <Client.h>       // Base Client interface for network operations
#include <Ethernet.h>     // For EthernetClient, assuming W5x00 hardware

// Forward declaration for a potential TLS client wrapper (e.g., BearSSLClient)
// class BearSSLClient; // This would typically wrap _ethernet_client

class NetworkMQTT {
public:
    // Constructor now takes only macAddress, network clients are managed internally
    NetworkMQTT(const char* macAddress);
    ~NetworkMQTT(); // Destructor to clean up client objects if necessary

    bool connect(); // Attempts to establish a secure MQTT connection
    // Publish method now includes QoS parameter as arduino-mqtt supports it directly
    bool publish(const char* topic, const char* payload, bool retained = false, uint8_t qos = 0);
    bool subscribe(const char* topic, uint8_t qos = 0);
    void loop(); // Handles keepalives and processes incoming messages
    bool isConnected();
    
    // Callback for incoming messages, signature adapted for arduino-mqtt
    // The library typically uses `void messageReceived(String &topic, String &payload)`
    // or a lambda: `_mqttClient.onMessage([this](String &topic, String &payload) { ... });`
    // For a generic callback setter, we might need to wrap it or use std::function if compatible.
    // For now, let's assume direct use of onMessage in .cpp or a simplified callback.
    // void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback); // Old PubSubClient way
    void onMessage(std::function<void(String &topic, String &payload)> callback);


private:
    EthernetClient _ethernet_client; // Base Ethernet client for W5x00 hardware

    // For TLS, _ethernet_client would be wrapped by a TLS-capable client.
    // Example: BearSSLClient _tls_client;
    // And then _mqttClient would use _tls_client.
    // For this step, we'll assume _mqttClient can be configured for TLS or we'll
    // manage the secure client setup in _setupSecureClient().
    // If arduino-mqtt's MQTTClient itself can take a Client& and be configured for TLS (e.g. with BearSSL certs),
    // then we pass the wrapped TLS client to it.
    // For now, _mqttClient will use _ethernet_client directly, and TLS setup will be a placeholder.
    // A more robust solution would use a specific TLSClient type here.
    Client* _network_client_for_mqtt; // Pointer to the client MQTTClient will use (could be _ethernet_client or a _tls_client)


    MQTTClient _mqttClient; // MQTTClient from arduino-mqtt library

    String _macAddressStr; // Stored MAC address
    String _clientIdStr;   // Constructed client ID
    
    // LWT parameters are now set directly using _mqttClient.setWill()
    // String _lwtTopicStr; // No longer needed as member, constructed locally
    // const char* _lwtMessageOffline = LWT_MESSAGE_OFFLINE; // From mqtt_config.h
    // const uint8_t _lwtQos = LWT_QOS; // From mqtt_config.h
    // const bool _lwtRetain = LWT_RETAIN; // From mqtt_config.h
    
    // For non-blocking reconnection attempts
    unsigned long _lastReconnectAttemptMillis = 0;
    // Using define from mqtt_config.h if available, or keep local
    const unsigned long _reconnectIntervalMillis = 5000; // Default, can be configured

    void _reconnect();
    void _setupSecureClient(); // Placeholder for TLS client configuration
    
    // Callback from MQTTClient
    std::function<void(String &topic, String &payload)> _onMessageCallback;
    static void _internalMessageReceived(String &topic, String &payload); // Static wrapper
    static NetworkMQTT* _instance; // Static instance pointer for the callback
};

#endif // NETWORK_MQTT_H
