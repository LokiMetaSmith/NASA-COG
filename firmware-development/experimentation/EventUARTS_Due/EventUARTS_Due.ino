// EventUARTS-Due
// Inspired by code here: https://arduino-er.blogspot.com/2015/04/arduino-due-muilt-serial-and-serialevent.html
// Type stuff in the Serial Monitor (UART0) and it is forwared to UART1
// All other UART input is forwared out the Serial Monitor (UART0)

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "UART3Loopback"
#define VERSION ":V0.4"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1_Firmware:_"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


int LED = 13;
boolean LEDst = true;

#define SHUT_DOWN 49

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  
  Serial1.begin(4800);
  while (!Serial1);
  Serial2.begin(4800);
  Serial3.begin(9600);

  Serial.println("===================================");
  Serial.print(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier

  // Add a unique delimiter after initialization
  Serial.println("=== INITIALIZATION COMPLETE ===");

  pinMode(SHUT_DOWN, INPUT_PULLUP);
  digitalWrite(LED, LOW);

  // Add a flush after reset
  Serial1.flush();
  Serial1.println();
}



void loop() {

  //  while (Serial.available() > 0) {
  //    char a = Serial.read();
  //    Serial1.write(a); // First power supply
  //    Serial2.write(a); // Second power supply
  //    //Serial3.write(a);
  //  }

}

//UART0
void serialEvent() {
  while (Serial.available() > 0) {
    char a = Serial.read();

    if (digitalRead(SHUT_DOWN) == LOW) {
      Serial2.write(a);   //Stack supply 2 on J27
    } else {
      Serial1.write(a);   //Stack supply 1 on J10
    }

    ToggleLED();
    if (a == '\n') {
      Serial.print("Serial Terminal UART0 in at: ");
      Serial.println(millis());
    }
  }
}

//UART1
void serialEvent1() {
  while (Serial1.available() > 0) {
    char a = Serial1.read();
    Serial.write(a);
    ToggleLED();
    if (a == '\n') {
      Serial.print("Serial UART1 in at: ");
      Serial.println(millis());
    }
  }
}

//UART2
void serialEvent2() {
  while (Serial2.available() > 0) {
    char a = Serial2.read();
    Serial.write(a);
    ToggleLED();
    if (a == '\n') {
      Serial.print("Serial2 UART2 in at: ");
      Serial.println(millis());
    }
  }
}

//UART3
void serialEvent3() {
  while (Serial3.available() > 0) {
    char a = Serial3.read();
    Serial.write(a);
    ToggleLED();
    if (a == '\n') {
      Serial.print("Serial UART3 in at: ");
      Serial.println(millis());
    }
  }
}

void ToggleLED() {
  digitalWrite(LED, LEDst = !LEDst);
}
