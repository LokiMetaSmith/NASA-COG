/* Program: DueWithThreeSSRs
  Tests the three SSR drive circuits on the Control V1.2 assembly
  Tests the SHUT DOWN switch
  Tests four power supplies, 24V, 12V, AUX1 and AUX2.
  Setup:
  Connect the +12V batter to TBD.
  Connect the progrmable current supply at J10
  Connect an LED with series resistor at J13, J30 and J31. Pin 1 is positive and Pin 2 is ground.
  Series resistor should limit current from +24V for LED.
  SSR1 and SSR2 flash independently.
  Press switch S2, "SHUT DOWN" to turn on the SSR3 LED.
  Ethernet link status reported on serial monitor.
  Read voltages and display during setup before the long power supply setup.
  Note the power supply test takes about 38 seconds.
*/

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "OEDCS_Factory_Test V1.2"
#define VERSION ";_Rev_0.7"                         // Report UID and MAC
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.2"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200
//

#include <LiquidCrystal_I2C.h>
#define ADDRESS_LCD_MARYVILLE 0x27
#define COLUMNs_LCD_MARYVILLE 20
#define ROWs_LCD_MARYVILLE 4
LiquidCrystal_I2C lcd(ADDRESS_LCD_MARYVILLE, COLUMNs_LCD_MARYVILLE, ROWs_LCD_MARYVILLE);  // set the LCD address to 0x27 for a 16 chars and 2 line display


#include <SPI.h>
#include <Ethernet.h>
#include <MQTT.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 177 };  // <- change to match your network


EthernetClient net;
MQTTClient client;

//MQTT Connect
void connect() {
  Serial.print("connecting...");
  while (!client.connect("PubInv", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT connected!");
  // client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

#define ETHERNET_CS 10  //HIGH->Enabled, LOW->Disabled
int link_status;

long previousLinkMillis = 0;
const long LINK_TIME = 1500;  // One and 1/2 second.

const int SET_BLOWER_LOW = 220;
//const int SET_BLOWER_MID = 128;  // Medium
const int SET_BLOWER_HIGH = 20;

//Check power supplies. Reports status on serial port, OLED display.
class PowerSense {
  // Class Member Variables
  // These are initialized at startup
  int ADCinPin;                  // the number of the ADC pin
  long ReadPeriod;               // milliseconds of on-time
  unsigned long previousMillis;  // will store last time ADC was read
  String my_pinName;
  float my_R1;
  float my_R2;
  float voltage;
  int my_offsetX = 0;
  int my_offsetY = 41;  //Benieth center

  // Constructor - creates a Flasher
  // and initializes the member variables and state
public:
  PowerSense(const String pinName, int pin, long period, float R1 = 10000, float R2 = 24700, int offsetX = 0, int offsetY = 0) {
    ADCinPin = pin;
    previousMillis = millis();
    ReadPeriod = period;
    my_pinName = pinName;
    my_R1 = R1;
    my_R2 = R2;
    voltage = 0;
    my_offsetX = offsetX;
    my_offsetY = offsetY;
  }

  void Update() {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= ReadPeriod) {
      previousMillis = currentMillis;  // Remember the time

      //Force ADCinPin low so that if floating it will read low for the test below.
      // pinMode(ADCinPin, OUTPUT);
      // digitalWrite(ADCinPin, LOW);
      // pinMode(ADCinPin,INPUT);
      // delay(100)    ;

      voltage = analogRead(ADCinPin) * 3.3 * (my_R1 + my_R2) / (1023 * my_R2);  // RAW Read of the ADC
      Serial.print(my_pinName);                                                 //
      Serial.print(": ");                                                       //
      Serial.println(voltage);                                                  // RAW Read of the ADC
    }
  }
};  //end PowersSense

class Flasher {
  // Class Member Variables
  // These are initialized at startup
  int ledPin;    // the number of the LED pin
  long OnTime;   // milliseconds of on-time
  long OffTime;  // milliseconds of off-time

  // These maintain the current state
  int ledState;                  // ledState used to set the LED
  unsigned long previousMillis;  // will store last time LED was updated

  // Constructor - creates a Flasher
  // and initializes the member variables and state
public:
  Flasher(int pin, long on, long off) {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);

    OnTime = on;
    OffTime = off;

    ledState = LOW;
    previousMillis = 0;
  }

  void Update() {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();

    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) {
      ledState = LOW;                  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
      ledState = HIGH;                 // turn it on
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
  }
};

void UpdateEthernet() {
  // check to see if it's time to update LAN Link status
  unsigned long currentMillis = millis();

  if (((currentMillis - previousLinkMillis) >= LINK_TIME) || (currentMillis < previousLinkMillis)) {
    previousLinkMillis = currentMillis;
    Serial.println("Checking LAN.");
    //FLE    digitalWrite(ETHERNET_CS, LOW);  // select ethernet mode
    link_status = Ethernet.linkStatus();
    //      delay(1000);  // Hold the splash screen a second
    auto link = Ethernet.linkStatus();
    //      delay(1000);  // Hold the splash screen a second
    digitalWrite(ETHERNET_CS, HIGH);  // deselect ethernet mode
    Serial.print("Link status: ");
    lcd.setCursor(0, 2);
    lcd.print("Link status: ");

    switch (link_status) {
      case Unknown:
        Serial.println("Unknown");
        lcd.setCursor(12, 2);
        lcd.print("Unknown");
        break;
      case LinkON:
        Serial.println("ON");
        lcd.setCursor(12, 2);
        lcd.print("ON     ");
        break;
      case LinkOFF:
        Serial.println("OFF");
        lcd.setCursor(12, 2);
        lcd.print("OFF   ");
        break;
    }
  }  // update time.
}


// Resistive dividers Vin = Vadc*3.3/1032 *(R1+R1)/R2
//Read every two seconds
//              Signal name, Pin number, R1, R2, Xoffset, Yoffset
//        PowerSense(const String pinName, int pin, long period, float R1 = 10000, float R2 = 24700, int offsetX = 0, int offsetY = 0)
PowerSense SENSE_24V("+24V ", 1, 10000, 40000, 4700, 0, 50);     //Read A1. R101+R105+R106, R102.
PowerSense SENSE_12V("+12V ", 2, 10000, 40000, 10000, 64, 50);   //Read A2. R103+R107+R108, R104.
PowerSense SENSE_AUX1("AUX1 ", 3, 10000, 10000, 14700, 0, 60);   //Read A3. R123, R124+R125.
PowerSense SENSE_AUX2("AUX2 ", 6, 10000, 10000, 14700, 64, 60);  //Read A6 R126, R127+R128.

//Control V1.1 signal pin names
#define SSR1 51
#define SSR2 52
#define SSR3 53
#define SHUT_DOWN 49
// #define LED_RED 43
// #define LED_BLUE 44
// #define LED_GREEN 45
//Front Panel LEDs and switches
#define FAULT_LED 43
#define STATUS_LED 44
#define ENC_SW 42

#define nFAN1_PWM 9       // The pin D9 for driving the Blower.
#define BLOWER_ENABLE 22  // The pin D22 for Enable 24V to the Blower.

/*(GPIO HIGH, Battery engaged, GPIO LOW, battery disengaged; when GPIO LOW unit will shut off when front
power switch is toggled)  */
#define KEEP_ALIVE 45  // The pin D45 for the control to the relay which when active high connects Standby Battery


// Programable Power Supply Enable
#define PS1_EN 23
#define PS2_EN 8


//Flasher to exercise the SSRs pins and the Building LED.
Flasher led0(13, 100, 400);    //Pins for Control V1.1
Flasher led1(SSR1, 100, 400);  //Pins for Control V1.1
Flasher led2(SSR2, 350, 350);
Flasher led3(FAULT_LED, 300, 400);
Flasher led4(STATUS_LED, 375, 400);
//Flasher led3(SSR3, 150, 350);

//Tests for press of switch, "SHUT DOWN". Turns OFF the SSR3 LED
void updateSHUTDOWN() {
  if (digitalRead(SHUT_DOWN) == LOW) {
    Serial.println("Shutdown button pressed");
    digitalWrite(SSR3, LOW);
    analogWrite(nFAN1_PWM, SET_BLOWER_HIGH);  // Set for high
    digitalWrite(KEEP_ALIVE, HIGH);           // TUrn on Keep Alive.
    delay(500);
  } else {
    digitalWrite(SSR3, HIGH);
    analogWrite(nFAN1_PWM, SET_BLOWER_LOW);  // Set for low
    digitalWrite(KEEP_ALIVE, LOW);           // TUrn off Keep Alive.
  }
}  //end update shutdown button


bool updatePowerMonitor(void) {
  //Analog read of the +24V expected about 3.25V at ADC input.
  // SENSE_24V on A1.
  // Full scale is 1023, ten bits for 3.3V.
  //30K into 4K7
  const long FullScale = 1023;
  const float percentOK = 0.75;
  const long R1 = 10000;
  const long R2 = 24700;
  const float Vcc = 3.3;
  bool powerIsGood = false;
  //int lowThreshold24V = (24 * (R2 / (R1 + R2)) / Vcc) * FullScale * percentOK;  //1023 * 3 / 4;
  //  int lowThreshold24V = (24 * (24700 / (10000 + 24700)) / 3.3) * 1023.0 * 3.0 / 4.0;  //1023 * 3 / 4;
  int lowThreshold24V = 600;  // Typical is 790 so use about 3/4
  const long POWER_MONITOR_TIME = 2000;
  static long previousPowerMillis = 0;

  long currentMillis = millis();

  if (((currentMillis - previousPowerMillis) >= POWER_MONITOR_TIME) || (currentMillis < previousPowerMillis)) {
    previousPowerMillis = currentMillis;
    Serial.print("analogRead(A1)= ");
    Serial.println(analogRead(A1));

    Serial.print("lowThreshold24V= ");
    Serial.println(lowThreshold24V);

    // pinMode(A1, OUTPUT);    //Force A1 low so that if floating it will read low for the lowThreshold24V test below
    // digitalWrite(A1, LOW);
    // pinMode(A1, INPUT);
    int read24V = analogRead(A1);
    if (read24V > lowThreshold24V) {
      Serial.println("24V power OK");
      powerIsGood = true;
      return true;
    } else {
      Serial.print("Bad 24V power = ");
      Serial.println(int((3.3 * read24V) / 1023));
      powerIsGood = false;
      return false;
    }
  }
}
class PSU {
  // #define ADDRESS 0x04
#define ADDRESS 0x00
  //#define MYDELAY 500
#define MYDELAY 10
  uint8_t error = 0;       // error
  char manuf[17];          // INFO 0
  char model[17];          // INFO 1
  char voltage_string[5];  // INFO 2
  char revision[5];        // INFO 3
  char manuf_date[9];      // INFO 4
  char serial[17];         // INFO 5
  char country[17];        // INFO 6
  uint16_t rate_voltage;   //50-51
  uint16_t rate_current;   //52-53
  uint16_t max_voltage;    //54-55
  uint16_t max_current;    //56-57
  uint16_t out_voltage;    //60-61
  uint16_t out_current;    //62-63
  uint8_t temp;            //68
  uint8_t status0;         //6C
  uint8_t status1;         //6F
  uint16_t set_voltage;    // 70-71 r/w
  uint16_t set_current;    // 72-73 r/w
  uint8_t control;         //  7C  r/w
  uint8_t on_off;
  String serial1Buffer;
public:
  int evalResponse() {
    bool validEntry = true;
    char buff[255];

    uint8_t c = 0;
    //while( validEntry)

    while (validEntry) {
      c = Serial1.readBytesUntil('\n', buff, sizeof buff);
      if (c == 3 && (buff[0] == '=' && buff[1] == '>')) {
        // Serial.println("Command executed/recieved successfully.");
        error = 0;
      }
      if (c == 3 && (buff[0] == '?' && buff[1] == '>')) {
        Serial.println("Command error, not accepted.");
        validEntry = false;
        error = 1;
      }
      if (c == 3 && (buff[0] == '!' && buff[1] == '>')) {
        Serial.println("Command correct but execution error (e.g. parameters out of range).");
        validEntry = false;
        error = 2;
      } else {
        validEntry = false;
        //return 0;
      }
    }
    return 1;
  }

  int setPS_Addr(uint8_t addr) {
    Serial1.print("ADDS ");
    Serial1.print(addr);
    Serial1.print("\r\n");
    delay(50);
    return evalResponse();
  }

  int setPS_Val(uint8_t addr, const char *loc, const char *val) {
    if (!setPS_Addr(addr)) {
      Serial.println("didn't set address");
      return 0;
    }

    Serial1.print(loc);
    Serial1.print(' ');
    Serial1.print(val);
    Serial1.print("\r\n");
    delay(100);
    char b[5];
    int c = Serial1.readBytesUntil('\n', b, sizeof b);
    if (c != 3 || b[0] != '=' || b[1] != '>') return 0;
    return 1;
  }

  int setPS_GlobOnOff(uint8_t addr, const char *val) {
    if (strcasecmp(val, "on") == 0) val = "1";
    else val = "0";
    return setPS_Val(addr, "GLOB", val);
  }

  int setPS_OnOff(uint8_t addr, const char *val) {
    if (strcasecmp(val, "on") == 0) val = "1";
    else val = "0";
    return setPS_Val(addr, "POWER", val);
  }

  int setPS_Voltage(uint8_t addr, uint16_t volts) {
    char b[7];
    snprintf(b, sizeof b, "%4.1f", volts / 100.0);
    return setPS_Val(addr, "SV", b);
  }

  int setPS_Current(uint8_t addr, uint16_t amps) {
    char b[7];
    snprintf(b, sizeof b, "%4.1f", amps / 100.0);
    return setPS_Val(addr, "SI", b);
  }

  // TODO: This is untested
  int setPS_GCurrent(uint8_t addr, uint16_t amps) {
    char b[7];
    snprintf(b, sizeof b, "%4.1f", amps / 100.0);
    return setPS_Val(addr, "GSI", b);
  }

  char *getPS_Val(uint8_t addr, const char *val) {
    static char rval[250];
    static char b[50];
    rval[0] = '\0';
    b[0] = '\0';
    if (!setPS_Addr(addr)) {
      Serial.println("didn't set address");
      return 0;
    }
    int c = 0;
    Serial1.print(val);
    Serial1.print("\r\n");
    delay(100);
    c = Serial1.readBytesUntil('\n', b, sizeof b);
    b[c - 1] = '\0';

    for (int i = 0; i < 5; i++) {
      if (b[0] != '=' && b[1] != '>') {
        strcat(rval, b);
      }
      if (b[0] == '?' && b[1] == '>') {
        Serial.println("Command error, not accepted.");
      }
      if (b[0] == '!' && b[1] == '>') {
        Serial.println("Command correct but execution error (e.g. parameters out of range).");
      }
      delay(10);
      c = Serial1.readBytesUntil('\n', b, sizeof b);
      b[c - 1] = '\0';
    }
    return rval;
  }

  void getPS_Manuf(int addr) {
    char *r = getPS_Val(addr, "INFO 0");
    strncpy(manuf, r, sizeof manuf);
    Serial.print("PS Manufacturier= ");
    Serial.println(manuf);
  }

  void getPS_Model(int addr) {
    char *r = getPS_Val(addr, "INFO 1");
    strncpy(model, r, sizeof model);
  }

  void getPS_VoltageString(int addr) {
    char *r = getPS_Val(addr, "INFO 2");
    strncpy(voltage_string, r, sizeof voltage_string);
  }

  void getPS_Revision(int addr) {
    char *r = getPS_Val(addr, "INFO 3");
    strncpy(revision, r, sizeof revision);
  }

  void getPS_ManufDate(int addr) {
    char *r = getPS_Val(addr, "INFO 4");
    strncpy(manuf_date, r, sizeof manuf_date);
  }

  void getPS_Serial(int addr) {
    char *r = getPS_Val(addr, "INFO 5");
    strncpy(serial, r, sizeof serial);
  }

  void getPS_Country(int addr) {
    char *r = getPS_Val(addr, "INFO 6");
    strncpy(country, r, sizeof country);
  }
  //12.00V 66.67A
  void getPS_RateVoltage(int addr) {
    char *r = getPS_Val(addr, "RATE?");
    char b[20];
    strncpy(b, r, sizeof b);
    char *ptr = NULL;
    rate_voltage = -1;
    if ((ptr = strchr(b, ' '))) {
      ptr = strchr(b, 'V');
      *ptr = '\0';
      rate_voltage = int(atof(b) * 100);
    }
  }

  void getPS_RateCurrent(int addr) {
    char *r = getPS_Val(addr, "RATE?");
    char b[20];
    strncpy(b, r, sizeof b);
    char *ptr = NULL;
    char *ptr_2 = NULL;
    rate_current = -1;
    if ((ptr = strchr(b, ' '))) {
      ptr_2 = strchr(ptr, 'A');
      *ptr_2 = '\0';
      rate_current = int(atof(ptr + 1) * 100);
    }
  }

  void getPS_OnOff(int addr) {
    char *r = getPS_Val(addr, "POWER 2");
    switch (r[0]) {
      case '0': on_off = 0; break;
      case '1': on_off = 1; break;
      case '2': on_off = 0; break;
      case '3': on_off = 1; break;
    }
  }

  void getPS_MaxVoltage(int addr) {
    max_voltage = -1;
  }

  void getPS_MaxCurrent(int addr) {
    max_current = -1;
  }

  void getPS_OutVoltage(int addr) {
    char *r = getPS_Val(addr, "RV?");
    out_voltage = int(atof(r) * 100);
  }

  void getPS_OutCurrent(int addr) {
    char *r = getPS_Val(addr, "RI?");
    out_current = int(atof(r) * 100);
  }

  void getPS_Status0(int addr) {
    char *r = getPS_Val(addr, "STUS 0");
    status0 = (r[0] - '0') << 4;
    status0 += (r[1] - '0') & 0x0F;
  }

  void getPS_Status1(int addr) {
    char *r = getPS_Val(addr, "STUS 1");
    status1 = (r[0] - '0') << 4;
    status1 += (r[1] - '0') & 0x0F;
  }

  void getPS_Temp(int addr) {
    char *r = getPS_Val(addr, "RT?");
    temp = atoi(r);
  }

  void getPS_SetVoltage(int addr) {
    char *r = getPS_Val(addr, "SV?");
    set_voltage = int(atof(r) * 100);
  }

  void getPS_SetCurrent(int addr) {
    char *r = getPS_Val(addr, "SI?");
    set_current = int(atof(r) * 100);
  }

  void getPS_Control(int addr) {
  }

  void test_PS() {
    Serial.println("Start of test_PS");

    getPS_Manuf(ADDRESS);
    Serial.print("Manuf: ");
    if (!strlen(manuf)) strcpy(manuf, "UNKWN");
    Serial.println(manuf);
    delay(MYDELAY);

    getPS_Model(ADDRESS);
    Serial.print("Model: ");
    if (!strlen(model)) strcpy(manuf, "UNKWN");
    Serial.println(model);
    delay(MYDELAY);

    getPS_VoltageString(ADDRESS);
    Serial.print("VoltageSt: ");
    if (!strlen(voltage_string)) strcpy(manuf, "UNKWN");
    Serial.println(voltage_string);
    delay(MYDELAY);

    getPS_Revision(ADDRESS);
    Serial.print("Rev: ");
    if (!strlen(revision)) strcpy(manuf, "UNKWN");
    Serial.println(revision);
    delay(MYDELAY);

    getPS_ManufDate(ADDRESS);
    Serial.print("ManufDate: ");
    if (!strlen(manuf_date)) strcpy(manuf, "UNKWN");
    Serial.println(manuf_date);
    delay(MYDELAY);

    getPS_Serial(ADDRESS);
    Serial.print("Serial: ");
    if (!strlen(serial)) strcpy(manuf, "UNKWN");
    Serial.println(serial);
    delay(MYDELAY);

    getPS_Country(ADDRESS);
    Serial.print("Country: ");
    if (!strlen(country)) strcpy(manuf, "UNKWN");
    Serial.println(country);
    delay(MYDELAY);

    getPS_RateVoltage(ADDRESS);
    Serial.print("RateVoltage: ");
    if (rate_voltage < 0) Serial.println("UNKWN");
    else Serial.println(rate_voltage);
    delay(MYDELAY);

    getPS_RateCurrent(ADDRESS);
    Serial.print("RateCurrent: ");
    if (rate_current < 0) Serial.println("UNKWN");
    else Serial.println(rate_current);
    delay(MYDELAY);

    if (setPS_OnOff(ADDRESS, "ON")) Serial.println("Turned it on");
    else Serial.println("failed to turn it on");

    //if (setPS_Voltage(ADDRESS, 1000)) Serial.println("Set volts to 5.0");
    if (setPS_Voltage(ADDRESS, 500)) Serial.println("Set volts to 5.0");
    else Serial.println("failed to set volts");

    if (setPS_Current(ADDRESS, 0)) Serial.println("Set current to 5");
    else Serial.println("failed to set current");
  }
};

PSU test_PSU1;

void updatePumping(void) {
  // If ENC_SW high then enable blower and flashing of STATUS_LED
  if (digitalRead(ENC_SW)) {
    //Blower enable high
    digitalWrite(BLOWER_ENABLE, HIGH);
    pinMode(STATUS_LED, OUTPUT);
  } else {
    //Blower enable low and turn off STATUS_LED
    digitalWrite(BLOWER_ENABLE, LOW);
    pinMode(STATUS_LED, LOW);
  }
}  //end updatePumping

void init_LCD(void) {
  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
}  //end init_LCD

void splashLCD(void) {
  lcd.setCursor(0, 0);  //Column, row
                        //  lcd.print("Hello, world!");
  lcd.print(PROG_NAME);
  lcd.setCursor(0, 1);
  lcd.print(VERSION);
  lcd.setCursor(0, 2);
  lcd.print("Compiled at: ");
  lcd.setCursor(0, 3);
  lcd.print(F(__DATE__ " " __TIME__));
}  //end splashLCD

void updateMQTTmessage(void) {
  static unsigned long lastMillis = 0;
  // publish a message roughly every second.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    char message[80] = { "a1OEDCS_Factory_Test V1.2, V(A1)= " };  //The constant part of the message

    //float voltage = analogRead(ADCinPin) * 3.3 * (my_R1 + my_R2) / (1023 * my_R2);
    float voltage = analogRead(A1) * 3.3 / 1023;
    Serial.print("voltage= ");
    Serial.println(voltage);

    char char_val[80] = { (char)voltage };
    //    char messageValue[80] = {"99Volts"};
    //strcat(dest, src);
    //sprintf(char_array, "%f", float_num);
    sprintf(char_val, "%.2f", voltage);
    strcat(message, char_val);

    //    client.publish("/hello", "world");
    client.publish("3C61053DF08C_ALM", message);
    Serial.print("char_val= ");
    Serial.println(char_val);

  }  //updateMQTTmessage
}  //updateMQTTmessage

void setup() {
  //serial1Buffer.reserve(256);
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier

  init_LCD();
  splashLCD();

  //readUID_ComposeMAC();
  // pinMode(ETHERNET_CS, OUTPUT);  // make sure that the default chip select pin is set to output, even if you don't use it:
  // digitalWrite(ETHERNET_CS, HIGH);
  // pinMode(4, OUTPUT);  // On the Ethernet Shield, CS is pin 4
  // You can use Ethernet.init(pin) to configure the CS pin
  // Ethernet.init(ETHERNET_CS);  // Most Arduino shields
  Ethernet.begin(mac, ip);  //Hard code the MAC and the IP address
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("public.cloud.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();

  Serial1.begin(4800);
  while (!Serial1)
    ;

  pinMode(FAULT_LED, OUTPUT);  // On the Front Panel
  digitalWrite(FAULT_LED, HIGH);
  pinMode(STATUS_LED, OUTPUT);  // On the Front Panel
  digitalWrite(STATUS_LED, HIGH);

  pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(SSR3, OUTPUT);
  pinMode(BLOWER_ENABLE, OUTPUT);
  pinMode(ENC_SW, INPUT);                  // If low lets turn off the BLOWER_ENABLE (24V to blower.)
  digitalWrite(BLOWER_ENABLE, HIGH);       //Set high to enable blower power.
  analogWrite(nFAN1_PWM, SET_BLOWER_LOW);  // Set for low RPM
  //analogWrite(nFAN1_PWM, 128);  // Set for medium RPM
  pinMode(PS1_EN, OUTPUT);
  pinMode(PS2_EN, OUTPUT);
  digitalWrite(PS1_EN, HIGH);  //Set high to enable PS1
  digitalWrite(PS2_EN, HIGH);  //Set high to enable PS2



  pinMode(KEEP_ALIVE, OUTPUT);
  digitalWrite(KEEP_ALIVE, LOW);  // TUrn off Keep Alive.

  SENSE_24V.Update();   //Read A1 every two seconds.
  SENSE_12V.Update();   //Read A2 every two seconds.
  SENSE_AUX1.Update();  //Read A3 every two seconds.
  SENSE_AUX2.Update();  //Read A4 every two seconds.

  Serial.print("Start of test_PSU1: ");
  Serial.println(millis());
  //  test_PSU1.test_PS();  //run once to test psu

  test_PSU1.getPS_Manuf(0x00);  //Test for PS at default address.

  Serial.print("End of test_PSU1: ");
  Serial.println(millis());

  digitalWrite(FAULT_LED, LOW);
  digitalWrite(STATUS_LED, LOW);

}  //End setup()


void loop() {
  //MQTT
  client.loop();
  if (!client.connected()) {
    connect();
  }

  updateMQTTmessage();

  led0.Update();        //SSR1
  led1.Update();        //SSR2 cannot be used on systems with a stack
  led2.Update();        //SSR3 cannot be used on systems with a stack
  SENSE_24V.Update();   //Read A1 every two seconds.
  SENSE_12V.Update();   //Read A2 every two seconds.
  SENSE_AUX1.Update();  //Read A3 every two seconds.
  SENSE_AUX2.Update();  //Read A4 every two seconds.
  led3.Update();        //FAULT_LED blink
  led4.Update();        //STATUS_LED blink
  updateSHUTDOWN();     //Press switch and make blower go fast.
  UpdateEthernet();     //Check link status.
  updatePumping();      //Pumping/Standby switch,turn on blower and STATUS_LED

  //Check the AC input power by inference of the +24V.  March 2025 Redundant
  // if (!updatePowerMonitor()) {
  //    Serial.println("Bad power");
  //   ;
  // }


}  //end of loop()
