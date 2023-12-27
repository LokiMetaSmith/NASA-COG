// DueTach
// This program extracted from the OEDCS 3.11 to try to replicate oscilating tac readings observed on 21 Dec on Mule.
//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
//namespace tach_data {

// This may seem odd, but you can use two fans in serious
// for redundancy or great flow against pressure.

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DueTach"
#define VERSION ";_Rev_0.5"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#define BAUD_RATE 115200

#define nFAN1_PWM 9 // The pin D9 for driving the Blower.
#define BLOWER_ENABLE 22 // The pin D22 for Enable 24V to the Blower.

#define NUMBER_OF_FANS 1

#define PERIOD 1000

int g_loopdelay = 1;

unsigned long volatile tach_data_ts[NUMBER_OF_FANS];
unsigned long volatile tach_data_cnt[NUMBER_OF_FANS];
unsigned long volatile tach_data_ocnt[NUMBER_OF_FANS];
unsigned long volatile tach_data_duration[NUMBER_OF_FANS];

int PWM_PIN[NUMBER_OF_FANS];
int TACH_PIN[NUMBER_OF_FANS];

uint8_t fan_Enable;

void tachISR(uint8_t i) {
  tach_data_cnt[i]++;
  //    refresh_tach_data(i);
}
void tachISR0() {
  tachISR(0);
};
void tachISR1() {
  tachISR(1);
};
void tachISR2() {
  tachISR(2);
};
void tachISR3() {
  tachISR(3);
};

void refresh_tach_data(uint8_t i) {
  unsigned long m = millis();
  if (tach_data_ts[i] + PERIOD < m) {
//    noInterrupts();
    tach_data_ocnt[i] = tach_data_cnt[i];
    tach_data_duration[i] = m - tach_data_ts[i];
    tach_data_ts[i] = m;
    tach_data_cnt[i] = 0;
//    interrupts();

//    Serial.print(tach_data_duration[i]);
//    Serial.print(", ");
//    Serial.print(tach_data_ocnt[i] * 30);
//    Serial.print(", ");

    //Calculate RPM like in OEDCS but div by 2.0
    float num_revolutions = (float) tach_data_ocnt[i] / 2.0;
    float one_revolution_time_ms = (float) tach_data_duration[i];
    long rpm = (long) (60000.0 * ( num_revolutions / one_revolution_time_ms));
    Serial.print(tach_data_duration[i]);
    Serial.print(", ");
    Serial.println(rpm);

  }
}


//For serial input and user interface
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool autoIncerment = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Serial.println();

  Serial.print("Period_ms, RPM, ");
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  //  Serial.print("Compiled at: ");
  //  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier



  //We need to enable the blower if we are to have any tac to count
  pinMode(BLOWER_ENABLE, OUTPUT);
  digitalWrite(BLOWER_ENABLE, HIGH); //Set high to enable blower power.
  analogWrite(nFAN1_PWM, 200);  // Set for low RPM

  PWM_PIN[0] = 9;
  TACH_PIN[0] = A0;
  fan_Enable = 22;

  attachInterrupt(digitalPinToInterrupt(TACH_PIN[0]), tachISR0, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSerialInput();
  refresh_tach_data(0);  //Prints the duration of the tac count and the count*30=RPM
  //  updateBlowerSpeed();
  updateCompetingFunction();

}//end loop()


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
//void serialEvent1() {
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}//end serialEvent1()


void updateSerialInput(void) {
  // Get user input, a string when a newline arrives:
  //Manages the state of auto incrementing.
  if (stringComplete) {
    //    Serial.println("Got a new string");
    //    Serial.println(inputString);  //For debuging string input.

    //    if (inputString == "L\n") {
    //      Serial.println("Set Linearize the fan");
    //    }//Test for "L"
    //    if (inputString == "l\n") {
    //      Serial.println("Clear Linearize the fan");
    //    }//Test for "l"

    if (inputString.toInt() < 0) {
      autoIncerment = false; // set for
      //      Serial.println("Set auto increment false");
    }
    if (inputString.toInt() > 9999) {
      autoIncerment = true; // set for
      //      Serial.println("Set auto increment true");
    } else {
      //      updatelinearFanPWM(inputString);
      updateLoopDelay(inputString.toInt()); //Make a delay.
    }
    inputString = "";
    stringComplete = false;
  }//end processing string.
}//end updateSerialInput

void updateLoopDelay(int myLoopdelay) {
  g_loopdelay = myLoopdelay;
  Serial.print("Period_ms, RPM, ");
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print(", New_loop_delay");
  Serial.print(g_loopdelay);
}//end updateLoopDelay()

//A function to hog time in the main loop.
void updateCompetingFunction() {
  delay(g_loopdelay);
}
