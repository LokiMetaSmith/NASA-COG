#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// Note: Client certificate authentication is handled by embedding certificates
// in firmware/include/certificates.h and configuring the TLS client.
// No specific flags here are needed if always using embedded certs when TLS is active.

// MQTT Broker Details
// IMPORTANT: REPLACE "192.168.1.100" WITH YOUR ACTUAL MQTT BROKER IP ADDRESS OR HOSTNAME
#define MQTT_BROKER_IP "192.168.1.100" // Replace with your MQTT Broker's IP or hostname

// Standard MQTT Non-Secure port (MQTT over TCP, typically 1883)
#define MQTT_BROKER_PORT 1883

// MQTT Broker WebSocket URI (e.g., "ws://broker.example.com:80/mqtt" or "wss://broker.example.com:443/mqtt")
// Leave blank if not using WebSockets. If filled, this may take precedence over MQTT_BROKER_IP and MQTT_BROKER_PORT for connection.
#define MQTT_BROKER_WEBSOCKET_URI "" 

// Standard MQTT Secure port (MQTTS over TCP, typically 8883)
// Used if MQTT_BROKER_WEBSOCKET_URI is blank and a secure TCP connection is desired.
#define MQTT_BROKER_SECURE_PORT 8883 


// MQTT Credentials (optional, leave blank if not used)
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

// MQTT Client ID prefix (MAC address will be appended)
#define MQTT_CLIENT_ID_PREFIX "OEDCS-" 


// LWT (Last Will and Testament) Configuration
#define LWT_TOPIC_SUFFIX "/status" // MAC address will be prepended, e.g., devices/XX:XX:XX:XX:XX:XX/status
#define LWT_MESSAGE_ONLINE "online"
#define LWT_MESSAGE_OFFLINE "offline"
#define LWT_QOS 0
#define LWT_RETAIN true

// Data and Event Topics
// Base topic for device-specific communications
#define MQTT_DEVICE_BASE_TOPIC "devices/"
#define MQTT_DATA_TOPIC_SUFFIX "/data"   // MAC address will be prepended after base topic
#define MQTT_EVENT_TOPIC_SUFFIX "/events" // MAC address will be prepended after base topic

#endif // MQTT_CONFIG_H
