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
//#define PROG_NAME "DueWithThreeSSRs"
#define VERSION ";_Rev_0.2"                         //Enable power supply test during setup()
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.2"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200
//
#include <SPI.h>
#include <Ethernet.h>

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
      previousMillis = currentMillis;                                           // Remember the time
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
    digitalWrite(ETHERNET_CS, LOW);  // select ethernet mode
    link_status = Ethernet.linkStatus();
    //      delay(1000);  // Hold the splash screen a second
    auto link = Ethernet.linkStatus();
    //      delay(1000);  // Hold the splash screen a second
    digitalWrite(ETHERNET_CS, HIGH);  // deselect ethernet mode
    Serial.print("Link status: ");
    switch (link_status) {
      case Unknown:
        Serial.println("Unknown");
        break;
      case LinkON:
        Serial.println("ON");
        break;
      case LinkOFF:
        Serial.println("OFF");
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
#define nFAN1_PWM 9       // The pin D9 for driving the Blower.
#define BLOWER_ENABLE 22  // The pin D22 for Enable 24V to the Blower.

// Programable Power Supply Enable
#define PS1_EN 23
#define PS2_EN 8


//Flasher to exercise the SSRs pins and the Building LED.
Flasher led0(13, 100, 400);    //Pins for Control V1.1
Flasher led1(SSR1, 100, 400);  //Pins for Control V1.1
Flasher led2(SSR2, 350, 350);
//Flasher led3(SSR3, 150, 350);

//Tests for press of switch, "SHUT DOWN". Turns OFF the SSR3 LED
void updateSHUTDOWN() {
  if (digitalRead(SHUT_DOWN) == LOW) {
    Serial.println("Shutdown button pressed");
    digitalWrite(SSR3, LOW);
    analogWrite(nFAN1_PWM, SET_BLOWER_HIGH);  // Set for high
    //analogWrite(nFAN1_PWM, 20);  // Set for high
    delay(500);
  } else {
    digitalWrite(SSR3, HIGH);
    analogWrite(nFAN1_PWM, SET_BLOWER_LOW);  // Set for low
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
  int lowThreshold24V = (24 * (24700 / (10000 + 24700)) / 3.3) * 1023 * 3/4;  //1023 * 3 / 4;
  const long POWER_MONITOR_TIME = 2000;
  static long previousPowerMillis = 0;

  long currentMillis = millis();

  if (((currentMillis - previousPowerMillis) >= POWER_MONITOR_TIME) || (currentMillis < previousPowerMillis)) {
    previousPowerMillis = currentMillis;
    if (analogRead(A1) > lowThreshold24V) {
      Serial.println("24V power OK");
      powerIsGood = true;
      return true;
    } else {
      Serial.println("Bad 24V power");
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

void setup() {
  //serial1Buffer.reserve(256);

  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier

  Serial1.begin(4800);
  while (!Serial1)
    ;

  pinMode(ETHERNET_CS, OUTPUT);  // make sure that the default chip select pin is set to output, even if you don't use it:
  digitalWrite(ETHERNET_CS, HIGH);
  pinMode(4, OUTPUT);  // On the Ethernet Shield, CS is pin 4

  pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(SSR3, OUTPUT);
  pinMode(BLOWER_ENABLE, OUTPUT);
  digitalWrite(BLOWER_ENABLE, HIGH);       //Set high to enable blower power.
  analogWrite(nFAN1_PWM, SET_BLOWER_LOW);  // Set for low RPM
  //analogWrite(nFAN1_PWM, 128);  // Set for medium RPM
  pinMode(PS1_EN, OUTPUT);
  pinMode(PS2_EN, OUTPUT);
  digitalWrite(PS1_EN, HIGH);  //Set high to enable PS1
  digitalWrite(PS2_EN, HIGH);  //Set high to enable PS2
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(ETHERNET_CS);  // Most Arduino shields

  SENSE_24V.Update();   //Read A1 every two seconds.
  SENSE_12V.Update();   //Read A2 every two seconds.
  SENSE_AUX1.Update();  //Read A3 every two seconds.
  SENSE_AUX2.Update();  //Read A4 every two seconds.

  Serial.print("Start of test_PSU1: ");
  Serial.println(millis());
  //  test_PSU1.test_PS();  //run once to test psu
  Serial.print("End of test_PSU1: ");
  Serial.println(millis());
}  //End setup()

void loop() {
  static int pos = 0;

  led0.Update();
  led1.Update();  //cannot be used on systems with a stack
  led2.Update();  //cannot be used on systems with a stack
  //led3.Update();  //Does not work on Due hardware.
  SENSE_24V.Update();   //Read A1 every two seconds.
  SENSE_12V.Update();   //Read A2 every two seconds.
  SENSE_AUX1.Update();  //Read A3 every two seconds.
  SENSE_AUX2.Update();  //Read A4 every two seconds.

  updateSHUTDOWN();  //Check for press of switch

  UpdateEthernet();

  if (!updatePowerMonitor()) {
//    Serial.println("Bad power");
    ;
  }

}  //end of loop()
