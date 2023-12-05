/* Program: DueWithThreeSSRs
  Tests the three SSR drive circuits on the Control V1.1 assembly

  Setup:
  Connect an LED with series resistor at J13, J30 and J31.
  Pin 1 is positive and Pin 2 is ground.
  Series resistor should limit current from +24V for LED.
  SSR1 and SSR2 flash independently.
  Press switch S2, "SHUT DOWN" to turn on the SSR3 LED.
*/

#include <RotaryEncoder.h>

#define BAUD_RATE 115200

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

//Control V1.1 signal pin names
#define SSR1 51
#define SSR2 52
#define SSR3 53
#define SHUT_DOWN 49
#define LED_RED 43
#define LED_BLUE 44
#define LED_GREEN 45
#define BEEPER 50   //A buzzer.

//Control V1.1 hardware
//Rotary Encoder on RepRap
#define PIN_IN1 40
#define PIN_IN2 41
#define ENC_SW 42   //A switch

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);


Flasher led0(13, 100, 400);      //Pins for Control V1.1
Flasher led1(SSR1, 100, 400);    //Pins for Control V1.1
Flasher led2(SSR2, 350, 350);
//Flasher led3(SSR3, 150, 350);

void updateSHUTDOWN(){
    if (digitalRead(49) == LOW) {
    Serial.println("Shutdown button pressed");
    digitalWrite(SSR3, LOW);
    digitalWrite(BEEPER, !digitalRead(BEEPER));  //Make some sound
  } else {
//    Serial.println("Button Open");
    digitalWrite(SSR3, HIGH);   
  }
}//end update shutdown button

//Check for encoder button pressed and return true
bool updateENC_BUT(){
    if (digitalRead(ENC_SW) == LOW) {
    Serial.println("Button");
    digitalWrite(SSR3, LOW);
    digitalWrite(BEEPER, !digitalRead(BEEPER));
    return true; //Reset the position
  } else {
//    Serial.println("Button Open");
    digitalWrite(SSR3, HIGH);
    return false;
  }
}//end update shutdown button

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.println("DueWithThreeSSRs");
  pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(SSR3, OUTPUT);
  pinMode(BEEPER, OUTPUT);
}

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

  updateSHUTDOWN(); //Check for press of switch
 
  if (updateENC_BUT()){ //Check encoder, zero if button pressed
    pos =0;
    encoder.setPosition(0);
//    Serial.println("Reset position.");
  }
}
