// Copyright (C) 2023 Robert Read.

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

#include <Arduino.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "utility/w5100.h"
#include <network_udp.h>
#include <flash.h>
#include <debug.h>

// TODO: all of this should be moved to a more accessible configuration file.
char timeServer[] = "time.nist.gov";
char mcogs[] = "mcogs.coslabs.com";

byte packetBuffer[buffMax]; //buffer to hold incoming packet,

#define localPort 2390
#define serverPort 57575

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
#define W5200_CS  10
#define SDCARD_CS 4

#define WATCH_DOG_TIME 10000

uint8_t
NetworkUDP::networkCheck() {
  switch(Ethernet.linkStatus()) {
  case Unknown: return 1; break;
  case LinkON: break;
  case LinkOFF: return 2; break;
  }

  switch (Ethernet.maintain()) {
  case 0: break; // nothing
  case 1: return 3; // renewal failed
  case 2: break; // renew success
  case 3: return 3; // rebind failed
  case 4: break; // rebind success
  }

  return 100;
}

void 
NetworkUDP::enableEthernet() {	
        digitalWrite(W5200_CS, LOW);       // select Network mode
}

void 
NetworkUDP::disableEthernet() {
        digitalWrite(W5200_CS, HIGH);       // deselect Network mode        
}

void 
NetworkUDP::printPacketInfo(int packetsize) {
  IPAddress remoteIp = Udp.remoteIP();
  if (DEBUG_UDP > 1) {
    CogCore::Debug<const char *>("UDP Packet received, size ");
    CogCore::Debug<uint32_t>(packetsize);
    CogCore::Debug<const char *>("\n");
    CogCore::Debug<const char *>("From ");
    char t[16];
    sprintf(t, "%u.%u.%u.%u", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3]);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>(", port ");
    CogCore::Debug<uint32_t>(Udp.remotePort());
    CogCore::Debug<const char *>("\n");
  }
}

void
NetworkUDP::printTime(unsigned long time) {
  CogCore::Debug<const char *>("Unix time = ");
  CogCore::Debug<uint32_t>(time);
  CogCore::Debug<const char *>("\n");
  // print the hour, minute and second:
  CogCore::Debug<const char *>("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  // print the hour (86400 equals secs per day)
  if ((time % 86400L) / 3600 < 10)  CogCore::Debug<const char *>("0");
  CogCore::Debug<uint32_t>((time  % 86400L) / 3600);
  CogCore::Debug<const char *>(":");

  // In the first 10 minutes of each hour, we'll want a leading '0'
  if (((time % 3600) / 60) < 10) CogCore::Debug<const char *>("0");

  // print the minute (3600 equals secs per minute)
  CogCore::Debug<uint32_t>((time  % 3600) / 60);
  CogCore::Debug<const char *>(":");

  // In the first 10 seconds of each minute, we'll want a leading '0'
  if ((time % 60) < 10) CogCore::Debug<const char *>("0");

  CogCore::Debug<uint32_t>(time % 60); // print the second
  CogCore::Debug<const char *>("\n");
}

// send an NTP request to the time server at the given address
unsigned long
NetworkUDP::getTime(uint16_t timeout) {
#define NTP_PACKET_SIZE 48
  byte ntpBuffer[NTP_PACKET_SIZE];

  memset(ntpBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in buffer to 0

  // Initialize values needed to form NTP request
  ntpBuffer[0] = 0b11100011;   // LI, Version, Mode
  ntpBuffer[1] = 0;     // Stratum, or type of clock
  ntpBuffer[2] = 6;     // Polling Interval
  ntpBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntpBuffer[12]  = 49;
  ntpBuffer[13]  = 0x4E;
  ntpBuffer[14]  = 49;
  ntpBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  if (! Udp.beginPacket(timeServer, 123)) { //NTP requests are to port 123
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("can't resolve timeserver\n");
    return 0;
  }
  Udp.write(ntpBuffer, NTP_PACKET_SIZE);
  if (! Udp.endPacket()) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("Could not send time request\n");
    return 0;
  }

  // Note: This is a hard loop --- UDP_TIMEOUT blocks the machine for that time
  unsigned long startMs = millis();
  int packetSize = 0;
  while (! packetSize && (millis() - startMs) < timeout) {
    delay(10);
    packetSize = Udp.parsePacket();
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("No time data returned\n");
    return 0;
  }

  Udp.read(ntpBuffer, NTP_PACKET_SIZE);

  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  unsigned long highWord = word(ntpBuffer[40], ntpBuffer[41]);
  unsigned long lowWord = word(ntpBuffer[42], ntpBuffer[43]);

  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;

  // subtract seventy years:
  return secsSince1900 - seventyYears;
}

bool
NetworkUDP::sendData(char *data, unsigned long current_time, uint16_t timeout) {
  if (! Udp.beginPacket(mcogs, serverPort)) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("Can't resolve mcogs\n");
    return false;
  }

  Udp.write("PUT ", 4);
  Udp.write("/", 1);
  Udp.write(macString, 17);
#ifdef RIBBONFISH
  Udp.write("/OEDCS", 6);
#else
  Udp.write("/Stage2", 7);
#endif
  Udp.write("/Data\n", 6);
  Udp.write("{ \"TimeStamp\": ", 15);
  char ts[15];
  sprintf(ts, "%ld", current_time);
  Udp.write(ts, strlen(ts));
  Udp.write(",\n", 2);
  if (data && strlen(data)) Udp.write(data, strlen(data));
  Udp.write("}", 1);
  if (! Udp.endPacket()) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("Can't send data\n");
    return false;
  }

  unsigned long startMs = millis();
  int packetSize = 0;
  while (! packetSize && (millis() - startMs) < timeout) {
    delay(10);
    packetSize = Udp.parsePacket();
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("no response\n");
    return false;
  }

  Udp.read(packetBuffer, packetSize);
  packetBuffer[packetSize] = '\0';
  if (strncmp((char *)packetBuffer, "posted", 6)) return false;
  if (DEBUG_UDP > 2) {
    CogCore::Debug<const char *>((char *)packetBuffer);
    CogCore::Debug<const char *>("\n");
  }
  return true;
}

bool
NetworkUDP::getParams(uint16_t timeout) {
  if (! Udp.beginPacket(mcogs, serverPort)) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("can't resolve mcogs\n");
    return false;
  }
  Udp.write("GET ", 4);
  Udp.write("/", 1);
  Udp.write(macString, 17);
  Udp.write("/", 1);
#ifdef RIBBONFISH
  Udp.write("OEDCS/", 6);
#else
  Udp.write("STAGE2/", 7);
#endif  
  Udp.write("Params\n", 7);
  if (! Udp.endPacket()) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("can't send request\n");
    return false;
  }

  unsigned long startMs = millis();
  int packetSize = 0;
  while (!packetSize && (millis() - startMs) < timeout) {
    delay(10);
    if (DEBUG_UDP > 2) {
      CogCore::Debug<const char *>("Calling parse packet (should loop)\n");
      CogCore::Debug<uint32_t>((millis() - startMs));
      CogCore::Debug<const char *>("\n");
    }
    packetSize = Udp.parsePacket();
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) CogCore::Debug<const char *>("no params returned\n");
    return false;
  }

  IPAddress remoteIp = Udp.remoteIP();
  if (DEBUG_UDP > 1) {
    CogCore::Debug<const char *>("UDP Packet received, size ");
    CogCore::Debug<uint32_t>(packetSize);
    CogCore::Debug<const char *>("\n");
    delay(50);
    CogCore::Debug<const char *>("From ");
    delay(50);
    char t[16];
    sprintf(t, "%u.%u.%u.%u", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3]);
    CogCore::Debug<const char *>(t);
    CogCore::Debug<const char *>(", port ");
    CogCore::Debug<uint32_t>(Udp.remotePort());
    delay(50);
  }

  Udp.read(packetBuffer, packetSize);
  if (DEBUG_UDP > 1) {
    CogCore::Debug<uint32_t>(packetSize);
    CogCore::Debug<const char *>("\n");
    delay(50);
  }
  packetBuffer[packetSize] = '\0';

  if (DEBUG_UDP > 1) {
    CogCore::Debug<const char *>("About to construct config\n");
    delay(50);
  }

  if (DEBUG_UDP > 1) {
    CogCore::Debug<const char *>("constructed config\n");
    delay(50);
    CogCore::Debug<const char *>((char *)packetBuffer);
    CogCore::Debug<const char *>("\n");
    delay(50);
  }

  // Now, at this point, we roughly have a new script.
  // so we will parse it and poke it into the machine script as
  // a single pointer switch.
  return true;
}

void
NetworkUDP::printNet() {
  //  Ethernet.MACAddress(mac);
  CogCore::Debug<const char *>("The MAC address is: ");
  for (uint8_t i = 0; i < 6; i++) {
    char t[4];
    sprintf(t, "%0X", mac[i]);
    CogCore::Debug<const char *>(t);
    if (i < 5) CogCore::Debug<const char *>(":");
  }
  CogCore::Debug<const char *>("\n");

  CogCore::Debug<const char *>("IP address: ");
  IPAddress   tempIP = Ethernet.localIP();
  char t[16];
  sprintf(t, "%u.%u.%u.%u", tempIP[0], tempIP[1], tempIP[2], tempIP[3]);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>("\n");
  CogCore::Debug<const char *>("Subnet Mask: ");
  tempIP = Ethernet.subnetMask();
  sprintf(t, "%u.%u.%u.%u", tempIP[0], tempIP[1], tempIP[2], tempIP[3]);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>("\n");
  CogCore::Debug<const char *>("Gateway: ");
  tempIP = Ethernet.gatewayIP();
  sprintf(t, "%u.%u.%u.%u", tempIP[0], tempIP[1], tempIP[2], tempIP[3]);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>("\n");
  CogCore::Debug<const char *>("DNS Server: ");
  tempIP = Ethernet.dnsServerIP();
  sprintf(t, "%u.%u.%u.%u", tempIP[0], tempIP[1], tempIP[2], tempIP[3]);
  CogCore::Debug<const char *>(t);
  CogCore::Debug<const char *>("\n");
}

uint8_t
NetworkUDP::networkStart() {
  pinMode(4, OUTPUT);      // On the Ethernet Shield, CS is pin 4
  digitalWrite(4, HIGH);       // deselect SD mode
  Ethernet.init(10);

  if (W5100.init() == 0) return 1;

  if (!Ethernet.hardwareStatus()) return 2;

  if (setGlobalMacAddress() != 0) {
    CogCore::Debug<const char *>("Problem reading EFC\n");
    CogCore::Debug<const char *>("Using default mac\n");
  }

  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac);
  uint32_t add = 0;
  W5100.setIPAddress((uint8_t *) &add);
  SPI.endTransaction();
  
  uint32_t startMs = millis();
  // this seems to take about 3 seconds!!!  don't change
  while (W5100.getLinkStatus() != LINK_ON && (millis() - startMs) < 3000) {
    delay(10);
    watchdogReset();
  }
  
  if (W5100.getLinkStatus() != LINK_ON) return 3;
  
  if (Ethernet.begin(mac, WATCH_DOG_TIME - 500, 3000) == 0) return 4;
  
  printNet();
  
  Udp.stop();
  if (!Udp.begin(localPort)) return 5;

  if (DEBUG_UDP > 1) printNet();

  for (int i = 10; epoch == 0 && i > 0; i--) {
    epoch = getTime(2000);
    delay(50);
  }

  if (epoch == 0) CogCore::Debug<const char *>("Can't get time\n");
  else printTime(epoch);

  return 0;
}
