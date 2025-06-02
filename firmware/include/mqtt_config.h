#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT Broker Details
#define MQTT_BROKER_IP "192.168.1.100" // Replace with your MQTT Broker's IP or hostname
#define MQTT_BROKER_PORT 1883

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
