//
// Author: Forrest Lee Erickson
// Date: 20241013
// LICENSE "GNU Affero General Public License, version 3 "



// Heart beat aka activity indicator LED.
//Set LED for Uno or ESP32 Dev Kit on board blue LED.
//Wink the LED
void wink(void) {
  //const int LED_BUILTIN = 2;    // ESP32 Kit//const int LED_BUILTIN = 13;    //Not really needed for Arduino UNO it is defined in library
  //const int LED_BUILTIN = 13;    // ESP32 Kit//const int LED_BUILTIN = 13;    //Not really needed for Arduino UNO it is defined in library
  const int HIGH_TIME_LED = 900;
  const int LOW_TIME_LED = 100;
  static unsigned long lastLEDtime = 0;
  static unsigned long nextLEDchange = 100; //time in ms.
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(LED_BUILTIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}//end LED wink


//Wink the FAULT
void winkFault(void) {
  // FAULT_LED_FRONT_PANEL
  const int HIGH_TIME_LED = 800;
  const int LOW_TIME_LED = 200;
  static unsigned long lastLEDtime = 0;
  static unsigned long nextLEDchange = 100; //time in ms.
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(FAULT_LED_FRONT_PANEL) == LOW) {
      digitalWrite(FAULT_LED_FRONT_PANEL, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(FAULT_LED_FRONT_PANEL, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}//end FAULT wink


//Wink the STATUS_LED_FRONT_PANEL LED
void winkStatus(void) {
  // FAULT_LED_FRONT_PANEL
  const int HIGH_TIME_LED = 700;
  const int LOW_TIME_LED = 300;
  static unsigned long lastLEDtime = 0;
  static unsigned long nextLEDchange = 100; //time in ms.
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(STATUS_LED_FRONT_PANEL) == LOW) {
      digitalWrite(STATUS_LED_FRONT_PANEL, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(STATUS_LED_FRONT_PANEL, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}//end STATUS LED wink


//Wink the STATUS_LED_FRONT_PANEL LED
void winkBattery(void) {
  // FAULT_LED_FRONT_PANEL
  const int HIGH_TIME_LED = 500;
  const int LOW_TIME_LED = 500;
  static unsigned long lastLEDtime = 0;
  static unsigned long nextLEDchange = 500; //time in ms.
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(KEEP_ALIVE_BATTERY) == LOW) {
      digitalWrite(KEEP_ALIVE_BATTERY, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(KEEP_ALIVE_BATTERY, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}//end KEEP_ALIVE_BATTERY wink

