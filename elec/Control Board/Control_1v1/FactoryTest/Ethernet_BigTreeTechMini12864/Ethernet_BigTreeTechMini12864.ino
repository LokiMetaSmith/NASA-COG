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
#define VERSION ";_Rev_0.8"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200

#include <SPI.h>
#include <Ethernet.h>

// Blink the built in LED
const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillisLED = 0;        // will store last time LED was updated
const long interval_LED = 500;           // interval_LED at which to blink (milliseconds)

//Name the pins to the Display
#define DISPLAY_RESET 46            // Reset button on display front panel
#define DISPLAY_DC 47               // Data on display
#define DISPLAY_CS 48               // Chip select for display

//Name the pins to the Ethernet Shield
#define LAN_CS0 10     // Chip select for LAN 
#define LAN_SD_CS1 4   // Chip select for SD card on LAN sheild

//Name the pins to the Rotary Encoder Switch
#define ENC_SW 42   //A switch low when pressed.


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 177);  //From the reference code
IPAddress ip(192, 168, 1, 137);   //OEDCS SN11 on Lee's LAN
unsigned int localPort = 8888;      // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back
EthernetUDP Udp;   // An EthernetUDP instance to let us send and receive packets over UDP

// Check the LAN, update display
unsigned long previousMillisLAN = 0;        // will store last time LAN was updated
const long interval_LAN = 1000;           // interval at which to check LAN and update display (milliseconds)

void updateLAN_Display(void) {
  digitalWrite(LAN_CS0, HIGH);       // select ethernet mode
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
//  digitalWrite(LAN_SD_CS1, LOW);       // Select SD mode  THIS IS REQUIRED FOR THE LAN TO WORK. Missnamed?
  auto link = Ethernet.linkStatus();
  digitalWrite(LAN_CS0, HIGH);       // deselect ethernet mode
  digitalWrite(LAN_SD_CS1, HIGH);       // deselect SD mode
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
  }//end case
}// end updateLAN_Display()

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier


  // LAN setup
  // You can use Ethernet.init(pin) to configure the CS pin
  pinMode(LAN_CS0, OUTPUT);    // make sure that the default chip select pin is set to output, even if you don't use it:
  digitalWrite(LAN_CS0, HIGH);       // deselect LAN mode
  pinMode(LAN_SD_CS1, OUTPUT);      // On the Ethernet Shield, CS is pin 4
  digitalWrite(LAN_SD_CS1, HIGH);       // deselect SD mode
  Ethernet.init(LAN_CS0);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // Setup for switch on Mini13864
  pinMode(ENC_SW, INPUT_PULLUP);  //A switch low when pressed.

  //Mini12864 GPIO setup
  pinMode(DISPLAY_CS, OUTPUT);
  pinMode(DISPLAY_DC, OUTPUT);
  pinMode(DISPLAY_RESET, INPUT_PULLUP);
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  digitalWrite(DISPLAY_DC, HIGH);

  //Setup Back light to display and rotary encoder. Splash Message
  setupBacklights();  //Setup the neopixels
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  digitalWrite(LAN_CS0, HIGH);       // deselect LAN mode
  setupDisplay();     //The Graphic display
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  digitalWrite(LAN_CS0, HIGH);       // deselect LAN mode
  splashScreen();     //A message
  digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
  digitalWrite(LAN_SD_CS1, HIGH);       // deselect SD mode
  digitalWrite(LAN_CS0, HIGH);       // deselect LAN mode

  digitalWrite(ledPin, LOW);
}//end setup()

void loop() {
  unsigned long currentMillis = millis(); // for LED

  //Time to Update the LED?
  if (currentMillis - previousMillisLED >= interval_LED) {
    // save the last time you blinked the LED
    previousMillisLED = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }//end LED update.

  //Time to Update the LINK Status?
  if (currentMillis - previousMillisLAN >= interval_LAN) {
    // save the last time you checked the LAN
    previousMillisLAN = currentMillis;
    updateLAN_Display();
  }//end LAN update.

}// end loop()
