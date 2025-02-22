/*
  UDPSendReceiveString_OEDCS:
  This sketch receives UDP message strings, prints them to the serial port
  and sends an "acknowledge" string back to the sender

  A Processing sketch is included at the end of file that can be used to send
  and received messages for testing with a computer.

  created 21 Aug 2010
  by Michael Margolis

  This code is in the public domain.

  Modified by Forrest Lee Erickson for OEDCS
  Date: 20240217
*/
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "UDPSendReceiveString_OEDCS"
#define VERSION ";_Rev_0.2"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.2"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200

//Name the pins to the Display
#define DISPLAY_RESET 46  // Reset button on display front panel
#define DISPLAY_DC 47     // Data on display
#define DISPLAY_CS 48     // Chip select for display

#include <Ethernet.h>
#include <EthernetUdp.h>
#define UDP_TX_PACKET_MAX_SIZE 255  //increase UDP size


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
//IPAddress ip(192, 168, 1, 177);  //From the reference code
IPAddress ip(192, 168, 1, 137);  //OEDCS SN11 on Lee's LAN


unsigned int localPort = 8888;  // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier

  //Mini12864 GPIO setup
  pinMode(DISPLAY_CS, OUTPUT);
  pinMode(DISPLAY_DC, OUTPUT);
  pinMode(DISPLAY_RESET, INPUT_PULLUP);
  digitalWrite(DISPLAY_CS, HIGH);  // deselect Display mode
  digitalWrite(DISPLAY_DC, HIGH);

  //Name the pins to the Ethernet Shield
#define LAN_CS0 10    // Chip select for LAN
#define LAN_SD_CS1 4  // Chip select for SD card on LAN sheild

  pinMode(LAN_CS0, OUTPUT);     // make sure that the default chip select pin is set to output, even if you don't use it:
  digitalWrite(LAN_CS0, HIGH);  // deselect LAN mode
  //  pinMode(LAN_SD_CS1, PINPUT_PULLUP);      // On the Ethernet Shield, CS is pin 4
  pinMode(LAN_SD_CS1, OUTPUT);     // On the Ethernet Shield, CS is pin 4
  digitalWrite(LAN_SD_CS1, HIGH);  // deselect SD mode


  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(LAN_SD_CS1);  // Makes no sense but try
  Ethernet.init(LAN_CS0);  // Most Arduino shields
  // Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // start the Ethernet
  Ethernet.begin(mac, ip);
  // Check for Ethernet hardware present
  digitalWrite(LAN_CS0, HIGH);  // select ethernet mode
  // digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  // digitalWrite(LAN_SD_CS1, LOW);       // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
  while (Ethernet.hardwareStatus() == EthernetNoHardware) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // toggle the LED 
//    digitalWrite(LAN_CS0, !digitalRead(LAN_CS0));  // toggle the LAN_CS0
    digitalWrite(MOSI, !digitalRead(MOSI));  // toggle the MOSI

    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    //    while (true) {
    //      delay(1); // do nothing, no point running without Ethernet hardware
    delay(200);
    Ethernet.begin(mac, ip);  //FLE this added so that the Ethernet.hardwareStatus() ran for each loop.
  }
  digitalWrite(LAN_CS0, HIGH);     // deselect ethernet mode
  digitalWrite(LAN_SD_CS1, HIGH);  // deselect SD mode
  Serial.println("Found LAN hardware");

  digitalWrite(LAN_CS0, HIGH);     // select ethernet mode
  digitalWrite(DISPLAY_CS, HIGH);  // deselect Display mode
  digitalWrite(LAN_SD_CS1, LOW);   // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("LinkOFF Ethernet cable is not connected.");
    delay(100);
  }
  digitalWrite(LAN_CS0, HIGH);     // deselect ethernet mode
  digitalWrite(LAN_SD_CS1, HIGH);  // deselect SD mode

  Serial.println("Ethernet Found");
  // start UDP
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  digitalWrite(LAN_CS0, HIGH);     // select ethernet mode
  digitalWrite(DISPLAY_CS, HIGH);  // deselect Display mode
  digitalWrite(LAN_SD_CS1, LOW);   // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
  int packetSize = Udp.parsePacket();
  digitalWrite(LAN_CS0, HIGH);     // deselect ethernet mode
  digitalWrite(LAN_SD_CS1, HIGH);  // deselect SD mode

  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.print(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.print(", Contents: ");
    Serial.println(packetBuffer);

    //    // send a reply to the IP address and port that sent us the packet we received
    //    //    digitalWrite(LAN_SD_CS1, LOW);       // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
    //    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //    Udp.write(ReplyBuffer);
    //    Udp.endPacket();
    //    //    digitalWrite(LAN_SD_CS1, HIGH);       // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
  }
  delay(10);
}
