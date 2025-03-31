# Factory Test Plan, Control V1.2, Printed Wiring Assembly
Journal of tests for assemblies of January 2025.  
PCB assembly by JLCPCB with some finish work in Maryville TN.  
Fiffteen Assemblies assigned SN 16-30.  

Notes made in issue #TBD during the development of this test plan.

## Resources
12V power supply with current monitor. Color the plug Red. 
Multi meter to measure voltage and resistance.  (Actual Multimeter (EMCO DMR-3800) )  
PC with Arduino IDE to connect to USB programing port of Due  
<img width="400" atl="USB programing port of Due" src="https://github.com/user-attachments/assets/a4672b03-30f1-4d7a-87e9-0666e32118ae">  

Firmware: https://github.com/PubInv/NASA-MCOG/tree/develop/elec/Control%20Board/Control_1v1/FactoryTest  
Arduino Sketches: 
1. DueWithThreeSSRs
2. GraphicsTest

## Test Procedure
### Before Test, Finish Assembly
The following reworks and changes are required on the assemblies build for JLCPCB order of Control V1.2 in early 2025. 
Notes: All of the JST two pin hearders were found to be incorrecly orentaged bacuse of the "correction" of the footpring and silk screen on V1.2 The connectors for ground pins near the thermocouple amplifier are all ground and so do not need correction however the others must be removed and rotated.  JLCPCB did not have stock of the poly fuse. Other changed were found during factory testing detailed in this list of changes

#### Early 2025 Changes to Assemblies
1. Simply remove J28.
2. Color the connector J11, +12VIn red with a sharpie.
3. Color the connector J12, +24V in black with a sharpie.
4. Remove and rotate 180 degrees connector J11, J12, J13, J25, J30, and J31.
5. Change R106 and R108 from 10K to 20K. This rework is done by unsoldering the existing 10K resistor and rotating it 90 degrees on one pad. Add a second 10K resistor to the now vacant pad and a short wire beween the two resistors. Lee used 30 AWG wire wrap wire.
6. Add a 1 Ohm 0603 SMT resistor at R74 to provide 5V power at the I2C connector J24.
7. Install PolyFuse F801 near the relay.  
<img width="300" atl="PolyFuse F80" src="https://github.com/user-attachments/assets/339f6acd-438f-4da2-925a-ca8c99533dc1">  

8. Add an new resistor R186 a 10K on D42 from J8 pin 24 5o pin 30 (GND).   
<img width="300" atl="new resistor R186" src="https://github.com/user-attachments/assets/4bda4713-b8a1-43a6-a2e8-ea4b3a86b847"> <img width="400" atl="image2" src="https://github.com/user-attachments/assets/7586917d-1c07-479e-9c7b-e52c6a60cb8c">  
9. DO NOT INSTALL DUE AT THIS TIME, but do install stackable (feed through) headers for the Due connections which make this assembly become a shield for a Due. 


### Unpowered Test of PWA.
To prevent possible damage to units and test equipment, **before** adding power to the assembly make the following tests.

Summary and data capture table

Tester Name ____ F. Lee Erickson ________________
Date of Test _____ 20250130 _____________________

| **Test ID**       | 1                                              | 2                             | 3                             | 4                             | 5                             | 6                             |   |   |
|-------------------|------------------------------------------------|-------------------------------|-------------------------------|-------------------------------|-------------------------------|-------------------------------|---|---|
| **Test Name**     | Inspection                                     | +24In In No Short             | +12In In No Short             | 5VHEAD net                    | +5V net                       | +3.3V net                     |   |   |
| **Setup**         | Inspect correct orientation on polarized parts | Set ohm meter 20M. Com to GND | Set ohm meter 20M. Com to GND | Set ohm meter 20M. Com to GND | Set ohm meter 20M. Com to GND | Set ohm meter 20M. Com to GND |   |   |
| **Test Location** |                                                | TP20 (J12 Pin 1)              | TP 2 (J11 Pin 1)              | J8 Pin 1                      | J3 Pin 4                      | J3 Pin 5                      |   |   |
| **Requirements**  |                                                | > 10K                         | > 10K                         | Open or > 1 Meg               | Open or > 1 Meg               | Open or > 1 Meg               |   |   |
| **Results SN 9**  | D1 and D2 Probably backwards.                  | 34.6K                         | 39.8K                         | Open                          | 12.1 Meg                      | 11.7 Meg                      |   |   |
| **Results SN 10** |                                                | 34.6K                         | 39.9K                         | Open                          | Open                          | Open                          |   |   |
| **Results SN 11** |                                                | 34.5K                         | 39.8K                         | Open                          | Open                          | Open                          |   |   |
| **Results SN 12** |                                                | 34.6K                         | 39.8k                         | Open                          | 2.4K (Issue #126 #126)        | Open                          |   |   |
| **Results SN 13** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 14** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 15** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 16** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 17** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 18** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 19** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 20** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 21** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 22** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 23** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 24** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 25** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 26** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 27** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 28** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 29** |                                                |                               |                               |                               |                               |                               |   |   |
| **Results SN 30** |                                                |                               |                               |                               |                               |                               |   |   |
|                   |                                                |                               |                               |                               |                               |                               |   |   |
|                   |                                                |                               |                               |                               |                               |                               |   |   |
|                   |                                                |                               |                               |                               |                               |                               |   |   |

### Program Due with Firmware
Before connecting to the Controller V1.2 Assembly, program the Due with Load the factory test firmware "MAX31850_Tester.ino" 

## Finish Assembly

#### Build Up MAX31850 Thermocouple Amplifier Sub Assemblies
Note that the side of the board to be up has the pads for configuring the addresses of the assemblies. 
Solder the header and screw terminal to the MAX31850 on the correct side.

1. Install the MAC13850 thermocouple amplifier assemblies to the Control V1.1 at JP9, JP10 and JP15.
Add a ?2.5mm? x 10 mm screw with nut as mechanice support and retention.
Observing polarity, connect thermocouples to the MAX31850 amplifire assemblies.
Cut the four on board shunt on each MAX31850 PCB (See image below for where to cut on one).  Measure with multimeter that the shunt to GND is open.
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/df5c26bd-828b-47e3-a31b-94f947e3e6c9)

3. Add screws for legs such as 8-32 x 2") with nuts.  These legs are necessary because the long stacking connectors to be installed must not be bent which would happen if the legs were not installed.
4. Solder in the stakcing Due connectors, seven locations.  The long tails must go in the top and through to the bottom
5. Connect the Due to the Controller assembly.  The Due is under the Controller V1.1 assembly as a "shield".
6. Connect the Ethernet W5200 (or similar) sheild on the top of the Controller V1.1

### Apply power. 
Note the +12VIn, the +24VIn and the Stack programable power supply are connected to a switchable outlet switch.  
With the power outlet off, 
Connect the +12VIn power at J11, the RED connector.  
Connect the  +24VIn power at J12.  
Connect SSR simulator LEDS at  SSR1, SSR2, SSR3.  
Connect the Pumping/Standby switch at J801, FAULT LED with resistor at J802, and STANDBY LED with resistor at J803.
Connect the Stack power supply at J10 ( or J27) at this time.

Apply power by switching on the AC power strip.
**Test ID7:** Note current in table on the +12VIn supply current monitor for excessive, above 100 mA.  THIS NUMBER WILL NEED TO BE CHECKED WITH DUE's THAT HAVE NEVER BEEN PROGRAMED.
Check that the BUILDIN_LED ner the LAN Shield and SSR2 and SSR3 connectors is blinking rapidly as an idication that the firmware is running.
Open the Arduino Serial Plotter (<Ctrl> <Shift> <L>) and touch some thermocouples.

### Temperature Measurements for SN 1 to SN15.
For SN16 and beyond the test is moved further below and uses the OEDCS firmware
**Test ID8:** Typical Serial Plotter results. Touching some of the thermocouples to ensure they respond.
Note resutls in table
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/e4f5bb6a-2a47-4039-8720-b6846f36b7f7)

**Results continued...**
| **Test ID**       | 7                         | 8                   | 9          |
|-------------------|---------------------------|---------------------|------------|
| **Test Name**     | Current, Temperature Only | Check thermocouples | future use |
| **Setup**         | In line DC current meter  | Connect three       |            |
| **Test Location** | Due input                 | Serial Plolter      |            |
| **Requirements**  | 85 mA                     | Three traces        |            |
| **Results SN 9**  |                           | OK                  |            |
| **Results SN 10** | 270mA                     | OK                  |            |
| **Results SN 11** | 200mA                     | OK                  |            |
| **Results SN 12** |                           |                     |            |
| **Results SN 13** |                           |                     |            |
| **Results SN 14** |                           |                     |            |
| **Results SN 15** |                           |                     |            |



#### Load and run factory test firmware DueWithThreeSSRs.ino  
Open the Arduino Serial Monitor. Observe the data.  
**Test ID10 ** Observe SSR1, SSR2, SSR3 LED Blinking.  
**Test ID11 ** Observe Mini12864 Display back lit and displaying splash message and four measurements.  
**Test ID12 ** Encoder Rotation Knob is back lit. Rotate the know and observe the serial port reporting changes.  
**Test ID13 ** Encoder knob switch. Press the knob and observer serial port message about shutdown.  
**Test ID14 ** PCB mount SHUT DOWN button. Press the button and observer serial port message about shutdown.  
**Test ID15 ** Observer serial port message about "Link Status: ON"  
**Test ID16 ** Stack Voltage, SKIP after SN15 Observer serial port message about "?????"  
**Test ID17 ** Stack Power,  SKIP after SN15 Observer serial port message about "?????"  
**Test ID18 ** Cumulocity,  SKIP after SN15 Observer serial port message about "?????"  
**Test ID19 ** Reset, Observer serial port message as DUT reboots"  
**Test ID20 ** PCB mount SHUT DOWN button. Press the button and observer the Battery Connection LED lights.  
**Test ID21 ** Pumping/StandBy with to Pumping (Blue LED ON) PCB mount SHUT DOWN button. Press the button and observer the Blower goes to high speed STATUS_LED is on.  
**Test ID22 ** Pumping/StandBy with to Standby (Blue LED off) PCB mount SHUT DOWN button. Press the button and observer the Blower is off, STATUS_LED is oFf.  



**Results ID 10-21 continued...**

| **Test ID**       | 10                            | 11                | 12                    | 13                   | 14                   | 15               | 16                         | 17                  | 18                     | 19             | 20                     | 21                  | 22                                                      |
|-------------------|-------------------------------|-------------------|-----------------------|----------------------|----------------------|------------------|----------------------------|---------------------|------------------------|----------------|------------------------|---------------------|---------------------------------------------------------|
| **Test Name**     | SSR1,  SSR2,  SSR3, LED Blink | Mini12864 Display | Encoder Rotation Knob | Encoder knob  switch | PCB button SHUT DOWN | LAN   CONNECTION | Stack voltage @ reset < 1V | Stack power to load | Cumulocity   reporting | Reset to  safe | Connection   @ Battery | Blower              | Pump/ Standby                                           |
| **Setup**         |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | Press  SHUT DOWN       | Press  SHUT DOWN    | Blue  Light On                                          |
| **Test Location** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Requirements**  |                               |                   |                       |                      |                      | Link OK          |                            |                     |                        |                | Battery  LED           | Blower  High  Speed | Blower On When  ShutDown  STATUS_LED  on when  pumping. |
| **Results SN 9**  | OK                            |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Results SN 10** | OK                            | OK                | OK                    | OK                   | OK                   | OK               | OK                         | OK                  | OK                     | OK             | NA                     |                     |                                                         |
| **Results SN 11** | OK                            | OK                | OK                    | OK                   | OK                   | OK               | OK                         | OK                  | OK                     | OK             | NA                     |                     |                                                         |
| **Results SN 12** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Results SN 13** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Results SN 14** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Results SN 15** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                | NA                     |                     |                                                         |
| **Results SN 16** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 17** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 18** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 19** | OK                            | NA                | NA                    | NA                   | OK                   | OK               | Return "SL_Power"          | NA                  | NA                     | NA             | OK                     | OK                  |                                                         |
| **Results SN 20** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 21** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 22** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 23** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 24** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 25** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 26** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 27** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 28** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 29** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
| **Results SN 30** |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
|                   |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
|                   |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |
|                   |                               |                   |                       |                      |                      |                  |                            |                     |                        |                |                        |                     |                                                         |

### Stack Power Supply Control 
With AC power on.

Load the OEDCS firmware into the unit under test.  
For the firmware to report the MAC address, before compiling, set the ETHERNET_REQUIRED define for 1
In the main.cpp file lines 75 and 76 are:
...
#define ETHERNET_REQUIRED 1
//#define ETHERNET_REQUIRED 0 //No ethernet.  FLE
...

Once the fimrware is loaded, open the serial monitor. 
Observe the traffice and note that a Power supply  is reported.  
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/f4dca5d2-eb4a-4583-9b26-30fd3afd920a)  
Observe the traffice and note that an IP address is reported.  
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/607dcabb-5c68-43cf-8c0f-e29e81acb0e4)  
On serial port monitor confirm device connects to LAN **ID16**.  

#### Set Stack power supply and +24VIn.  
Connect a 1.5K 1/2 wat resistor load on the Stack Power Supply ouput.
With multi meeter observe the voltage on the stack power supply load.  
After observe and record voltage **ID16** after reset it should be 1V the power on voltage.
Send OEDCS serial port commands to set Stack power to load manual.
* s:1
* a:0.1
* w:2.5
Observe and record **ID17** Stack power to load should now be about 12V

### Observe data on Cumulocity  
https://mcogs.us.cumulocity.com/apps/app-builder/index.html#/application/34000/dashboard/71202350/device/45175917
![image](https://github.com/PubInv/NASA-MCOG/assets/5836181/2a72fef9-9d39-459b-a6e8-70a8755b9307)
Observe and report in issue **ID19**. 

### Reset to Safe
Press and hold the reset button.
Observe that the Stack power supply voltage goes to zero.  (IT DID NOT as of 20231218 with TF800A12K)
Observe that the blower stops.
Observe that the SSR LEDs go off.
Recorde results in test issue **ID19**  

### Capture MAC, Send for Server Setup
After reset and connection to a LAN the Serial Monitor report displays the device MAC address. Catpure to the table below.

| **Unite Serial Number** | **MAC Address**   | **Note** |
|-------------------------|-------------------|----------|
| 1                       | FE:ED:1B:70:08:77 |          |
| 2                       | FE:ED:10:69:73:70 |          |
| 3                       | NA                |          |
| 4                       | NA                |          |
| 5                       | NA                |          |
| 9                       | FE:ED:1D:70:05:6D |          |
| 10                      | FE:ED:12:71:06:65 |          |
| 11                      | FE:ED:16:6B:72:67 |          |
| 12                      | FE:ED:17:73:0E:69 |          |
| 13                      |                   |          |
| 15                      | ??                |          |
| 16                      | FE:ED:74:66:03:6E |          |
| 17                      | FE:ED:16:73:00:65 |          |
| 18                      | FE:ED:13:70:06:6D |          |
| 19                      | FE:ED:1D:70:00:6D |          |
| 20                      | FE:ED:74:66:03:67 |          |
| 21                      | FE:ED:10:73:00:6F |  FAILED?        |
| 22                      | NA                |          |
| 23                      | NA                |          |
| 24                      | NA                |          |
| 25                      | NA                |          |
| 26                      | NA                |          |
| 27                      | NA                |          |
| 28                      | NA                |          |
| 29                      | NA                |          |
| 30                      | NA                |          |

At end of shift send all new MAC Addresses for addition ot the server.

### Temperature Measurements.
**Test ID8:** 
Observe on Serial monitor that two thermocouples report abmiant temprature.
![image](https://github.com/user-attachments/assets/481c901c-4578-4e7c-a5c2-71b3fb57a0cd)
Touching some of the thermocouples to ensure they respond.
Note resutls in table on colum 8

**Results continued...**
| **Test ID**       | 7                         | 8                                                           | 9          |
|-------------------|---------------------------|-------------------------------------------------------------|------------|
| **Test Name**     | Current, Temperature Only | Check thermocouples                                         | future use |
| **Setup**         | In line DC current meter  | Connect three                                               |            |
| **Test Location** | Due input                 | Serial Plolter                                              |            |
| **Requirements**  | 85 mA                     | Three traces                                                |            |
| **Control V1.2**  | Skip test                 | Post Heater/Post Stack                                      |            |
| **Results SN 16** | NA                        | Not captured                                                |            |
| **Results SN 17** | NA                        | Not captured                                                |            |
| **Results SN 18** | NA                        | Post Heater C: 20.25 Post Getter C: NA Post Stack  C: 20.50 |            |
| **Results SN 19** | NA                        | Not captured                                                |            |
| **Results SN 20** | NA                        | Post Heater C: 21.75 Post Getter C: NA Post Stack  C: 23.25 |            |
| **Results SN 21** | NA                        |                                                             |            |
| **Results SN 22** | NA                        |                                                             |            |
| **Results SN 23** | NA                        |                                                             |            |
| **Results SN 24** | NA                        |                                                             |            |
| **Results SN 25** | NA                        |                                                             |            |
| **Results SN 26** | NA                        |                                                             |            |
| **Results SN 27** | NA                        |                                                             |            |
| **Results SN 28** | NA                        |                                                             |            |
| **Results SN 29** | NA                        |                                                             |            |
| **Results SN 30** | NA                        |                                                             |            |
|                   |                           |                                                             |            |
|                   |                           |                                                             |            |
|                   |                           |                                                             |            |

### End of Test
Power off the unit under test.
Apply a green permanet lable near the PubInv logo.

## Rejoice another great OEDCS has been born!


