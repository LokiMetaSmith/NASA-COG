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
#ifdef USE_THREE_TC_CONFIG
  DeviceAddress postGetterThermometer;
#endif
  DeviceAddress postStackThermometer;

  // This is Rob's attempt to check the
  // Address pins returned by a given socket location.
  // This is an attempt to understand Adafruit's documenation
  // and use this example code:
  // https://github.com/adafruit/MAX31850_OneWire/blob/master/examples/MAX31850_Temperature/MAX31850_Temperature.ino
#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3

  void MAX31850Temperature::readAddressPins() {
    byte i;
    byte present = 0;
    byte temptype;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    CogCore::DebugLn<const char *>("******************************************");
    if ( !oneWire.search(addr)) {
      CogCore::DebugLn<const char *>("");
      CogCore::DebugLn<const char *>("No more addresses.");
      CogCore::DebugLn<const char *>("");
      oneWire.reset_search();
      delay(250);
      return;
    }
    CogCore::DebugLn<const char *>("ROM =");
    for( i = 0; i < 8; i++) {
      Serial.write(' ');
      CogCore::Debug<const char *>(" ");
      Serial.print(addr[i], HEX);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
      CogCore::Debug<const char *>("CRC is not valid!");
      return;
    }
    CogCore::DebugLn<const char *>("");

    // the first ROM byte indicates which chip
    switch (addr[0]) {
    case 0x10:
      CogCore::DebugLn<const char *>("  Chip = DS18S20");  // or old DS1820
      temptype = TYPE_DS18S20;
      break;
    case 0x28:
      CogCore::DebugLn<const char *>("  Chip = DS18B20");
      temptype = TYPE_DS18B20;
      break;
    case 0x22:
      CogCore::DebugLn<const char *>("  Chip = DS1822");
      temptype = TYPE_DS18S22;
      break;
      // ADDED SUPPORT FOR MAX31850!
    case 0x3B:
      CogCore::DebugLn<const char *>("  Chip = MAX31850");
      temptype = TYPE_MAX31850;
      break;
    default:
     CogCore::DebugLn<const char *>("Device is not a DS18x20 family device.");
      return;
    }

    oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0xBE);         // Read Scratchpad

    CogCore::DebugLn<const char *>("  Data = ");
    Serial.print(present, HEX);
    CogCore::DebugLn<const char *>(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = oneWire.read();
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    CogCore::DebugLn<const char *>(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    CogCore::DebugLn<const char *>("");

    Serial.print("  Address = 0x"); Serial.println(data[4] & 0xF, HEX);

    // Now we map these sockets to our thermocouples meaning...
    // This is expected to be different for the V2 control board.
    int ADDR_LINES = data[4] & 0xF;
    const bool V_1_1 = true;
    const bool V_1_2 = false;
    if (V_1_1) {
      switch (ADDR_LINES) {
      case 0x0: // This is the far one (odd man out)
        break;
      case 0x1: // This is the third socket from the panel
        for( i = 0; i < 8; i++) {
#ifdef USE_THREE_TC_CONFIG
          postStackThermometer[i] = addr[i];
#endif
        }
        break;
      case 0x4: // This is the second socket from the panel
        for( i = 0; i < 8; i++) {
#ifdef USE_THREE_TC_CONFIG
          postGetterThermometer[i] = addr[i];
#endif
          postStackThermometer[i] = addr[i];
        }
        break;
      case 0x2: // This is the first socket from the panel
        for( i = 0; i < 8; i++) {
          postHeaterThermometer[i] = addr[i];
        }
        break;
      default:
        CogCore::DebugLn<const char *>("Internal error! Unknown thermocouple address: ");
        CogCore::DebugLn<int>(ADDR_LINES);
      }
    }
    // This is conjectural, based on https://github.com/PubInv/NASA-MCOG/issues/389
    if (V_1_2) {
      switch (ADDR_LINES) {
      case 0x8: // This is the far one (odd man out)
        break;
      case 0x1: // This is the third socket from the panel
        for( i = 0; i < 8; i++) {
#ifdef USE_THREE_TC_CONFIG
          postStackThermometer[i] = addr[i];
#endif
        }
        break;
      case 0x2: // This is the second socket from the panel
        for( i = 0; i < 8; i++) {
#ifdef USE_THREE_TC_CONFIG
          postGetterThermometer[i] = addr[i];
#endif
          postStackThermometer[i] = addr[i];
        }
        break;
      case 0x4: // This is the first socket from the panel
        for( i = 0; i < 8; i++) {
          postHeaterThermometer[i] = addr[i];
        }
        break;
      default:
        CogCore::DebugLn<const char *>("Internal error! Unknown thermocouple address: ");
       CogCore::DebugLn<int>(ADDR_LINES);
      }
    }

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (temptype == TYPE_DS18S20) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else if (temptype == TYPE_MAX31850) {
      //Serial.println(raw, HEX);
      if (raw & 0x01) {
        CogCore::DebugLn<const char *>("**FAULT!**");
        return;
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    CogCore::Debug<const char *>("  Temperature = ");
    CogCore::Debug<float>(celsius);
    CogCore::Debug<const char *>(" Celsius, ");
    CogCore::Debug<float>(fahrenheit);
    CogCore::DebugLn<const char *>(" Fahrenheit");

  }

  MAX31850Temperature::MAX31850Temperature() {
  oneWire.begin(MAX31850_DATA_PIN);
    // Pass our oneWire reference to Dallas Temperature.

  sensors.setOneWire(&oneWire);

      // Start up the library
  sensors.begin();

  CogCore::Debug<const char *>("Reading Sockets...");
  for(int i = 0; i < 4; i++) {
    readAddressPins();
  }
  CogCore::Debug<const char *>("postHeaterThermometer: ");
  printAddress(postHeaterThermometer);
  CogCore::DebugLn<const char *>("");
#ifdef USE_THREE_TC_CONFIG
  CogCore::Debug<const char *>("postGetterThermometer: ");
  printAddress(postGetterThermometer);
  CogCore::DebugLn<const char *>("");
#endif
  CogCore::Debug<const char *>("postStackThermometer: ");
  printAddress(postStackThermometer);
  CogCore::DebugLn<const char *>("");

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

  // assign address manually.  the addresses below will need to be changed
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

//   if (!sensors.getAddress(postHeaterThermometer, 0)) {
//     CogCore::Debug<const char *>("Unable to find address for Device 0\n");
//     if (!ALLOW_BAD_THERMOCOUPLES) {
//       CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
//     } else {
//       CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
//     }
//   }
// #ifdef USE_THREE_TC_CONFIG
//   if (!sensors.getAddress(postGetterThermometer, 1)) {
//     CogCore::Debug<const char *>("Unable to find address for Device 1\n");
//     if (!ALLOW_BAD_THERMOCOUPLES) {
//       CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
//     } else {
//       CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
//     }
//   }
// #endif
//   int lastIndex;
// #ifdef USE_THREE_TC_CONFIG
//   lastIndex = 2;
// #else
//   lastIndex = 1;
// #endif
//   if (!sensors.getAddress(postStackThermometer, lastIndex)) {
//     CogCore::Debug<const char *>("Unable to find address for Device: ");
//     CogCore::Debug<int>(lastIndex);
//     CogCore::Debug<const char *>("\n");
//     if (!ALLOW_BAD_THERMOCOUPLES) {
//       CogCore::Debug<const char *>("Refusing to continue without a working thermocouple.\n");
//     } else {
//       CogCore::Debug<const char *>("Foolishly carrying on because ALLOW_BAD_THERMO_COUPLES_FOR_TESTING is set!\n");
//     }
//   }

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
  CogCore::Debug<const char *>("Address of Device : 0 ");
  printAddress(postHeaterThermometer);
  CogCore::Debug<const char *>("\n");

#ifdef USE_THREE_TC_CONFIG
  CogCore::Debug<const char *>("Address of Device : 1 ");
  printAddress(postGetterThermometer);
  CogCore::Debug<const char *>("\n");
#endif

  CogCore::Debug<const char *>("Address of Device Down Stream Device ");
  printAddress(postStackThermometer);
  CogCore::Debug<const char *>("\n");

  // set the resolution to 9 bit
  sensors.setResolution(postHeaterThermometer, TEMPERATURE_PRECISION);
#ifdef USE_THREE_TC_CONFIG
  sensors.setResolution(postGetterThermometer, TEMPERATURE_PRECISION);
#endif
  sensors.setResolution(postStackThermometer, TEMPERATURE_PRECISION);

  CogCore::Debug<const char *>("Device 0 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postHeaterThermometer));
  CogCore::Debug<const char *>("\n");

#ifdef USE_THREE_TC_CONFIG
  CogCore::Debug<const char *>("Device 1 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postGetterThermometer));
  CogCore::Debug<const char *>("\n");
#endif

  CogCore::Debug<const char *>("Device 2 Resolution: ");
  CogCore::Debug<uint32_t>(sensors.getResolution(postStackThermometer));
  CogCore::Debug<const char *>("\n");

#endif
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

#ifdef USE_ADDRESS_BASED_RETRIEVAL
    switch(idx) {
    case 0:
      tempC = this->sensors.getTempC(postHeaterThermometer);
      break;
    case 1:

#ifdef USE_THREE_TC_
      tempC = this->sensors.getTempC(postGetterThermometer);
#else
      tempC = this->sensors.getTempC(postStackThermometer);
#endif
      break;
    case 2:
      tempC = this->sensors.getTempC(postStackThermometer);
      break;
    default: {
      CogCore::Debug<const char *>("INTERNAL ERROR! BAD IDX FOR MAX31850: ");
      CogCore::Debug<uint32_t>(idx);
      CogCore::Debug<const char *>("\n");
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
