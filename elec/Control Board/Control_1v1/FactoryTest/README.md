# Factory Test Plan, Control V1.1, Printed Wiring Assembly
Journal of tests for assemblies of October 2023.  
PCB assembly by JLCPCB with some finish work in Maryville TN.  
Five Assemblies assigned SN 9-14.  

Notes made in issue #89 during the development of this test plan.

## Resources
12V power supply with current monitor
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
|       2 	| +24 In No Short 	| Set ohm meter 20M. Com to GND                  	| TP20 (J12 Pin 1)   	|  > 10K 	|  34.6K            	|               	|               	|               	|               	|
|       3 	| +12 In No Short 	| Set ohm meter 20M. Com to GND                  	| TP 2 (J11 Pin 1) |   > 10K 	 |    39.8K |               	|               	|               	|               	|
|       4 	| 5VHEAD net      	| Set ohm meter 20M. Com to GND                  	| J8 Pin 1 | Open or > 1 Meg 	|     Open |               	|               	|               	|               	|
|       5 	| +5V net         	| Set ohm meter 20M. Com to GND                  	| J3 Pin 4 | Open or > 1 Meg 	|     12.1 Meg |               	|               	|               	|
|       6 	| +3.3V net       	| Set ohm meter 20M. Com to GND                  	| J3 Pin 5 | Open or > 1 Meg 	|   11.7 Meg |               	|               	|               	|               	|



## Finish Assembly

Before connecting to the Controller, program the Due with Load the factory test firmware "MAX31850_Tester.ino" 

Added legs (8-32 x 2") with nuts.
Solder in the Due connectors, seven locations.  The long tails must go in the top and through to the bottom

Connect the Due to the Controller assembly.
Install the thermocouple amplifier assemblies.
Observing polarity, connect thermocouples, 

### Apply power. 
Apply power to Due. Note current for excessive, above 100 mA.  THIS NUMBER WILL NEED TO BE CHECKED WITH DUE's THAT HAVE NEVER BEEN PROGRAMED.

**Results continued...**
| Test ID 	| Test Name  | Setup | Test Location | Requirements  | Results SN 9 | Results SN 10 | Results SN 11 | Results SN 12 | Results SN 13 	|
|--------:	|------------ | -------- |---------- | ------------	|--------------	|----------	|----------- |--------------- |--------------- |
|       7 | Current, Temperature Only  | In line DC current meter |    Due input |  85 mA |             |             |             |         |
|       8 | Check thermocouples  | Connect three |   Serial Plolter |  Three traces |   OK   |             |             |             |         |
|       9 | boo  | In line DC current meter |    Due input |               |             |             |             |         |            |
|     10 | murphy  | In line DC current meter |    Due input |               |             |             |             |         |            |


### Temperature Measurements.
Typical Serial Plotter results.
![image](https://github.com/PubInv/NASA-COG/assets/5836181/1fea245c-46c5-4b3a-8275-9ffe570d2142)



