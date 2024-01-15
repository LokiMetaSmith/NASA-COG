/*
  cog_hal.h -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

  Copyright 2023, Robert L. Read

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

#ifndef COG_HAL_H
#define COG_HAL_H

#include <machine.h>
#include <abstract_ps.h>
#include <abstract_fan.h>
#include <BigTreeTechMini12864.h>
//#define RF_FAN 2
//#define _HEATER 3
//#define _STACK DAC0
// This should change to PIN 5 when
// we get the planned control board.
//#define MAX31855_DATA_PIN 5
//#define MAX31850_DATA_PIN 5
// #define RF_FAN_TACH 5
//#define RF_MOSTPLUS_FLOW_PIN A0
//#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75
//(MAXCLK, MAXCS, MAXDO);
//#ifdef DISPLAY_ENABLED
//#define LCD_CLOCK  // Clock (Common), sometimes called SCK or SCL
//#define LCD_MOSI   // MOSI (common), sometimes called SDA or DATA
#define LCD_RESET 46 // LCD reset, sometimes called RST or RSTB
#define LCD_CS 48    // LCD CS, sometimes called EN or SS
#define LCD_RS 47    // LCD RS, sometimes called A0 or DC

//Name the display pins on the Due GPIO
#define DISPLAY_RESET 46            // display reset, keep high or don't care
#define DISPLAY_DC 47               // Display data / command line, keep high for display cs control
#define DISPLAY_CS 48               // Chip select for display, display LOW->Enabled, HIGH->Disabled

#define ETHERNET_CS 10
#define SD_CARD_CS  4

#define U8_DC 47  //LCD A0
#define U8_CS 48 //D0LCD_CS
#define U8_RST 46 //LCD_RESET_PIN

#define DISPLAY_CLK 13  //OEDCS SPI for display
#define DISPLAY_MOSI 11

#define U8_Width 128
#define U8_Height 64



#define BEEPER 50   //A buzzer.
#define BEEPER_PIN 50 // buzzer pin
//#define U8_MISO 50
//#define U8_MOSI 51
//#define U8_SCK  8
//#define SDSS   53 //sd card ss select
//#define SD_CS     49  //sd card card detect
//GPIO Defines
//Rotary Encoder on BigTreeTech MINI 12864
#define PIN_IN1 40
#define PIN_IN2 41
#define ENC_SW 42   //A switch

#define encA               40
#define encB               41
#define encButton          42

#define LED_RED 43
#define LED_BLUE 44
#define LED_GREEN 45
#define NEOPIX_DIN 43
#define NUMPIXELS 3
#define LED_DIN    37 //NEOPIXEL Digital INIT_Kd
//#define LED_PIN_GREEN  23 //active high
//#define LED_PIN_BLUE   53 //active high
//#endif

#define FIXED_HIGH_43 43
#define SHUT_DOWN_BUTTON 49


#ifdef CONTROL_1V1
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
D43     FIXED_HIGH_43           Output          Artifically high just as a convenience.
D44	LPBK0			Output		Varying loopback signal
D45	LPBK1			Input		Read of digital loopback signal
D49     SHUT_DOWN               Input           Enter Emergency Shutdown if pulled low
D51	HEAT1			Output		Positive SSR signal
D52 Heat2			Output		Positive SSR signal for heaterPIDTask
D53 HEAT3			Output		Positive SSR signal for heater PID
*/

#endif

class COG_HAL : public MachineHAL {
public:
  const static int NUM_FANS = 1;
  AbstractFAN* _fans[NUM_FANS];
  const static int NUM_STACKS = 1;
  AbstractPS* _stacks[NUM_STACKS];
  Mini12864* _displays[1];
  
  bool init() override;
  void _updateFanPWM(float unitInterval);

  bool isShutDownButtonPushed();

  static const int NUM_HEATERS = 1;
  const int HEATER_PINS[NUM_HEATERS] = {HEATER_PIN};

  // These are lightly tested at present
  const double INIT_Kp = 0.005;
  const double INIT_Ki = 0.0005;
  const double INIT_Kd = 0.0002;

};

#endif
