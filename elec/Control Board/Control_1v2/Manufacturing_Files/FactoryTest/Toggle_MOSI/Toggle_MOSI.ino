#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "Toggle_MOSI"
#define VERSION ";_Rev_0.1"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.2"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "
// This was written as a troubleshooting aid for what was found to be a short under the ISP connector to the LAN card.

#define BAUD_RATE 115200

#define LAN_CS0 10  // Chip select for LAN

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);  // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier

  pinMode(MOSI, OUTPUT);                   //
  digitalWrite(MOSI, !digitalRead(MOSI));  // toggle the MOSI

  pinMode(LAN_CS0, OUTPUT);
  digitalWrite(LAN_CS0, HIGH);  // deselect ethernet mode

  delay(10);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LAN_CS0, HIGH);  // deselect ethernet mode
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // toggle the LED

// Toggle both the chip select and the COPI (MOSI) for observation with oscilliscope.
  digitalWrite(LAN_CS0, !digitalRead(LAN_CS0));          // toggle the LAN_CS0
  digitalWrite(MOSI, !digitalRead(MOSI));                // toggle the MOSI
  delay(10);
  digitalWrite(LAN_CS0, !digitalRead(LAN_CS0));  // toggle the LAN_CS0
  digitalWrite(MOSI, !digitalRead(MOSI));        // toggle the MOSI

  delay(100);
}
