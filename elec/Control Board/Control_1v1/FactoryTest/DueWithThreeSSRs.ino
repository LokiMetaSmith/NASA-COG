/* Program: DueWithThreeSSRs
 Tests the three SSR drive circuits on the Control V1.1 assembly

 Setup:
 Connect an LED with series resistor at J13, J30 and J31. 
 Pin 1 is positive and Pin 2 is ground. 
 Series resistor should limit current from +24V for LED.
  SSR1 and SSR2 flash independently.
  Press switch S2, "SHUT DOWN" to turn on the SSR3 LED.
*/


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

Flasher led0(13, 100, 400);      //Pins for Control V1.1
Flasher led1(51, 100, 400);      //Pins for Control V1.1
Flasher led2(52, 350, 350);
//Flasher led3(53, 150, 350);

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("DueWithThreeSSRs");
  pinMode(49, INPUT_PULLUP);
  pinMode(53, OUTPUT);
}

void loop() {

  led0.Update();
  led1.Update();
  led2.Update();
  if (digitalRead(49) == LOW){
    Serial.println("Button pressed");
    digitalWrite(53, LOW);
  }else{
    Serial.println("Button Open");
    digitalWrite(53, HIGH);
  }
  //led3.Update();
}
