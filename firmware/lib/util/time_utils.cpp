#include "time_utils.h"
#include <EthernetUdp.h> // For NTP
#include <debug.h>       // For CogCore::Debug
#include <util.h>        // For t_millis() and watchdogReset()

volatile unsigned long system_epoch = 0; // Definition of the global epoch
// Static variable to store millis() at the time of successful NTP sync
static unsigned long millis_at_sync = 0;

#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message
byte ntp_packet_buffer[NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

bool syncNTPTime() {
    // Note: This function attempts NTP synchronization up to 5 times at startup.
    // For long-running applications, more sophisticated periodic re-synchronization
    // or handling of prolonged NTP failure might be necessary.
    EthernetUDP Udp; // Temporary UDP instance for NTP
    bool ntp_success = false;

    // Try to start UDP for NTP
    if (!Udp.begin(NTP_LOCAL_PORT)) {
        CogCore::Debug<const char*>("Failed to start UDP for NTP on port ");
        CogCore::Debug<int>(NTP_LOCAL_PORT);
        CogCore::Debug<const char*>("
");
        return false;
    }
    CogCore::Debug<const char*>("UDP for NTP started on port ");
    CogCore::Debug<int>(NTP_LOCAL_PORT);
    CogCore::Debug<const char*>("
");

    // Try up to 5 times to get NTP time
    for (int i = 0; i < 5; ++i) {
        CogCore::Debug<const char*>("NTP attempt #");
        CogCore::Debug<int>(i + 1);
        CogCore::Debug<const char*>("
");

        // Clear the buffer
        memset(ntp_packet_buffer, 0, NTP_PACKET_SIZE);
        // Initialize values needed to form NTP request
        ntp_packet_buffer[0] = 0b11100011;   // LI, Version, Mode
        // (The rest of the NTP packet fields can be left as 0 for a simple request)

        // Send NTP packet
        if (!Udp.beginPacket(NTP_SERVER_NAME, 123)) { // NTP requests are to port 123
            CogCore::Debug<const char*>("NTP: DNS lookup or beginPacket failed for time server.
");
            delay(1000); // Wait a bit before retrying
            watchdogReset();

            continue;
        }
        Udp.write(ntp_packet_buffer, NTP_PACKET_SIZE);
        if (!Udp.endPacket()) {
            CogCore::Debug<const char*>("NTP: Could not send time request.
");
            delay(1000);
            watchdogReset();
            continue;
        }

        // Wait for a response with timeout
        unsigned long start_ms = t_millis();
        int packet_size = 0;
        while (t_millis() - start_ms < 2000) { // 2-second timeout
            packet_size = Udp.parsePacket();
            if (packet_size >= NTP_PACKET_SIZE) {
                break;
            }
            delay(10); // Small delay to yield
            watchdogReset();
        }

        if (packet_size >= NTP_PACKET_SIZE) {
            Udp.read(ntp_packet_buffer, NTP_PACKET_SIZE); // Read the packet into the buffer

            unsigned long high_word = word(ntp_packet_buffer[40], ntp_packet_buffer[41]);
            unsigned long low_word = word(ntp_packet_buffer[42], ntp_packet_buffer[43]);
            unsigned long secs_since_1900 = high_word << 16 | low_word;

            const unsigned long seventy_years = 2208988800UL;
            system_epoch = secs_since_1900 - seventy_years;
            millis_at_sync = t_millis(); // Store millis at time of successful sync

            CogCore::Debug<const char*>("NTP time received. Epoch: ");
            CogCore::Debug<unsigned long>(system_epoch);
            CogCore::Debug<const char*>("
");
            ntp_success = true;
            break; // Success
        } else {
            CogCore::Debug<const char*>("NTP: No response or bad packet.
");
        }
        watchdogReset();
    }

    Udp.stop(); // Release UDP port
    if (!ntp_success) {
        CogCore::Debug<const char*>("Failed to get NTP time after multiple attempts.
");
    }
    return ntp_success;
}

unsigned long getCurrentTimestamp() {
    if (system_epoch == 0) { // NTP sync hasn't happened or failed
        return t_millis() / 1000; // Fallback to millis since boot (seconds)
    }
    // Calculate current time based on the last synced epoch and millis passed since then
    return system_epoch + (t_millis() - millis_at_sync) / 1000;
}
