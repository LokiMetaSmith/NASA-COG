// Copyright (C) 2022
// Robert Read

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifndef TF800A12K
#define TF800A12K

// Put guard for Arduino here

#ifdef ARDUINO

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <abstract_ps.h>
#include <machine.h>

#define ADDRESS 0x00
#define MYDELAY 50

//pin defines, should probably go somewhere else such as cog_hal

//PS1  is attached to Serial1
#define PS1_EN 23
#define PS1_AUX_SENSE A3 //A6


//PS2  is attached to Serial2
#define PS2_EN 8
#define PS2_AUX_SENSE A4 //A6




// TODO: This could potentially become part of
// an abstract "power supply"
class SL_PS : public AbstractPS  {
private:
  int DEBUG_SL_PS = 1;
  int DEBUG_SL_PS_UV = 0;
  int init(); // return -1 if it failes!
  uint8_t address = ADDRESS;
  char manuf[17]; // INFO 0
  char model[17];  // INFO 1
  char voltage_string[5]; // INFO 2
  char revision[5]; // INFO 3
  char manuf_date[9];  // INFO 4
  char serial[17];  // INFO 5
  char country[17];  // INFO 6
  uint16_t rate_voltage;  //50-51
  uint16_t rate_current;  //52-53
  uint16_t max_voltage;  //54-55
  uint16_t max_current;  //56-57
  uint16_t out_voltage;  //60-61
  uint16_t out_current;  //62-63
  uint8_t temp;  //68
  uint8_t status0;  //6C
  uint8_t status1;  //6F
  uint16_t set_voltage; // 70-71 r/w
  uint16_t set_current; // 72-73 r/w
  uint8_t control; //  7C  r/w
  uint8_t on_off;
public:
  SL_PS();
  SL_PS(const char * name, uint8_t id);
  int reInit();
  int reInit( uint16_t volts, uint16_t amps);
  bool disable();
  bool evaluatePS();

  int setPS_Addr(uint8_t addr);

  int setPS_Val(uint8_t addr, const char *loc, const char *val);
  int setPS_Control(uint8_t addr,const char *val);
  int setPS_GlobOnOff(uint8_t addr, const char *val);
  int setPS_OnOff(uint8_t addr, const char *val);
  int setPS_Voltage(uint8_t addr, uint16_t volts);
  int setPS_Current(uint8_t addr, uint16_t amps);
  char *getPS_Val(uint8_t addr, const char *val);
  int getPS_Manuf(int addr);
  int getPS_Model(int addr);
  int getPS_VoltageString(int addr);
  int getPS_Revision(int addr);
  int getPS_ManufDate(int addr);
  int getPS_Serial(int addr);
  int getPS_Country(int addr);
  int getPS_RateVoltage(int addr);
  int getPS_RateCurrent(int addr);
  int getPS_OnOff(int addr);
  void getPS_MaxVoltage(int addr);
  void getPS_MaxCurrent(int addr);
  int getPS_OutVoltage(int addr);
  int getPS_OutCurrent(int addr);
  int getPS_Status0(int addr);
  int getPS_Status1(int addr);
  int getPS_Temp(int addr);
  int getPS_SetVoltage(int addr);
  int getPS_SetCurrent(int addr);
  int getPS_Control(int addr);


  void printFullStatus(int addr);
  //  void updateAmperage(float amperage);
  int updateAmperage(float amperage, MachineConfig *config);
  //  void updateVoltage(float voltage);
  int updateVoltage(float voltage, MachineConfig *config);
};

#endif
#endif
