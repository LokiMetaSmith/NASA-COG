/*
   File: Dallas_Tester.ino modified
   By: (Forrest) Lee Erickson
   Date: 20230921
   Use the serial plotter to view results
   This program forked from Dallas_Tester.ino on 20230921
   Set D9 PWM for about 50% to test PWM on MOM (Mock up of Maryville hardware)
   Add an EMA filtered output.
   Rev 0.5 Correct numberOfDevices in the legend of plot.
   Rev 0.5 REM out the filtered output on 202040711 
*/
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "MAX31850_Tester"
#define VERSION ";_Rev_0.5"
#define DEVICE_UNDER_TEST "Hardware: Mockup Of Maryville"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#define SHUT_DOWN 49


#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
//#define ONE_WIRE_BUS 5
#define ONE_WIRE_BUS 5  // But port 5 on Controller v1.

#define TEMPERATURE_PRECISION 9 // Lower resolution

#define MAX_NUM_THERMOCOUPLES 10 // An arbitrary maximum mumber of thermocouples.
float now_Temp[MAX_NUM_THERMOCOUPLES]; //
float ema_Temp[MAX_NUM_THERMOCOUPLES]; //

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

#define nFAN1_PWM 9 // The pin D9 for driving the Blower.

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return (-127);
  }
  //  Serial.print("Temp C: ");
  //  Serial.print(tempC);
  return (tempC);
  //  Serial.print(" Temp F: ");
  //  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void setup(void)
{
  // start serial port
  
  Serial.begin(115200);
  Serial.flush();
  delay(500);
  Serial.flush();
  delay(500);
  Serial.flush();
  delay(500);
  Serial.flush();

  // Start up the library
  sensors.begin();
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  //Report the Number of devices found
//  Serial.print("Number of devices found = ");
//  Serial.print(numberOfDevices);
//  Serial.print(", ");
  //Serial.print("TC0, TC1, ");
  //for (int i = 0; i <= 255; i++)
  for (int i = 0; i < numberOfDevices; i++) {
    Serial.print("TC");
    Serial.print(i);
    Serial.print(", ");
  }
  Serial.print(PROG_NAME);
  Serial.println(VERSION);

  pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  //Signal start of setup.

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address

    //    if (sensors.getAddress(tempDeviceAddress, i))
    sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

    if (false)
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();

      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
    //  sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
    } else {
      //      Serial.print("Found ghost device at ");
      //      Serial.print(i, DEC);
      //      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

  //  analogWrite(nFAN1_PWM, 127);  // Set for 50%
  analogWrite(nFAN1_PWM, 200);  // Set for low RPM


  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(500);
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(500);
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(500);

  //Initilize the EMA
  for (int k = 0; k < 2; k++) {
    sensors.requestTemperatures(); // Send the command to get temperatures
    delay(500);
    for (int i = 0; i < numberOfDevices; i++) {
      if (sensors.getAddress(tempDeviceAddress, i)) {

        ema_Temp[i] = printTemperature(tempDeviceAddress); // Use a simple function to print out the data
        //ema_Temp[i] = 23.5;
      }
    }
  }//end of k loop

  digitalWrite(LED_BUILTIN, LOW);   //Signal end of setup.
}// end setup()



void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //  Serial.println("DONE");
  digitalWrite(LED_BUILTIN, HIGH);   //Signal end of temp request.

  float ALPHA = 0.05; //Moveing average

  // Loop through each device, print out temperature data
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      // Output the device ID
      //		Serial.print("Temperature for device: ");
      //		Serial.print(i,DEC);

      // It responds almost immediately. Let's print out the data
      now_Temp[i] = printTemperature(tempDeviceAddress); // Use a simple function to print out the data
      Serial.print(now_Temp[i]);
      Serial.print(", ");
      
      // Print a filtered measurement
//      if (digitalRead(SHUT_DOWN) == LOW) {  //Press button for ten times longer filtering
//        ema_Temp[i] = ((1 - (ALPHA / 10.0)) * ema_Temp[i]) + ((ALPHA / 10.0) * now_Temp[i]) ; // EMA See: https://en.wikipedia.org/wiki/Exponential_smoothing
//      } else {
//        ema_Temp[i] = ((1 - ALPHA) * ema_Temp[i]) + (ALPHA * now_Temp[i]) ; // EMA See: https://en.wikipedia.org/wiki/Exponential_smoothing
//      }
//      Serial.print(ema_Temp[i]);
//      //Serial.print(tempC);
//      Serial.print(", ");

    }
    //else ghost device! Check your power requirements and cabling
  }
  Serial.println(); //end of line
  digitalWrite(LED_BUILTIN, LOW);   //Signal end of print temp.

}//end loop()

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
