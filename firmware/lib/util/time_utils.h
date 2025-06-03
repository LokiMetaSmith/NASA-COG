#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>

// Global variable to store the system epoch (seconds since Jan 1, 1970)
// Marked volatile as it can be updated by NTP sync and used elsewhere.
extern volatile unsigned long system_epoch;

// NTP server details
#define NTP_SERVER_NAME "time.nist.gov"
#define NTP_LOCAL_PORT 8888 // Local port to listen for NTP packets

// Function to initialize and perform NTP synchronization
// Needs access to an EthernetUDP instance.
bool syncNTPTime();

// Function to get current Unix timestamp
unsigned long getCurrentTimestamp();

#endif // TIME_UTILS_H
