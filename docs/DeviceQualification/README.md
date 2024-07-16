# Summary
Part of a test plan for qualifying the OEDCS (Oxygen Engine Digital Control System) 
Written during the Task6 development effort which is to deliver a controller for "long duration" reliability testing of the OEDCS when attached to a CFC.  

## Scope: AC Drop Out / Brown Out Testing
A test specification and procedure for low AC input, aka Brownout.  
A test specification and procedure for interrupted AC input, aka drop out.  

## Required Test Equipment and Setup
Device under test. The device under test must NOT be receiving power from the USB connection.  Modify (cut up) a USB cable to interrupt the +5V to the DUT.  

In addition to a device under test, 
Both typical and corner case loads for the stack supply 
* Shunt
* Nominal load
* Open 
AC line auto transformer make: Model number: Serial number: 
Device for interrupting the AC power. (A switched outlet is a start. However an AC relay under computer control for set drop out duration and phase is even better.)  
Multi meter for monitoring RAW AC Line Input.  
Multi meter for monitoring AC output of Auto transformer, the input to the OEDCS.  
Multi meter for monitoring +24 power supply at Controller TP20.  
Multi meter for monitoring +12 power supply at Controller TP2.  
Multi meter for monitoring +5 power supply at Controler J3 pin 4.  
Multi meter for monitoring voltage Back Up (Sealed Lead Acid batter) at Controller TP???  
Possible oscilloscope for capturing sequence of events and the unexpected.  
Possible logic analyzer for capturing sequence of events and the unexpected.  
Lab thermometer to capture ambient temperature.  


### Setup For Test
*Describe the solution you'd like* 
*A clear and concise description of what you want to happen.* 
Set up of DUT and testing system in a test state(s) 
Connect multi meters at TP2, TP20, the sealed lead acid battery.
> Screen shot of +5V, TP2 and TP20 
![image](https://github.com/user-attachments/assets/007cb85b-6c81-4a9e-9fe5-3183bd31a677)


Connect OEDCS at front panel (except where noted) to:
1. USB and a PC with a logging terminal program such as TeraTerm
2. NO LAN connection (Future feature expansion)
3. Three or four thermocouples
4. I2C to pressure transducer (Future feature expansion)
5. Blower
6. AC to heater
7. DC to: Stack or shunt or open
8. Through the AC interrupt fixture and the auto transformer connect the AC input **on back panel**

Connect multi meters at +5V, +12 PT2, and +24 TP20.  
Connect current meter in line with Back Up battery.  

## Test Procedure Brown Out
Test conditions and procedures and expected results or information to be captured 
### Test Capture Normal Operation Conditions with Stack Supply: Open Load
During this test, journal how the system fails, the sequence and at what voltages supplies go out of regulation. At what AC input voltage does the controller stop if any?

Capture lab ambient temperature.  
All external power to the device is off.
Connect the USB cable to the DUT and a PC.
Use Serial Monitor the USB to Serial connection to the DUT.  Capture to a TIME STAMPED file with a name reflecting the test a log during the test. Example OEDCS_SN1_NOMINAL_AC_STACK_OPEN.txt 
??? Do we press reset now ???

Apply nominal 120VAC power. 
Monitor system status.  Verify that the controller is operating normally by monitoring the USB serial report.
Capture these parameters at this nominal input.
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition Brown Out with Stack Supply: Open Load
Capture lab ambient temperature.  
Lower AC input voltage, 5V steps may be a good step size. Allow to stabilize for 15 seconds (Verify that this seams to be a good time.) 
At each AC voltage input capture the following:
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition Brown Out Recovery with Stack Supply: Open Load
Capture lab ambient temperature.  
Raise from zero AC input voltage, 5V steps may be a good step size. Allow to stabilize for 15 seconds (Verify that this seams to be a good time.) 
Journal how the system restarts, what is the sequence of each supply. 
At each AC voltage input capture the following:
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

Any pathological behaviours?
On what conditions do the controller become active?  
On what conditions does the +12V supply become active?  
On what conditions does the +24V supply become active?  
Describe how the stack power supply recovers and any intervention required by firmware to be implemented in an issue.

### Test For Condition Brown Out with Stack Supply: Shunt
Change out stack supply for a shunt. Retest same as above 
### Test For Condition Brown Out Recovery with Stack Supply: Shunt
Retest same as above. 

### Test For Condition Brown Out with Stack Supply: Nominal load
Change out stack supply for a shunt. Retest same as above 
### Test For Condition Brown Out Recovery with Stack Supply: Nominal load
Retest same as above. 

## Test Procedure AC Drop Out
Test conditions and procedures and expected results or information to be captured 
It is assumed the system still has normal conditions as per the test above "Test Capture Normal Operation Conditions with Stack Supply"
If not journal again the nominal conditions

### Test Capture Normal Operation Conditions with Stack Supply: Nominal load
During this test, journal how the system fails, the sequence and at what voltages supplies go out of regulation or recover in pathological ways. 
Power will be interrupted for short but increasingly long times to root out pathological behavior. 

All external power to the device is off.
Connect the USB cable to the DUT and a PC.
Use Serial Monitor the USB to Serial connection to the DUT.  Capture to a TIME STAMPED file with a name reflecting the test a log during the test. Example OEDCS_SN1_NOMINAL_AC_STACK_OPEN.txt 
??? Do we press reset now ???

Apply nominal 120VAC power. 
Monitor system status.  Verify that the controller is operating normally by monitoring the USB serial report.
Capture these parameters at this nominal input.
1. AC into ODECS
2. +24 at TP20
3. +12 at TP2
4. V sealed battery
5. +5V at Back Up battery
6. Current drain on the sealed lead acid battery

### Test For Condition Manual AC Drop Out with Stack Supply: Nominal load
#### Minimum Interruption
Assuming using a toggle switch, Interrupt (off then on) the AC as fast as possible at least five times in hopes of sampling several phases of the AC line at the time of interruption.
Observe the system for pathological behaviors.
Did the system operate normally through the interruption?  Describe system operation: 
______________________________________________________________________________________ 
______________________________________________________________________________________ 
______________________________________________________________________________________ 

#### Longer Interruption
Gradually Increase the off time repeating with the same off time for about five times

Observe the system for pathological behaviors.
Did the system operate normally through the interruption?  Describe system operation: 
______________________________________________________________________________________ 
______________________________________________________________________________________ 
______________________________________________________________________________________ 


### Test For Condition Automated AC Drop Out with Stack Supply: Nominal load
An AC Drop Out test system based on OEDCS.
Use an OEDCS with custom firmware for this test function as a AC interrupt fixture with which to test the OEDCS under actual test.
Connect from the AC output of the the drop out text fixture in to the the OEDCS under actual test.
Develop firm ware which can turn off (Open) the solid state relay SSR1 for increasing duration. Extra credit if the firmware can be phased triggered from the AC line. 

#### Interruption profile
Set for minimum phase delay
Set for minimum interruption time
Set for number if interruptions

With zero phase delay, increase interruption time till the system behavior changes and journal that change. Continue to look for pathological behaviors.
Observe the system for pathological behaviors.
Did the system operate normally through the interruption?  Describe system operation: 
______________________________________________________________________________________ 
______________________________________________________________________________________ 
______________________________________________________________________________________ 

With insight from the interruption time increase phase delay from zero through at least 32mS (two AC cycles),
Observe the system for pathological behaviors.
Did the system operate normally through the interruption?  Describe system operation: 
______________________________________________________________________________________ 
______________________________________________________________________________________ 
______________________________________________________________________________________ 
