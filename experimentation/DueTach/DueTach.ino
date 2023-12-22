// DueTach
// This program extracted from the OEDCS 3.11 to try to replicate oscilating tac readings observed on 21 Dec on Mule.
//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
//namespace tach_data {

// This may seem odd, but you can use two fans in serious
// for redundancy or great flow against pressure.

#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DueTach"
#define VERSION ";_Rev_0.1"
#define DEVICE_UNDER_TEST "Hardware:_Control_V1.1"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#define BAUD_RATE 115200

#define nFAN1_PWM 9 // The pin D9 for driving the Blower.
#define BLOWER_ENABLE 22 // The pin D22 for Enable 24V to the Blower.

#define NUMBER_OF_FANS 1

#define PERIOD 1000

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
    tach_data_ocnt[i] = tach_data_cnt[i];
    tach_data_duration[i] = m - tach_data_ts[i];
    tach_data_ts[i] = m;
    tach_data_cnt[i] = 0;
    Serial.print(tach_data_duration[i]);
    Serial.print(", ");
    Serial.println(tach_data_ocnt[i]*30);
  }
}
//}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Serial.println();
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier

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

  refresh_tach_data(0);  //Prints the duration of the tac count and the count*30=RPM
//  updateBlowerSpeed();

}
