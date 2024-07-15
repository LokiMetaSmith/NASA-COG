# Summary
Part of a test plan for qualifying the OEDCS (Oxygen Engine Digital Control System)  
Writen during the Task6 deveopment effort which is to deliver a controller for "long duration" reliability testing of the OEDCS when attached to a CFC.  

## Scope: AC Drop Out / Brown Out Testing
A test specification and procedure for low AC input, aka Brownout.  
A test specification and procedure for interupted AC input, aka drop out.  

## Required Test Equipment and Setup
Device under test. The device under test must NOT be receiving power from the USB connection.  Modify (cut up) a USB cable to interupt the +5V to the DUT.  

In addition to a device under test, 
Both typical and corner case loads for the stack supply (Shunt, Nominal load, Open)  
AC line auto transformer make:       Model number:       Serial number: 
Device for interupting the AC power. (A switched outlet is a start. However an AC relay uncer computer control for set drop out duration and phase is even better.  
Multimeter for monitoring RAW AC Line Input.  
Multimeter for monitoring AC output of Autotransformer, the input to the OEDCS.  
Multimeter for monitoring +24 power supply at Controler TP20.   
Multimeter for monitoring +12 power supply at Controler TP2.  
Multimeter for monitoring +5 power supply at Controler J3 pin 4.  
Multimeter for monitoring Back Up (Sealed Lead Acid batter) at Controler TP???    
Possible oscilliscope for capuring sequence of events and the unexpected.  
Possible logic analizer for capuring sequence of events and the unexpected.  
Lab thermomiter to capture ambiant temprature


### Setup For Test
*Describe the solution you'd like*  
*A clear and concise description of what you want to happen.*  
Set up of DUT and testing system in a test state(s)  
Connect multimeters at TP2, TP20, the sealed lead acid battery.
> Screen shot of +5V, TP2 and TP20  
![image](https://github.com/user-attachments/assets/007cb85b-6c81-4a9e-9fe5-3183bd31a677)


Connect OEDCS at front panel (excpet where noted) to:
1. USB and a PC with a loging terminal program such as TeraTerm
2. NO LAN connection (Future feature expansion)
3. Three or four thermocouples
4. I2C to pressure transducer (Future feature expansion)
5. Blower
6. AC to heater
7. DC to: Stack or shunt or open
8. Through the AC interupt fixture and the autotransformer connect the AC input **on back panel**

Connect multimeters at +5V, +12 PT2, and +24 TP20.  
Connect current meter in line with Back Up battery.  

## Test Procedure Brown Out
Test conditions and proceures and expected results or information to be captured  
### Test Capture Normal Opperation Conditions with Stack Supply: Open Load
During this test, journal how the system fails, the sequence and at what voltates supplies gor out of regulation. At what AC input voltage does the controller stop if any?

All external power to the device is off.
Connect the USB cable to the DUT and a PC.
Use Serial Monitor the USB to Serial connection to the DUT.  Capture to a TIME STAMPED file with an name reflecting the test a log during the test. Example OEDCS_SN1_NOMINAL_AC_STACK_OPEN.txt   
??? Do we press reset now ???

Apply nominal 120VAC power. 
Monitor system status.  Verify that the controler is opperating normaly by monitering the USB serial report.
Capture these paramgters at this nominal input.
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition Brown Out with Stack Supply: Open Load
Lower AC input voltage, 5V steps may be a good step size. Allow to stabalize for 15 seconds (Verify that this seams to be a good time.) 
At each AC voltage input capture the following:
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition Brown Out Recovery with Stack Supply: Open Load

Raise from zero AC input voltage, 5V steps may be a good step size. Allow to stabalize for 15 seconds (Verify that this seams to be a good time.) 
Journal how the system restarts, what is the sequence of each supply. 
At each AC voltage input capture the following:
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

Any pathological behaviours?
On what conditions does the controller become active?  
On what conditions does the +12V supply become active?  
On what conditions does the +24V supply become active?  
Describe how the stack power supply recovers and any interovention required by firmware to be implimented in an issue.

### Test For Condition Brown Out with Stack Supply: Shunt
Change out stack suupply for a shunt. Retest same as above 
### Test For Condition Brown Out Recovery with Stack Supply: Shunt
Retest same as above 

### Test For Condition Brown Out with Stack Supply: Nominal load
Change out stack suupply for a shunt. Retest same as above 
### Test For Condition Brown Out Recovery with Stack Supply: Nominal load
Retest same as above 


foo
## Test Procedure AC Drop Out
Test conditions and proceures and expected results or information to be captured  
It is assumed the system still has normal conditions as per the test above "Test Capture Normal Opperation Conditions with Stack Supply"
If not journal again the nominal conditions

### Test Capture Normal Opperation Conditions with Stack Supply: Open Load
During this test, journal how the system fails, the sequence and at what voltates supplies gor out of regulation. At what AC input voltage does the controller stop if any?

All external power to the device is off.
Connect the USB cable to the DUT and a PC.
Use Serial Monitor the USB to Serial connection to the DUT.  Capture to a TIME STAMPED file with an name reflecting the test a log during the test. Example OEDCS_SN1_NOMINAL_AC_STACK_OPEN.txt   
??? Do we press reset now ???

Apply nominal 120VAC power. 
Monitor system status.  Verify that the controler is opperating normaly by monitering the USB serial report.
Capture these paramgters at this nominal input.
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition AC Drop Out with Stack Supply: Open Load

Lower AC input voltage, 5V steps may be a good step size. Allow to stabalize for 15 seconds (Verify that this seams to be a good time.) 
At each AC voltage input capture the following:
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

