/*
  gpio_pin_defs.h -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

  Copyright 2024, Robert L. Read

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef GPIO_PIN_DEFS_H
#define GPIO_PIN_DEFS_H


#ifdef CTL_V_1_1
/*
A0	FAN1_FG			Input		Blower 1 Tachometer
A1  SENSE_24V       Analog Input
A2  SENSE_12V       Analog Input
A3  SENSE_AUX1
A4  Unused, was SENSE_AUX2
A5  Unused
A6  SENSE_AUX2, was Unused

D5	TEMP1			BIDirect	Dallas One-Wire connection to Thermocouple Breakouts	Daisy chain connection to temprature probes. Address of ???
D8
D9	nFAN1_PWM		Output		Blower PWM	This output will be inverted
D18	TX1	Output		Digital 	Power Supply TF800 Pin 23	This is Serial1 TX for (power supply)[https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
D19	RX1	Input		Digital 	Power Supply TF800 Pin 24	This is Serial1 RX for (power supply) [https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
D16	TX2	Output		Digital 	Power Supply TF800 Pin 23	This is Serial2 TX for (power supply)[https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
D17	RX2	Input		Digital 	Power Supply TF800 Pin 24	This is Serial2 RX for (power supply) [https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]

D22	BLOWER_ENABLE	Output		Blower Enable
D32	GPAD_nCS		Output		External SPI inverted select (for the GPAD)



// These two are not currently defined.
D44	LPBK0			Output		Varying loopback signal
D45	LPBK1			Input		Read of digital loopback signal

D43 for Status LED (GPIO HIGH, light is on)
D43     FIXED_HIGH_43           Output          Artifically high just as a convenience.
D44 for Fault LED (GPIO HIGH, light is on)
D45 for Keep Alive (GPIO HIGH, Battery engaged, GPIO LOW, battery disengaged; when GPIO LOW unit will shut off when front
power switch is toggled)
D42 for Front panel switch detection

// D49     SHUT_DOWN               Input           Enter Emergency Shutdown if pulled low
D51	HEAT1			Output		Positive SSR signal
D52 Heat2			Output		Positive SSR signal for heaterPIDTask
D53 HEAT3			Output		Positive SSR signal for heater PID
*/

#define FIXED_HIGH_43 43
#define SHUT_DOWN_BUTTON 49

#define BLOWER_ENABLE 22

#ifdef RIBBONFISH

#define RF_STACK DAC0
#define SENSE_12V A2
#define SENSE_24V A1
#elif ION_CONTROL_BOARD
// HACK! WARNING -- This is for the ESP32 and needs to Changed --- this is OBVIOUSLY WRONG
#define SENSE_12V 5
#define SENSE_24V 5
#endif


#ifdef CTL_V_1_1
#define HEATER_PIN 51

#define MAX31850_DATA_PIN 5

#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER

#define SANYO_ACE_FAN_PWM_PIN 9
#define SANYO_ACE_TACH_PIN A0

#define KEEP_ALIVE 45
#define PANEL_LED_FAULT 44
#define PANEL_LED_STATUS 43 // on -- at temperature -- flashing = warmup/cooldown, off/standby -- Machine Off
#define PANEL_SWITCH_DETECTION 42

// Note, this has often been defined in Platform.ini

#elif STAGE2_HEATER

// WARNING! These values are obsolete.
// There is probably no real dependence on these.
// They should be hunted down and removed.
#define MAX31850_DATA_PIN 5
// This is obsolete
#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER
// #define RF_FAN DAC1 //DIFFERENT FOR STAGE2_HEATER
#define RF_STACK DAC0
// #define RF_MOSTPLUS_FLOW_PIN A0
// #define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75

#endif
//Name the pins from the Due
#define DISPLAY_CS 48 // display LOW->Enabled, HIGH->Disabled
#define DISPLAY_DC 47 //display data / command line, keep high for display cs control
#define DISPLAY_RESET 46 // display reset, keep high or don't care

#elif ION_CONTROL_BOARD
/*
 * The ION control board is the official board for AmOx's commercial product.
 *
 * Analog0: 0-10V						GPIO-03
 * Analog1: Current monitor				GPIO-02
 * Analog2: 4-20mA						GPIO-01
 * Analog3: Voltage monitor				GPIO-04
 *
 * Fan0 Tach							GPIO-05
 * Fan0 PWM								GPIO-12
 * Fan1 PWM								GPIO-13
 * Fan2 PWM								GPIO-14
 * Fan Enable							GPIO-11
 */

// HACK! WARNING -- This is for the ESP32 and needs to Changed --- this is OBVIOUSLY WRONG
#define SENSE_12V 12
#define SENSE_24V 12

#define MAX31850_DATA_PIN 12

//#define THERMOCOUPLE_PIN MAX31850_DATA_PIN
#define THERMOCOUPLE_PIN 12

#define SANYO_ACE_FAN_PWM_PIN 12
#define SANYO_ACE_TACH_PIN 33

// Rob is making an arbitrary assignment here to get it to compile - rlr
#define HEATER_PIN 17
#define BLOWER_ENABLE 13

// There is no KEEP_ALIVE on ION
// #define KEEP_ALIVE 45
// There is no PANEL_LED on ION?
// #define PANEL_LED_FAULT 44
// #define PANEL_LED_STATUS 43 // on -- at temperature -- flashing = warmup/cooldown, off/standby -- Machine Off
// #define PANEL_SWITCH_DETECTION 42

// Note, this has often been defined in Platform.ini
// #define HEATER_PIN 51

#else


#endif

#endif // GPIO_PIN_DEFS_H
