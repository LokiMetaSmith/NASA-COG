// Control1_2Panel
// For testing the OEDCS front panel features new with Control V1.2
// Toggles Status LED, Fault LED, Battery 
// Poles O2_Switch_FRONT_PANEL the O2_Switch_FRONT_PANEL switch and reports status on Serial Monitor port
// On serial input poles the SHUT_DOWN switch and reports status on Serial Monitor port

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "Control1_2Panel"
#define VERSION ":V0.1"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.2_Firmware:_"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

int LED = 13;
boolean LEDst = true;

#define SHUT_DOWN 49

//Front Panel Parts
/*
LokiMetaSmith
on Sep 21, 2024
Author
@RobertLRead please assign and utilize
D43 for Status LED (GPIO HIGH, light is on)
D44 for Fault LED (GPIO HIGH, light is on)
D45 for Keep Alive (GPIO HIGH, Battery engaged, GPIO LOW, battery disengaged; when GPIO LOW unit will shut off when front power switch is toggled)
*/


#define O2_Switch_FRONT_PANEL 42  //O2 On/Standby Make Input with pullup
#define STATUS_LED_FRONT_PANEL 43
#define FAULT_LED_FRONT_PANEL 44
//Battery Relay
#define KEEP_ALIVE_BATTERY 45  //High for Batter Connect


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("===================================");
  Serial.print(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier

  pinMode(SHUT_DOWN, INPUT_PULLUP);

  //Front Panel Parts
  pinMode(O2_Switch_FRONT_PANEL, INPUT_PULLUP);
  pinMode(STATUS_LED_FRONT_PANEL, OUTPUT);
  pinMode(FAULT_LED_FRONT_PANEL, OUTPUT);
  //Battery Relay
  pinMode(KEEP_ALIVE_BATTERY, OUTPUT);

  //Flash LEDs on and Battery connection on.
  digitalWrite(STATUS_LED_FRONT_PANEL, HIGH);
  digitalWrite(FAULT_LED_FRONT_PANEL, HIGH);
  digitalWrite(KEEP_ALIVE_BATTERY, HIGH);  //Make relay click on

  delay(500);

  //Set LEDs on and Battery connection off.
  digitalWrite(STATUS_LED_FRONT_PANEL, LOW);
  digitalWrite(FAULT_LED_FRONT_PANEL, LOW);
  digitalWrite(KEEP_ALIVE_BATTERY, LOW);  //Make relay click off

  // Add a unique delimiter after initialization
  Serial.println("=== INITIALIZATION COMPLETE ===");
  Serial.println("");

  digitalWrite(LED_BUILTIN, LOW);
}// end setup()



void loop() {

  wink();
  winkFault();
  winkStatus();
  poleO2Switch();
  winkBattery();

}// end loop()

//UART0
void serialEvent() {
  while (Serial.available() > 0) {
    char a = Serial.read();

    if (digitalRead(SHUT_DOWN) == LOW) {
      //Serial2.write(a);   //Stack supply 2 on J27
      Serial.println("Shutdown pressed.");
    } else {
      //Serial1.write(a);   //Stack supply 1 on J10
      Serial.println("Shutdown NOT pressed.");
    }

    ToggleLED();
    if (a == '\n') {
      Serial.print("Serial Terminal UART0 in at: ");
      Serial.println(millis());
    }
  }
}


void ToggleLED() {
  digitalWrite(LED, LEDst = !LEDst);
}


//Pole the O2_Switch_FRONT_PANEL switch
void poleO2Switch(void) {
  //O2_Switch_FRONT_PANEL
  static unsigned long lastPoleTime = 0;
  static unsigned long nextPoleTime = 1000;  //time in ms.

  if (((millis() - lastPoleTime) > nextPoleTime) || (millis() < lastPoleTime)) {
    if (digitalRead(O2_Switch_FRONT_PANEL) == LOW) {
      //Serial2.write(a);   //Stack supply 2 on J27
      Serial.println("O2_Switch_FRONT_PANEL LOW.");
    } else {
      //Serial1.write(a);   //Stack supply 1 on J10
      Serial.println("O2_Switch_FRONT_PANEL HIGH.");
    }
    lastPoleTime = millis();
  }
}  //end LED wink
