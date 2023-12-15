// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any 4ater version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifdef ARDUINO
#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif
#include <MAX31850.h>

#include <core.h>
#include <machine.h>


#define TEMPERATURE_PRECISION 9

namespace Temperature {

  // TODO: Turn this into a Loop to support any number of thermocouples.
  // Print out an ordering so we can match ID against index
  // arrays to hold device addresses
  DeviceAddress postHeaterThermometer;
  DeviceAddress postGetterThermometer;
  DeviceAddress postStackThermometer;

  MAX31850Temperature::MAX31850Temperature() {
    oneWire.begin(MAX31850_DATA_PIN);
    // Pass our oneWire reference to Dallas Temperature.

    sensors.setOneWire(&oneWire);

      // Start up the library
  sensors.begin();

  // locate devices on the bus
  CogCore::Debug<const char *>("Locating devices...");
  CogCore::Debug<const char *>("Found ");
  CogCore::Debug<uint32_t>(sensors.getDeviceCount());
  CogCore::Debug<const char *>(" devices.\n");

  // report parasite power requirements
  CogCore::Debug<const char *>("Parasite power is: ");
  if (sensors.isParasitePowerMode()) {
    CogCore::Debug<const char *>("ON\n");
  } else {
    CogCore::Debug<const char *>("OFF\n");
  }

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // postHeaterThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  // postGetterThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
#ifndef ALLOW_BAD_THERMOCOUPLES_FOR_TESTING
  const bool ALLOW_BAD_THERMOCOUPLES = false;
#else
  const bool ALLOW_BAD_THERMOCOUPLES = true;
#endif

  if (!sensors.getAddress(postHeaterThermometer, 0)) {
    CogCore::Debug<const char *>("Unable to find address for Device 0\n");
    if (!ALLOW_BAD_THERMOCOUPLES) {
      CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
    } else {
      CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
    }
  }
  if (!sensors.getAddress(postGetterThermometer, 1)) {
    CogCore::Debug<const char *>("Unable to find address for Device 1\n");
    if (!ALLOW_BAD_THERMOCOUPLES) {
      CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
    } else {
      CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
    }
  }
  if (!sensors.getAddress(postStackThermometer, 2)) {
    CogCore::Debug<const char *>("Unable to find address for Device 2\n");
    if (!ALLOW_BAD_THERMOCOUPLES) {
      CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
    } else {
      CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
    }
  }

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them.  It might be a good idea to
  // check the CRC to make sure you didn't get garbage.  The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to postHeaterThermometer
  //if (!oneWire.search(postHeaterThermometer)) CogCore::Debug<const char *>("Unable to find address for postHeaterThermometer\n");
  // assigns the seconds address found to postGetterThermometer
  //if (!oneWire.search(postGetterThermometer)) CogCore::Debug<const char *>("Unable to find address for postGetterThermometer\n");

  // show the addresses we found on the bus
  CogCore::Debug<const char *>("Device 0 Address: ");
  printAddress(postHeaterThermometer);
  CogCore::Debug<const char *>("\n");

  CogCore::Debug<const char *>("Device 1 Address: ");
  printAddress(postGetterThermometer);
  CogCore::Debug<const char *>("\n");

  CogCore::Debug<const char *>("Device 2 Address: ");
  printAddress(postStackThermometer);
  CogCore::Debug<const char *>("\n");

  // set the resolution to 9 bit
  sensors.setResolution(postHeaterThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(postGetterThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(postStackThermometer, TEMPERATURE_PRECISION);

  CogCore::Debug<const char *>("Device 0 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postHeaterThermometer));
  CogCore::Debug<const char *>("\n");

  CogCore::Debug<const char *>("Device 1 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postGetterThermometer));
  CogCore::Debug<const char *>("\n");

  CogCore::Debug<const char *>("Device 2 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postStackThermometer));
  CogCore::Debug<const char *>("\n");
  }

  MAX31850Temperature::MAX31850Temperature(SensorConfig &config) {
    oneWire.begin(MAX31850_DATA_PIN);
    // Pass our oneWire reference to Dallas Temperature.

    sensors.setOneWire(&oneWire);
  }



  // function to print a device address
  void MAX31850Temperature::printAddress(DeviceAddress deviceAddress)
  {
    for (uint8_t i = 0; i < 8; i++)
      {
	char t[3];
	sprintf(t, "%0X", deviceAddress[i]);
	CogCore::Debug<const char *>(t);
      }
  }

  float MAX31850Temperature::ReadTemperature() {
    this->sensors.requestTemperatures(); // Send the command to get temperatures
    return GetTemperature(0);
  }
  float MAX31850Temperature::GetTemperature() {
    return GetTemperature(0);
  }

  float MAX31850Temperature::GetTemperature(int idx) {
    float tempC;

#ifdef USE_ADRESS_BASED_RETRIEVAL
    switch(idx) {
    case 0:
      tempC = this->sensors.getTempC(postHeaterThermometer);
      break;
    case 1:
      tempC = this->sensors.getTempC(postGetterThermometer);
      break;
    case 2:
      tempC = this->sensors.getTempC(postStackThermometer);
      brak;
    default: {
      CogCore::Debug<const char *>("INTERNAL ERROR! BAD IDX FOR MAX31850: ");
      CogCore::Debug<uint32_t>(idx);
      OxCore::Debug<const char *>("\n");
    }
    }
#else
    tempC = this->sensors.getTempCByIndex(idx);
#endif
    return tempC;

  }
  SensorConfig MAX31850Temperature::GetConfig() const {
    return _config;
  }
}
#endif
