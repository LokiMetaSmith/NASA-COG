# Factory Test Plan, Control V1.1, Printed Wiring Assembly
Journal of tests for assemblies of October 2023.  
PCB assembly by JLCPCB with some finish work in Maryville TN.  
Five Assemblies assigned SN 9-14.  

Notes made in issue #89 during the development of this test plan.

## Resources
12V power supply with current monitor.  
Multi meter to measure voltage and resistance.  (Actual Multimeter (EMCO DMR-3800) )  
PC with Arduino IDE to connect to USB programing port of Due  
Firmware: https://github.com/PubInv/NASA-MCOG/tree/develop/elec/Control%20Board/Control_1v1/FactoryTest  
Arduino Sketches: 
1. DueWithThreeSSRs
2. GraphicsTest

## Test Procedure
### Before Test, Finish Assembly
Install on these units the feed through headers for the Due connections which make this assembly become a shield for a Due.


### Unpowered Test of PWA.

Summary and data capture table

Tester Name ____ F. Lee Erickson ________________
Date of Test _____ 20231026 _____________________

| Test ID 	| Test Name       	| Setup                                          	| Test Location 	| Requirements    	| Results SN 9 	| Results SN 10 	| Results SN 11 	| Results SN 12 	| Results SN 13 	|
|--------:	|-----------------	|------------------------------------------------	|---------------	|-----------------	|--------------	|---------------	|---------------	|---------------	|---------------	|
|       1 	| Inspection      	| Inspect correct orientation on polarized parts 	|               	|                 	|      D1 and D2 Probably backwards. |               	|               	|               	|               	|
|       2 	| +24 In No Short 	| Set ohm meter 20M. Com to GND                  	| TP20 (J12 Pin 1)   	|  > 10K 	|  34.6K  	|  34.6K        	|     34.5K  	|  34.6K       	|               	|
|       3 	| +12 In No Short 	| Set ohm meter 20M. Com to GND                  	| TP 2 (J11 Pin 1) |   > 10K 	 |    39.8K |  39.9K      	|   39.8K         |   39.8k      	|               	|
|       4 	| 5VHEAD net      	| Set ohm meter 20M. Com to GND                  	| J8 Pin 1 | Open or > 1 Meg 	|     Open |       Open   	|      Open     	|      Open   	|               	|
|       5 	| +5V net         	| Set ohm meter 20M. Com to GND                  	| J3 Pin 4 | Open or > 1 Meg 	|     12.1 Meg |      Open   	|        Open 	| 2.4K (Issue #126 https://github.com/PubInv/NASA-MCOG/issues/126) |       	|
|       6 	| +3.3V net       	| Set ohm meter 20M. Com to GND                  	| J3 Pin 5 | Open or > 1 Meg 	|   11.7 Meg |         Open    	|         Open     	|         Open     	|               	|



### Program Due with Firmware
Before connecting to the Controller V1.1 Assembly, program the Due with Load the factory test firmware "MAX31850_Tester.ino" 

## Finish Assembly

#### Build Up MAX31850 Thermocouple Amplifier Sub Assemblies
Note that the side of the board to be up has the pads for configuring the addresses of the assemblies. 
Cut the on board shunt on each MAX31850 PCB. Measure with multimeter that the shunt to GND is open.
Solder the header and screw terminal to the MAX31850 on the correct side.

1. Install the MAC13850 thermocouple amplifier assemblies to the Control V1.1 at JP9, JP10 and JP15.
Add a ?2.5mm? x 10 mm screw with nut as mechanice support and retention.
Observing polarity, connect thermocouples to the MAX31850 amplifire assemblies. 
**NEED IMAGE HERE**

2. Added legs (8-32 x 2") with nuts.
3. Solder in the Due connectors, seven locations.  The long tails must go in the top and through to the bottom
4. Connect the Due to the Controller assembly.  The Due is under the Controller V1.1 assembly as a "shield".
5. Connect the Ethernet W5200 (or similar) sheild on the top of the Controller V1.1

### Apply power. 
Note the +12V, the +24V and the Stack programable power supply are connected to a switchable outlet switch.  
With the power outlet off, 
Connect the +12V power at J11.  
Connect the  +24V power at J12.  
Connect SSR simulator LEDS at  SSR1, SSR2, SSR3.
??? Do not connect the Stack power supply at J10 ( or J27) at this time. ???

Apply power by switching on the AC power strip.
**Test ID7:** Note current in table on the +12V supply current monitor for excessive, above 100 mA.  THIS NUMBER WILL NEED TO BE CHECKED WITH DUE's THAT HAVE NEVER BEEN PROGRAMED.
Check that the BUILDIN_LED ner the LAN Shield and SSR2 and SSR3 connectors is blinking rapidly as an idication that the firmware is running.
Open the Arduino Serial Plotter (<Ctrl> <Shift> <L>) and touch some thermocouples.
### Temperature Measurements.
**Test ID8:** Typical Serial Plotter results. Touching some of the thermocouples to ensure they respond.
Note resutls in table
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/e4f5bb6a-2a47-4039-8720-b6846f36b7f7)

**Results continued...**
| Test ID 	| Test Name  | Setup | Test Location | Requirements  | Results SN 9 | Results SN 10 | Results SN 11 | Results SN 12 | Results SN 13 	|
|--------:	|------------ | -------- |---------- | ------------	|--------------	|----------	|----------- |--------------- |--------------- |
|      7 | Current, Temperature Only  | In line DC current meter |    Due input |  85 mA |             |             |             |         |
|      8 | Check thermocouples  | Connect three |   Serial Plolter |  Three traces |   OK   |             |             |             |         |
|      9 | future use  |  |      |               |             |             |             |         |            |


Load and run factory test firmware DueWithThreeSSRs.ino
Open the Arduino Serial Monitor. Observe the data.
**Test ID10 ** Observe SSR1, SSR2, SSR3 LED Blinking.
**Test ID11 ** Observe Mini12864 Display back lit and displaying splash message and four measurements.
**Test ID12 ** Encoder Rotation Knob is back lit. Rotate the know and observe the serial port reporting changes.
**Test ID13 ** Encoder knob switch. Press the knob and observer serial port message about shutdown.
**Test ID14 ** PCB mount SHUT DOWN button. Press the button and observer serial port message about shutdown.


**Results continued...**
| Test ID 	| Test Name  | Setup | Test Location | Requirements  | Results SN 9 | Results SN 10 | Results SN 11 | Results SN 12 | Results SN 13 	|
|--------:	|------------ | -------- |---------- | ------------	|--------------	|----------	|----------- |--------------- |--------------- |
|      10 | SSR1, SSR2, SSR3 LED Blink  |  |      |               |             |             |             |         |            |
|      11 | Mini12864 Display           |  |      |               |             |             |             |         |            |
|      12 |Encoder Rotation Knob        |  |      |               |             |             |             |         |            |
|      13 |Encoder knob switch          |  |      |               |             |             |             |         |            |
|      14 | PCB button SHUT DOWN        |  |      |               |             |             |             |         |            |
|      15 | LAN CONNECTION              |  |      |               |             |             |             |         |            |
|      16 | Stack power to load         |  |      |               |             |             |             |         |            |
|      17 | Cumulocity reporting        |  |      |               |             |             |             |         |            |
|      18 | PxxxxxxxxxxxxxxxxxN         |  |      |               |             |             |             |         |            |

### Add Stack Power Supply Control 
With power off.
Connect at J10 with power supply which has a load.
Load the OEDCS firmware into the unit under test.
Open the serial monitor.
Apply power to Stack power supply and +24V.
Observe the voltage on the load.
On serial port monitor confirm device connects to LAN
On serial port monitor confirm device current and voltage on stack load is normal.
Observe data on Cumulocity

Rejoice another great OEDCS has been born!


