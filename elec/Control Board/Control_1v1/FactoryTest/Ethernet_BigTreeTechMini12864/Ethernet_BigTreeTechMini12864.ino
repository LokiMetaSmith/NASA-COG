/*
  Test of Ethernet sheild and BigTreeTech Mini 12864 display
  Displayes LAN Link Status
  Splash Screen

  This sketch prints the Ethernet link status. When the
  Ethernet cable is connected the link status should go to "ON".
  NOTE: Only WIZnet W5200 and W5500 are capable of reporting
  the link status. W5100 will report "Unknown".
  Hardware:
   - Ethernet shield or equivalent board/shield with WIZnet W5200/W5500
   - BigTreeTech Mini12864 display
   - Control V1.1 assembly
*/

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "Ethernet_BigTreeTechMini12864"
#define VERSION ";_Rev_0.2"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200

#include <SPI.h>
#include <Ethernet.h>

//Name the display pins on the Due GPIO
#define DISPLAY_RESET 46            // Reset button on display front panel
#define DISPLAY_DC 47               // Data on display
#define DISPLAY_CS 48               // Chip select for display

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier

  //Mini12864 setup

  pinMode(DISPLAY_CS, OUTPUT);
  pinMode(DISPLAY_DC, OUTPUT);
  pinMode(DISPLAY_RESET, OUTPUT);
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  digitalWrite(DISPLAY_DC, HIGH);
  digitalWrite(DISPLAY_RESET, HIGH);

  //Name the pins from the Ethernet Shield
#define LAN_CS 10     // Chip select for LAN 
#define LAN_SD_CS 4   // Chip select for SD card on LAN sheild

  // LAN setup
  // You can use Ethernet.init(pin) to configure the CS pin
  pinMode(LAN_CS, OUTPUT);    // make sure that the default chip select pin is set to output, even if you don't use it:
  pinMode(LAN_SD_CS, OUTPUT);      // On the Ethernet Shield, CS is pin 4
  Ethernet.init(LAN_CS);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // Setup for switch on Mini13864
#define ENC_SW 42   //A switch
  pinMode(ENC_SW, INPUT_PULLUP);

  //Display setup
  setupBacklights();  //Setup the neopixels
  setupDisplay();     //The Graphic display
  splashScreen();     //A message

}//end setup()

void loop() {
  digitalWrite(10, HIGH);       // select ethernet mode
  //  digitalWrite(4, LOW);       // deselect SD mode
  auto link = Ethernet.linkStatus();
  digitalWrite(10, LOW);       // deselect ethernet mode
  //  digitalWrite(4, LOW);       // deselect SD mode
  Serial.print("Link status: ");
  switch (link) {
    case Unknown:
      Serial.println("Unknown?");
      reportLAN_DisplayUnknown();
      break;
    case LinkON:
      Serial.println("ON");
      reportLAN_DisplayOn();
      break;
    case LinkOFF:
      Serial.println("OFF");
      reportLAN_DisplayOff();
      break;
  }
  delay(1000);
}
