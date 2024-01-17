// Copyright (C) 2022
// Robert Read

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
#endif

#include <TF800A12K.h>
#include <core.h>

SL_PS::SL_PS() {
}
SL_PS::SL_PS(const char * name, uint8_t id) {
  // C++ probably offers a better way to do this from the base class
  this->name = name;
  this->id = id;
}

int SL_PS::init() {

  int retval = 0;

  //PS1  is attached to Serial1
  pinMode( PS1_EN, OUTPUT);
  digitalWrite(PS1_EN, HIGH);

  pinMode( PS1_AUX_SENSE, INPUT);
  
  pinMode(6, INPUT_PULLUP);    //TEST OVER CURRENT EVENT, sets reported amperage to 60

  Serial1.begin(4800);
  // This would be better done as an error message than a hard loop...
  //  delay(500);
  //  if (!Serial1) {
  //    CogCore::Debug<const char*>("Could not create Serial1 connection!");
  //    return -1;
  //  }
  while (!Serial1);
  if (setPS_OnOff(ADDRESS, "OFF")) CogCore::Debug<const char *>("Turned PSU OFF!\n");

  if (DEBUG_SL_PS > 0) {
    watchdogReset();
    getPS_Manuf(ADDRESS);
    CogCore::Debug<const char *>("GetPS Manuf: ");
    if (!strlen(manuf)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(manuf);
    delay(MYDELAY);
    watchdogReset();


    getPS_Model(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS Model: ");
    if (!strlen(model)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(model);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_VoltageString(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS VoltageSt: ");
    if (!strlen(voltage_string)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(voltage_string);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_Revision(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS Rev: ");
    if (!strlen(revision)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(revision);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_ManufDate(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS ManufDate: ");
    if (!strlen(manuf_date)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(manuf_date);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_Serial(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS Serial Address: ");
    if (!strlen(serial)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(serial);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_Country(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS Country: ");
    if (!strlen(country)) strcpy(manuf, "UNKWN");
    CogCore::Debug<const char *>(country);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_RateVoltage(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS RateVoltage: ");
    if (rate_voltage < 0) CogCore::Debug<const char *>("UNKWN");
    else CogCore::Debug<uint32_t>(rate_voltage);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_RateCurrent(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS RateCurrent: ");
    if (rate_current < 0) CogCore::Debug<const char *>("UNKWN");
    else CogCore::Debug<uint32_t>(rate_current);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_MaxVoltage(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS MaxVoltage: ");
    if (max_voltage < 0) CogCore::Debug<const char *>("UNKWN");
    else CogCore::Debug<uint32_t>(max_voltage);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();


    getPS_MaxCurrent(ADDRESS);
    watchdogReset();
    CogCore::Debug<const char *>("GetPS MaxCurrent: ");
    if (max_current < 0) CogCore::Debug<const char *>("UNKWN");
    else CogCore::Debug<uint32_t>(max_current);
    CogCore::Debug<const char *>("\n");
    delay(MYDELAY);
    watchdogReset();
  }//endif (DEBUG_SL_PS > 0)


  //  snprintf(packetBuffer, sizeof packetBuffer, "{ \"Manufacturer\": \"%s\", \"Model\": \"%s\", \"VoltString\": \"%s\", \"Revision\": \"%s\", \"Serial\": \"%s\", \"VoltageRating\": %d, \"CurrentRating\": %d, \"MaxVoltage\": %d, \"MaxCurrent\": %d}", manuf, model, voltage_string, revision, serial, rate_voltage, rate_current, max_voltage, max_current);
  //  sendMsg(packetBuffer);
  watchdogReset();
  // Note! We want to turn off the machine as quickly as possible on startup!
  if (setPS_OnOff(ADDRESS, "ON")) CogCore::Debug<const char *>("Turned it on\n");
  else {
    CogCore::Debug<const char *>("failed to turn PS on\n");
    retval = -1;
  }
  watchdogReset();
  if (setPS_Voltage(ADDRESS, 0)) CogCore::Debug<const char *>("Set volts to 0.0 volts\n");
  else {
    CogCore::Debug<const char *>("failed to set volts\n");
    retval = -1;
  }
  watchdogReset();
  if (setPS_Current(ADDRESS, 0)) CogCore::Debug<const char *>("Set current to 0.0 amps\n");
  else {
    CogCore::Debug<const char *>("failed to set current\n");
    retval = -1;
  }

  return retval;
}

bool SL_PS::disable() {
	int retval = 0;

  //PS1  is attached to Serial1, check if AUX is high, if so, disable the PSU
  if(digitalRead( PS1_AUX_SENSE) == HIGH) {
	digitalWrite(PS1_EN, LOW);
	if(digitalRead( PS1_AUX_SENSE) == HIGH) {
	  CogCore::Debug<const char *>("failed to disable PSU");
	  return false;
	}
	return true;
  }
  CogCore::Debug<const char *>("PSU already disabled");
  return true;
}

int SL_PS::reInit() {
	return reInit(0,0);
}

int SL_PS::reInit(uint16_t volts, uint16_t amps) {


  //PS1  is attached to Serial1, check if AUX is high, if so, disable the PSU
  if(digitalRead( PS1_AUX_SENSE) == LOW) {
	digitalWrite(PS1_EN, HIGH);
	if(digitalRead( PS1_AUX_SENSE) == LOW) {
	  CogCore::Debug<const char *>("failed to enable PSU");
	}
  }

  int retval = 0;
  watchdogReset();
  getPS_Control(ADDRESS); //set
  watchdogReset();
  getPS_Status0(ADDRESS);
  watchdogReset();
  getPS_Status1(ADDRESS);
  watchdogReset();
  if (DEBUG_SL_PS > 0) {
	CogCore::DebugLn<const char *>( "Pre: setPS_Control");
	CogCore::Debug<const char *>( "status0: ");
    CogCore::DebugLn< uint8_t>(status0);
	CogCore::Debug<const char *>( "status1: ");
    CogCore::DebugLn< uint8_t>(status1);
  }
  setPS_GlobOnOff(ADDRESS,"off");
  setPS_Control(ADDRESS,"REMOTE");
    watchdogReset();
  getPS_Control(ADDRESS); //set
  watchdogReset();
  getPS_Status0(ADDRESS);
  watchdogReset();
  getPS_Status1(ADDRESS);
  watchdogReset();
  if (DEBUG_SL_PS > 0) {
	CogCore::DebugLn<const char *>( "Post: setPS_Control");
	CogCore::Debug<const char *>( "status0: ");
    CogCore::DebugLn< uint8_t>(status0);
	CogCore::Debug<const char *>( "status1: ");
    CogCore::DebugLn< uint8_t>(status1);
  }


  watchdogReset();
  if (setPS_Voltage(ADDRESS, volts)) CogCore::Debug<const char *>("Set volts to 0.0 volts\n");
  else {
    CogCore::Debug<const char *>("failed to set volts\n");
    retval = -1;
  }

  watchdogReset();
  if (setPS_Current(ADDRESS, amps)) CogCore::Debug<const char *>("Set current to 0.0 amps\n");
  else {
    CogCore::Debug<const char *>("failed to set current\n");
    retval = -1;
  }
  // Note! We want to turn off the machine as quickly as possible on startup!
  if (setPS_OnOff(ADDRESS, "ON")) CogCore::Debug<const char *>("Turned it on\n");
  else {
    CogCore::Debug<const char *>("failed to turn PS on\n");
    retval = -1;
  }
  return retval;
}

// Return True if Okay, false if bad.
bool SL_PS::evaluatePS(){
  getPS_Control(ADDRESS);
  watchdogReset();
  getPS_Status0(ADDRESS);//doesn't trigger any bits
  watchdogReset();
  getPS_Status1(ADDRESS);//doesn't trigger any bits for control or other flags
  watchdogReset();
  if (DEBUG_SL_PS > 0) {
    CogCore::Debug<const char *>( "status0: ");
    CogCore::DebugLn< uint8_t>(status0);
    CogCore::Debug<const char *>( "status1: ");
    CogCore::DebugLn< uint8_t>(status1);
    if(status0 & 0x01)CogCore::DebugLn<const char *>( "Bit-0 -> OVP Shutdown");
    if(status0 & 0x02)CogCore::DebugLn<const char *>( "Bit-1 -> OLP Shutdown");
    if(status0 & 0x04)CogCore::DebugLn<const char *>( "Bit-2 -> OTP Shutdown");
    if(status0 & 0x08)CogCore::DebugLn<const char *>( "Bit-3 -> FAN Failure");
    if(status0 & 0x10)CogCore::DebugLn<const char *>( "Bit-4 -> AUX or SMPS Fail");
    if(status0 & 0x20)CogCore::DebugLn<const char *>( "Bit-5 -> HI-TEMP Alarm");
    if(status0 & 0x40)CogCore::DebugLn<const char *>( "Bit-6 -> AC Input Power Down");
    if(status0 & 0x80)CogCore::DebugLn<const char *>( "Bit-7 -> AC Input Failure");
    if(!(status0 & 0xFF))CogCore::DebugLn<const char *>( "status0: OK");
    if(status1 & 0x01)CogCore::DebugLn<const char *>( "Bit-0 -> Inhibit by VCI / ACI or ENB");
    if(status1 & 0x02)CogCore::DebugLn<const char *>( "Bit-1 -> Inhibit by Software Command");
    if(status1 & 0x04)CogCore::DebugLn<const char *>( "Bit-2 -> (Not used)");
    if(status1 & 0x08)CogCore::DebugLn<const char *>( "Bit-3 -> (Not used)");
    if(status1 & 0x10)CogCore::DebugLn<const char *>( "Bit-4 -> (POWER)");
    if(status1 & 0x20)CogCore::DebugLn<const char *>( "Bit-5 -> (Not used)");
    if(status1 & 0x40)CogCore::DebugLn<const char *>( "Bit-6 -> (Not used)");
    if(status1 & 0x80)CogCore::DebugLn<const char *>( "Bit-7 -> (REMOTE)");
    if(!(status0 & 0xFF))CogCore::DebugLn<const char *>( "status0: OK");
    if(!(status1 & 0x6D))CogCore::DebugLn<const char *>( "status1: OK");
    if(control){CogCore::DebugLn<const char *>( "REMOTE");}else{CogCore::Debug<const char *>( "LOCAL");};
  };

  // I'm commenting this out as it considers my PSU (Austin unit, OEDCS #SN1) to be in error.
  // I don't know what the bits in status1 that are high are supposed to mean. On my machine they are:
  // Inhibit by VCI / ACI or ENB
  // Bit-1 -> Inhibit by Software Command
  // Bit-4 -> (POWER)
  // Bit-7 -> (REMOTE)
  // status0: OK
  // REMOTE
  // In binary: 10010011
  // normally status1: 1001 0010 //in OKC
  //6D is 0110 1101
#ifdef DO_NOT_CHECK_INHIBIT_BY_VCI_ON_PSU
  const int EXPECTED_STATUS1 = 0x93;
#else
  const int EXPECTED_STATUS1 = 0x92;
#endif
  if( !(status0 & 0xFF) && (status1 == EXPECTED_STATUS1)){
    if (DEBUG_SL_PS > 0) {
      CogCore::DebugLn<const char *>( "PSU GOOD!");
    }
    return true;
  } else{
    CogCore::Debug<const char *>( "status0: ");
    CogCore::DebugLn< uint8_t>(status0);
    CogCore::Debug<const char *>( "status1: ");
    CogCore::DebugLn< uint8_t>(status1);
    CogCore::DebugLn<const char *>( "RUNNING reInit!");

    // WARNING: LAWRENCE --- this return masks the code below it!
    // I am not sure what the correct solution is. - rlr
    return reInit();
    if( !(status0 & 0xFF)){
      CogCore::DebugLn<const char *>( "LOST CONTROL OF PSU, CHECK STACK VOLTAGE");
      return true;
    }
    //return false;

  }

  // //If everything is working, we will mask with a known good state status0 & 0xFF and status1 0x92 0b1001 0010
  // return false;

  // This line written by rlr to make it work until we ahve an explanation of what is supposed to be done above...
  //return (!(status0 & 0xFF));

}

int SL_PS::setPS_Addr(uint8_t addr) {
  Serial1.print("ADDS "); Serial1.print(addr); Serial1.print("\r\n");
  //delay(50);
  this->address = addr;
  char buff[5];
  uint8_t c = Serial1.readBytesUntil('\n', buff, sizeof buff);
  if (DEBUG_SL_PS > 0) {
	CogCore::Debug<const char *>( "setPS_Addr reads: ");
    CogCore::DebugLn< char *>(buff);
  }
  if (c != 3 || buff[0] != '=' || buff[1] != '>') return 0;
  return 1;
}
//= > CR LF -> Command executed successfully.
//? > CR LF -> Command error, not accepted.
//! > CR LF -> Command correct but execution error (e.g. parameters out of range).
int SL_PS::setPS_Val(uint8_t addr, const char *loc, const char *val) {
  if (!setPS_Addr(addr)) {
    CogCore::Debug<const char *>("setPS_Val didn't set address\n");
	reInit();
    return setPS_Addr(addr);
  }

  Serial1.print(loc);
  Serial1.print(' ');
  Serial1.print(val);
  Serial1.print("\r\n");
  // Do we need this delay?
  //  delay(50);
  char b[5];
  int c = Serial1.readBytesUntil('\n', b, sizeof b);
  if (DEBUG_SL_PS > 0) {
	CogCore::Debug<const char *>( "setPS_Val reads: ");
    CogCore::DebugLn< char *>(b);
  }
  if (c != 3 || b[0] != '=' || b[1] != '>') return 0;
  return 1;
}

int SL_PS::setPS_GlobOnOff(uint8_t addr, const char *val) {
  if (strcasecmp(val, "on") == 0) val = "1";
  else val = "0";
  return setPS_Val(addr, "GLOB", val);
}

int SL_PS::setPS_OnOff(uint8_t addr, const char *val) {
  if (strcasecmp(val, "on") == 0) val = "1";
  else val = "0";
  return setPS_Val(addr, "POWER", val);
}

int SL_PS::setPS_Voltage(uint8_t addr, uint16_t volts) {
  char b[7];
  snprintf(b, sizeof b, "%4.1f", volts / 100.0);
  return setPS_Val(addr, "SV", b);
}

int SL_PS::setPS_Current(uint8_t addr, uint16_t amps) {
  char b[7];
  snprintf(b, sizeof b, "%4.1f", amps / 100.0);
  return setPS_Val(addr, "SI", b);
}

int SL_PS::setPS_Control(uint8_t addr,const char *val) {
  if (strcasecmp(val, "LOCAL") == 0) val = "";
  if (strcasecmp(val, "REMOTE") == 0) val = "1";
  return setPS_Val(addr, "REMS", val);
}


char *SL_PS::getPS_Val(uint8_t addr, const char *val) {
  static char rval[100];
  static char b[50];
  rval[0]  = '\0';
  b[0]  = '\0';
  if (!setPS_Addr(addr)) {
    Serial.println("didn't set address");
    return 0;
  }
  int c = 0;
  Serial1.print(val); Serial1.print("\r\n");
  // delay(50);
  c = Serial1.readBytesUntil('\n', b, sizeof b);
  b[c-1] = '\0';
  if (DEBUG_SL_PS_UV > 0) {
    CogCore::Debug<const char *>("first read: ");
    CogCore::DebugLn<const char *>(b);
    CogCore::Debug<const char *>("=====\n");
  }

  for(int i=0; i<2;i++ )
    {
      if(b[0] != '=' && b[1] != '>') {
        if (DEBUG_SL_PS_UV > 0) {
          CogCore::Debug<const char *>("final: ");
          CogCore::DebugLn<int>(i);
          CogCore::Debug<const char *>(" : ");
          CogCore::DebugLn<const char *>(rval);
          CogCore::Debug<const char *>("=====\n");
        }
        if(b[0] == '?' && b[1] == '>') {
          Serial.println("Command error, not accepted.");
          return rval;
        }
        if(b[0] == '!' && b[1] == '>') {
          Serial.println("Command correct but execution error (e.g. parameters out of range).");
          return rval;
        }
        strncat(rval,b, sizeof b);
      }else{
        return rval;
      }

      //      delay(10);
      c = Serial1.readBytesUntil('\n', b, sizeof b);
      b[c-1] = '\0';
      if (DEBUG_SL_PS_UV > 0) {
        CogCore::Debug<const char *>("intermediate: ");
        CogCore::DebugLn<int>(i);
        CogCore::Debug<const char *>(" : ");
        CogCore::DebugLn<const char *>(b);
        CogCore::Debug<const char *>("=====\n");
      }
    }
  return rval;
}

void SL_PS::getPS_Manuf(int addr) {
  char *r = getPS_Val(addr, "INFO 0");
  strncpy(manuf, r, sizeof manuf);
}

void SL_PS::getPS_Model(int addr) {
  char *r = getPS_Val(addr, "INFO 1");
  strncpy(model, r, sizeof model);
}

void SL_PS::getPS_VoltageString(int addr) {
  char *r = getPS_Val(addr, "INFO 2");
  strncpy(voltage_string, r, sizeof voltage_string);
}

void SL_PS::getPS_Revision(int addr) {
  char *r = getPS_Val(addr, "INFO 3");
  strncpy(revision, r, sizeof revision);
}

void SL_PS::getPS_ManufDate(int addr) {
  char *r = getPS_Val(addr, "INFO 4");
  strncpy(manuf_date, r, sizeof manuf_date);
}

void SL_PS::getPS_Serial(int addr) {
  char *r = getPS_Val(addr, "INFO 5");
  strncpy(serial, r, sizeof serial);
}

void SL_PS::getPS_Country(int addr) {
  char *r = getPS_Val(addr, "INFO 6");
  strncpy(country, r, sizeof country);
}

void SL_PS::getPS_RateVoltage(int addr) {
  char *r = getPS_Val(addr, "RATE?");
  char b[20];
  strncpy(b, r, sizeof b);
  char *ptr = NULL;
  rate_voltage = -1;
  if ((ptr = strchr(b, ' '))) {
    ptr = strchr(b, 'V');
    *ptr = '\0';
    rate_voltage = int(atof(b) * 100);
  }
}

void SL_PS::getPS_RateCurrent(int addr) {
  char *r = getPS_Val(addr, "RATE?");
  char b[20];
  strncpy(b, r, sizeof b);
  char *ptr = NULL;
  char *ptr_2 = NULL;
  rate_current = -1;
  if ((ptr = strchr(b, ' '))) {
    ptr_2 = strchr(ptr, 'A');
    *ptr_2 = '\0';
    rate_current = int(atof(ptr + 1) * 100);
  }
}

void SL_PS::getPS_OnOff(int addr) {
  char *r = getPS_Val(addr, "POWER 2");
  switch (r[0]) {
  case '0': on_off = 0; break;
  case '1': on_off = 1; break;
  case '2': on_off = 0; break;
  case '3': on_off = 1; break;
  }
}

void SL_PS::getPS_MaxVoltage(int addr) {
  max_voltage = -1;
}

void SL_PS::getPS_MaxCurrent(int addr) {
  max_current = -1;
}

void SL_PS::getPS_OutVoltage(int addr) {
  char *r = getPS_Val(addr, "RV?");
  out_voltage = int(atof(r) * 100);
}

// CORRUP THIS WITH A TEST
void SL_PS::getPS_OutCurrent(int addr) {
  char *r = getPS_Val(addr, "RI?");
  if (digitalRead(6)) {
    out_current = int(atof(r) * 100);
  }else{
	CogCore::Debug<const char *>("getPS_OutCurrent Current: 60");
	out_current = 6000;
  }
  
}

void SL_PS::getPS_Status0(int addr) {
  char *r = getPS_Val(addr, "STUS 0");
  status0 = (r[0] - '0') << 4;
  status0 += (r[1] - '0') & 0x0F;
}

void SL_PS::getPS_Status1(int addr) {
  char *r = getPS_Val(addr, "STUS 1");
  status1 = (r[0] - '0') << 4;
  status1 += (r[1] - '0') & 0x0F;
}

void SL_PS::getPS_Temp(int addr) {
  char *r = getPS_Val(addr, "RT?");
  temp = atoi(r);
}

void SL_PS::getPS_SetVoltage(int addr) {
  char *r = getPS_Val(addr, "SV?");
  set_voltage = int(atof(r) * 100);
}

void SL_PS::getPS_SetCurrent(int addr) {
  char *r = getPS_Val(addr, "SI?");
  set_current = int(atof(r) * 100);
}

void SL_PS::getPS_Control(int addr) {
  char *r = getPS_Val(addr, "REMS 2");
  switch (r[0]) {
  case '0': control = 0; break;
  case '1': control = 1; break;
  }
}

void SL_PS::printFullStatus(int addr) {

  getPS_OutCurrent(addr);
  CogCore::Debug<const char *>("SL_PS out voltage: ");
  CogCore::Debug<uint32_t>(out_voltage);
  CogCore::Debug<const char *>("\n");
  CogCore::Debug<const char *>("SL_PS out current: ");
  CogCore::Debug<const char *>("\n");


}
// TODO: We are not handling the a bad return value well here!
// A problem setting this value could be an critical error...
void SL_PS::updateAmperage(float amperage, MachineConfig *config) {
  MachineStatusReport *msr = config->report;
  uint16_t amps = (uint16_t) (amperage * 100.0);

  if (DEBUG_SL_PS > 0) {
    CogCore::Debug<const char *>("Setting SL_PS_Amps: ");
    CogCore::Debug<uint32_t>(amperage);
    CogCore::Debug<const char *>("\n");
  }
  int ret_val = setPS_Current(this->address, amps);
  if (!ret_val) {
    CogCore::Debug<const char *>("FAILED TO SET AMPERAGE!\n");
    msr->ms = CriticalFault;
  }
  // I don't like to use delay but I think some time is needed here...
  delay(10);

  getPS_OutVoltage(this->address);
  getPS_OutCurrent(this->address);
  msr->stack_voltage = out_voltage / 100.0;
  msr->stack_amps = out_current / 100.0;
  msr->stack_ohms = msr->stack_voltage/ msr->stack_amps;
  msr->stack_watts = msr->stack_voltage * msr->stack_amps;
  if (DEBUG_SL_PS > 0) {
    printFullStatus(this->address);
  }
}

void SL_PS::updateVoltage(float voltage, MachineConfig *config) {

  MachineStatusReport *msr = config->report;
  uint16_t volts = (uint16_t) (voltage * 100.0);

  if (DEBUG_SL_PS_UV > 0) {
    CogCore::Debug<const char *>("Setting SL_PS_Volts: ");
    CogCore::Debug<uint32_t>(volts);
    CogCore::Debug<const char *>("\n");
  }
  int ret_val = setPS_Voltage(this->address, volts);
  if (!ret_val) {
    CogCore::Debug<const char *>("FAILED TO SET VOLTAGE!\n");
    msr->ms = CriticalFault;
  }


  // I don't like to use delay but I think some time is needed here...
  delay(10);

  if (DEBUG_SL_PS_UV > 0) {
    CogCore::Debug<const char *>("SL_PS Voltage Updated 1: ");
    CogCore::DebugLn<int>(millis());
  }

  getPS_OutVoltage(this->address);
  if (DEBUG_SL_PS_UV > 0) {
    CogCore::DebugLn<const char *>("SL_PS Voltage Updated 1.5");
    CogCore::DebugLn<int>(millis());
  }

  getPS_OutCurrent(this->address);

  if (DEBUG_SL_PS_UV > 0) {
    CogCore::DebugLn<const char *>("SL_PS Voltage Updated 2");
    CogCore::DebugLn<int>(millis());
  }

  msr->stack_voltage = out_voltage / 100.0;
  msr->stack_amps = out_current / 100.0;
  msr->stack_watts = msr->stack_voltage * msr->stack_amps;
  if (msr->stack_amps <= 0.0) {
    msr->stack_ohms = -999.0;
  } else {
    msr->stack_ohms = msr->stack_voltage/ msr->stack_amps;
  }

  if (DEBUG_SL_PS_UV > 0) {
    CogCore::DebugLn<const char *>("SL_PS Voltage Updated 3");
    CogCore::DebugLn<int>(millis());
  }

  if (DEBUG_SL_PS_UV > 0) {
    printFullStatus(this->address);
  }

  if (DEBUG_SL_PS_UV > 0) {
    CogCore::DebugLn<const char *>("SL_PS Voltage Updated 4");
  }

}

#endif
