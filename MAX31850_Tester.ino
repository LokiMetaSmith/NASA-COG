/*
   File: Dallas_Tester.ino modified
   By: (Forrest) Lee Erickson
   Date: 20230921
   Use the serial plotter to view results
   This program forked from Dallas_Tester.ino on 20230921
*/
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "MAX31850_Tester"
#define VERSION ";_Rev_0.1"
#define DEVICE_UNDER_TEST "Hardware: Mockup Of Maryville"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
//#define ONE_WIRE_BUS 5
#define ONE_WIRE_BUS 5  // But port 5 on Controller v1.

#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  delay(500);
  Serial.print("TC0, TC1, ");
  Serial.print(PROG_NAME);
  Serial.println(VERSION);

  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

//Print some status
//  // locate devices on the bus
//  Serial.print("Locating devices...");
//  Serial.print("Found ");
//  Serial.print(numberOfDevices, DEC);
//  Serial.println(" devices.");
//
//  // report parasite power requirements
//  Serial.print("Parasite power is: ");
//  if (sensors.isParasitePowerMode()) Serial.println("ON");
//  else Serial.println("OFF");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address

//    if (sensors.getAddress(tempDeviceAddress, i))

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
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
    } else {
//      Serial.print("Found ghost device at ");
//      Serial.print(i, DEC);
//      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
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
    return;
  }
  //  Serial.print("Temp C: ");
  Serial.print(tempC);
  //  Serial.print(" Temp F: ");
  //  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //  Serial.println("DONE");


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
      printTemperature(tempDeviceAddress); // Use a simple function to print out the data
      Serial.print(", ");

    }
    //else ghost device! Check your power requirements and cabling
  }
  Serial.println(); //end of line

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
