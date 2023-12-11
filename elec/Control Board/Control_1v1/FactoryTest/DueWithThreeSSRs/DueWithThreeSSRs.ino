
/* Program: DueWithThreeSSRs
  Tests the three SSR drive circuits on the Control V1.1 assembly
  Tests the SHUT DOWN switch
  Tests BigTreeTech MINI 12864 Rotary Encoder and switch

  Setup:
  Connect an LED with series resistor at J13, J30 and J31.
  Pin 1 is positive and Pin 2 is ground.
  Series resistor should limit current from +24V for LED.
  SSR1 and SSR2 flash independently.
  Press switch S2, "SHUT DOWN" to turn on the SSR3 LED.
  Press the BigTreeTech MINI 12864 Rotary Encoder switch and hear buzzer
  Rotate the BigTreeTech MINI 12864 Rotary Encoder and see the text message about position and direction.
*/

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "OEDCS Factory Test"
#define VERSION ";_Rev_0.3"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

#define BAUD_RATE 115200

#include <RotaryEncoder.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

const int NUMPIXELS = 3;
Adafruit_NeoPixel pixels(NUMPIXELS, 43, NEO_RGB + NEO_KHZ400);

// OLED Display
U8G2_ST7567_JLX12864_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 48, /* dc=*/ 47, /* reset=*/ 46); //Rotation 180


//Check power supplies. Reports status on serial port, OLED display.
class PowerSense
{
    // Class Member Variables
    // These are initialized at startup
    int ADCinPin;    // the number of the ADC pin
    long ReadPeriod;  // milliseconds of on-time
    unsigned long previousMillis;   // will store last time ADC was read
    String my_pinName;
    float my_R1;
    float my_R2;
    float voltage;
    int my_offsetX = 0;
    int my_offsetY = 41; //Benieth center

    // Constructor - creates a Flasher
    // and initializes the member variables and state
  public:
    PowerSense(const String pinName, int pin, long period, float R1 = 30000, float R2 = 4700, int offsetX = 0, int offsetY = 0)
    {
      ADCinPin = pin;
      previousMillis = 0;
      ReadPeriod = period;
      my_pinName = pinName;
      my_R1 = R1;
      my_R2 = R2;
      voltage = 0;
      my_offsetX = offsetX;
      my_offsetY = offsetY;
    }

    void Update()
    {
      // check to see if it's time to change the state of the LED
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= ReadPeriod)
      {
        previousMillis = currentMillis;  // Remember the time
        voltage = analogRead(ADCinPin) * 3.3 * (my_R1 + my_R2) / (1023 * my_R2); // RAW Read of the ADC
        Serial.print(my_pinName);  //
        Serial.print(": ");  //
        Serial.println(voltage);  // RAW Read of the ADC
        //Update OLED display
        u8g2.setFont(u8g2_font_6x10_mf); //Not transparent font
        u8g2.setFontMode(0);
        u8g2.setCursor(my_offsetX, my_offsetY);
        u8g2.print("               ");
        u8g2.sendBuffer();
        u8g2.setCursor(my_offsetX, my_offsetY);
        u8g2.print(my_pinName);
        u8g2.print(voltage);
        u8g2.sendBuffer();
      }
    }
};//end PowersSense

class Flasher
{
    // Class Member Variables
    // These are initialized at startup
    int ledPin;      // the number of the LED pin
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time

    // These maintain the current state
    int ledState;                 // ledState used to set the LED
    unsigned long previousMillis;   // will store last time LED was updated

    // Constructor - creates a Flasher
    // and initializes the member variables and state
  public:
    Flasher(int pin, long on, long off)
    {
      ledPin = pin;
      pinMode(ledPin, OUTPUT);

      OnTime = on;
      OffTime = off;

      ledState = LOW;
      previousMillis = 0;
    }

    void Update()
    {
      // check to see if it's time to change the state of the LED
      unsigned long currentMillis = millis();

      if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;  // Remember the time
        digitalWrite(ledPin, ledState);  // Update the actual LED
      }
      else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;   // Remember the time
        digitalWrite(ledPin, ledState);   // Update the actual LED
      }
    }
};

// Resistive dividers Vin = Vadc*3.3/1032 *(R1+R1)/R2
//Read every two seconds
//              Signal name, Pin number, R1, R2, Xoffset, Yoffset
PowerSense SENSE_24V("+24V ", 1, 2000, 30000, 4700, 0, 50); //Read A1. R101+R105+R106, R102.
PowerSense SENSE_12V("+12V ", 2, 2000, 30000, 10000, 64, 50); //Read A2. R103+R107+R108, R104.
PowerSense SENSE_AUX1("AUX1 ", 3, 2000, 10000, 14700, 0, 60); //Read A3. R123, R124+R125.
PowerSense SENSE_AUX2("AUX2 ", 6, 2000, 10000, 14700, 64, 60); //Read A6 R126, R127+R128.

//Control V1.1 signal pin names
#define SSR1 51
#define SSR2 52
#define SSR3 53
#define SHUT_DOWN 49
#define LED_RED 43
#define LED_BLUE 44
#define LED_GREEN 45
#define BEEPER 50   //A buzzer.
#define nFAN1_PWM 9 // The pin D9 for driving the Blower.
#define BLOWER_ENABLE 22 // The pin D22 for Enable 24V to the Blower.

//Rotary Encoder on BigTreeTech MINI 12864
#define PIN_IN1 40
#define PIN_IN2 41
#define ENC_SW 42   //A switch

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);


//Flasher to exercise the SSRs pins and the Building LED.
Flasher led0(13, 100, 400);      //Pins for Control V1.1
Flasher led1(SSR1, 100, 400);    //Pins for Control V1.1
Flasher led2(SSR2, 350, 350);
//Flasher led3(SSR3, 150, 350);

//Tests for press of switch, "SHUT DOWN". Buzzes on BigTreeTech MINI 12864
void updateSHUTDOWN() {
  if (digitalRead(49) == LOW) {
    Serial.println("Shutdown button pressed");
    digitalWrite(SSR3, LOW);
    digitalWrite(BEEPER, !digitalRead(BEEPER));  //Make some sound
  } else {
    digitalWrite(SSR3, HIGH);
  }
}//end update shutdown button

//Check for encoder button pressed and return true
bool updateENC_BUT() {
  if (digitalRead(ENC_SW) == LOW) {
    Serial.println("Button");
    digitalWrite(SSR3, LOW);
    digitalWrite(BEEPER, !digitalRead(BEEPER));  //Make some sound
    return true; //Reset the position
  } else {
    digitalWrite(SSR3, HIGH);
    return false;
  }
}//end update shutdown button


bool updatePowerMonitor(void) {
  //Analog read of the +24V expected about 3.25V at ADC input.
  // SENSE_24V on A1.
  // Full scale is 1023, ten bits for 3.3V.
  //30K into 4K7
  const long R1 = 30000;
  const long R2 = 4700;
  const float Vcc = 3.3;
  bool powerIsGood = false;
  int lowThreshold24V = 1023 * 3 / 4;

  if (analogRead(A1) > lowThreshold24V) {
    powerIsGood = true;
    return true;
  } else {
    powerIsGood = false;
    return false;
  }
}


void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.println("DueWithThreeSSRs");
  pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(SSR3, OUTPUT);
  pinMode(BEEPER, OUTPUT);
  pinMode(BLOWER_ENABLE, OUTPUT);
  digitalWrite(BLOWER_ENABLE, HIGH); //Set high to enable blower power.
  analogWrite(nFAN1_PWM, 200);  // Set for low RPM

  setupBacklights(); //Setup the neopixels
  setupu8g2(); //Setup the graphics display
  delay(1000);  // Hold the splash screen a second
}//End setup()

void loop() {
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = newPos;
  } // if

  led0.Update();
  led1.Update();
  led2.Update();
  //led3.Update();  //Does not work on Due hardware.
  SENSE_24V.Update(); //Read A1 every two seconds.
  SENSE_12V.Update(); //Read A2 every two seconds.
  SENSE_AUX1.Update(); //Read A3 every two seconds.
  SENSE_AUX2.Update(); //Read A4 every two seconds.

  updateSHUTDOWN(); //Check for press of switch

  if (updateENC_BUT()) { //Check encoder, zero if button pressed
    pos = 0;
    encoder.setPosition(0);    
  }

  if (!updatePowerMonitor()) {
    ;
  }

}//end of loop()
