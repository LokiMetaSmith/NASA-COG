Put the tests here.


Requirements and notes
Test Names,  Procedure and Results
1. Blower loss of power - Passed by (Initials) on (YYYYMMDD)
2. Blower stuck on  - Passed by (Initials) on (YYYYMMDD)
3. Heater loss of power - Passed by (Initials) on (YYYYMMDD)
4. System over temperature - (Absolute Max temperature) - Passed by (Initials) on (YYYYMMDD)
5. DC loss of power - (Loss of power to the Stack Supply or control) - Passed by (Initials) on (YYYYMMDD)
6. Cell stack overpower conditions - Passed by (Initials) on (YYYYMMDD)
7. Loss of service power - Passed by (Initials) on (YYYYMMDD)
8. Regular Log at 10 Minute Intervals, - Passed by (Initials) on (YYYYMMDD)
9. History @ Critical Fault, Records at 1 Sec, previous 10 Min.  - Passed by (Initials) on (YYYYMMDD)
Implementation Notes


Objective: The purpose of this document is to define the tests and capture test results to meet the requirements for MCOG Task 2. This document and the serial monitor files and log files from the report. 
Document, Version 1 |  Revision Date: Jan 9, 2024
Requirements and notes
Test with an ODECS composed of a Control V1.1 with all modifications

All of these requirements must be detected even if the abstract machine is in the “Off” state.
The system has 120vAC plugged in, and has been “initialized-” “says something on the Serial Terminal to 
Results will be labeled as “ - Passed by (Initials) on (YYYYMMDD)”
Results will list the FW version and hardware revision, as well as deviations such as PSU version
Test will list the load conditions, this is the “plant under control”
This procedure requires capturing files both from the Serial Monitor to the Due and normal and critical fault log files
These files will be organized by the firmware version against which the test is made and place in the repository at:     TBD file path
On a test failure a github Issue will be generated describing the failure with sufficient information to duplicate the failure.



Test Names,  Procedure and Results
Blower loss of power - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin plant except modified to interrupt only the +24 supply.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_1BLP_rev3.11 to indicate regression test serial monitor Blower Loss of Power test of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS: a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warm up phase on serial monitor. ___________
Note time and interrupt +24 supply.  Fan will spin down.
Observe the system reports loss of +24V on serial monitor  with the text “AC Power (+24V) FAIL“ and at what time: ___________
Observe the reports critical shutdown on serial monitor and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.
Disconnect the 24v power supply while in a Warm-Up phase
Observing the system response, did the system correctly detect loss of power to the fan?

Blower stuck on  - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Multi meter measuring voltage at Test point Switched_Fan_Power relative to ground.

Firmware: exact version
Test preconditions: OEDCS connected to Austin plant except modified to interrupt the PWM wire to the blower.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_2BSO_rev3.11 to indicate regression test serial monitor 2 Blower Stuck On of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warm up phase on serial monitor. ___________
Note time and interrupt (Open) the Blower PWM connection.  Blower will go to full speed.
Observe the system reports blower stuck on on serial monitor  with the text “??????? FAIL“ and at what time: ___________
Observe the system turn turns off FAN POWER at the test point,    ___________
Observes that the system reports critical shutdown on serial monitor and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.

Disconnect just the PWM pin (This should allow the PWM pin to float HIGH)
Observing the system response, did the system correctly detect the blower is now unresponsive and stuck on?

Heater loss of power - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin plant except modified to interrupt only the power to the heater (Unplug the heater).

To pass the test must detect Critcialfault and correctly respond for all three thermocouple over temprature conditions in all three operating conditions
Loss of AC Power During Warmup
Loss of AC Power During NormalOperation 
Loss of AC Power During Cooldown
The system must enter Criticalfault condition and shut down for each

Start of Warmup experiment:
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_3HLP_WP_rev3.11 to indicate regression test serial monitor Heater Loss of Power during Warm Up test of firmware 3.11).
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warm up phase on serial monitor. ___________
(Do we need to wait to interrupt AC?  What if the AC power is disconnected at the start?)
Note time and interrupt the AC power at the OEDCS to the heater.  
Observe the system reports ????? on serial monitor  with the text “??????“ and at what time: ___________
Observe the reports critical shutdown on serial monitor and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.

Start of NormalOperation  experiment:
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_3HLP_SS_rev3.11 to indicate regression test serial monitor Heater Loss of Power during NormalOperation  test of firmware 3.11).
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warms up and enters steady state on serial monitor. ___________
Note time and interrupt the AC power at the OEDCS to the heater.  _______________________
Observe the system reports ????? on serial monitor  with the text “??????“ and at what time: ___________
Observe the reports critical shutdown on serial monitor and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.

Start of Cooldown experiment:
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_3HLP_CD_rev3.11 to indicate regression test serial monitor Heater Loss of Power during Cooldown test of firmware 3.11).
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warms up and enters steady state on serial monitor. ___________
Command the system to Cooldown   (HOW?   a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 )
Note time time of the cool down command _______________________
Wait TBD 
Disconnect the AC power to the heater.
Note time time of the AC disconnect _______________________
Observe the system reports ????? on serial monitor  with the text “??????“ and at what time: ___________
Observe the reports critical shutdown on serial monitor and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.


Unplug the heater power cord from the GFCI outlet
Observing the system response, did the system correctly detect loss of power to the heater?
Note: This should only detect the unresponsiveness of the heater itself, not the entire system. 
If the heater is on and the A thermocouple temperature is going down, it is most likely the case that the power has failed. This might not be true at extremely high operating temperatures, but will be true under normal circumstances. The A thermocouple will not stay hot for long if the heater has lost power, so a test like “temperature goes down when on and below 700C” should be a robust test. I recommend this be tested in the DutyCycleTask, because that is where the heater is turned on.



System over temperature - (Absolute Max temperature) - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions:  The OEDCS will be connected to a normal or simulated plant in such a way that each thermocouple can be accessed (removed and relocated OR a substitute thermocouple connected) so that a heat source can be applied, one at a time, to take the thermocouple over temperature.
System over temperature test will be made with the system in the NormalOperation mode. Over temperature test will be made one at a time. After each over temperature the system will be operated to return to NormalOperation. 

To pass the test the system must enter Criticalfault condition and shut down for each thermocouple over temperature condition.
Over temp Heater thermocouple to TBD degrees C
Over temp Getter thermocouple to TBD degrees C
Over temp Stack (Kiln)  thermocouple to TBD degrees C

Start of Over Temperature experiment:

Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_4SOT_rev3.11 to indicate regression test serial monitor System Over Temperature during Warm Up test of firmware 3.11).  Note that as written this assumes capture into one file of all three tests. If impractical make three files indicating Heater, Getter or Stack in the file name.
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  TBD???   a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1
Start and verify system reports warm up phase on serial monitor. ___________
Verify system reports enter NormalOperation on serial monitor. ___________

Heater Thermocouple test
Note time and replace the “heater” thermocouple - A with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun to TBD degrees C.
Observe the system reports Criticalfault  on serial monitor  with the text “??????“ and at what time: ___________
Restore the normal system thermocouple couple connection and restart the system and allow the system to return to NormalOperation.

Getter Thermocouple test
Note time and replace the “heater” thermocouple - A with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun to TBD degrees C.
Observe the system reports Criticalfault  on serial monitor  with the text “??????“ and at what time: ___________
Restore the normal system thermocouple couple connection and restart the system and allow the system to return to NormalOperation.

Stack Thermocouple test
Note time and replace the “heater” thermocouple - A with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun to TBD degrees C.
Observe the system reports Criticalfault  on serial monitor  with the text “??????“ and at what time: ___________
Restore the normal system thermocouple couple connection and restart the system and allow the system to return to NormalOperation.
Set for Cooldown the system

Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.
Replace the “heater” thermocouple - A with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun
Replace the “getter”  thermocouple - B with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun
Replace the “stack”  thermocouple - C with a thermocouple attached or coupled to a remote thermal load such as a Kiln, or a heat gun
Bring the system up such that it is in a “Normal Operations” state 
The programmable power supply may need a power resistor and the fan may need to be installed 
Simulated thermal runaway, by removing one of the thermocouples and heating it with a heat gun such that the thermocouples  are heated to simulate a thermal runaway
Do this with all three thermocouples A, B and C
Observe the system response, did the system correctly detect over temperature?
Trips in read thermocouple even if the CogTask is delayed
This error is thrown even if the Abstract Machine is in the “off” state
DC loss of power - (Loss of power to the Stack Supply or control) - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin plant except modified to allow interruption of only the AC supply into the Stack power supply.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_5DCLOP_rev3.11 to indicate regression test serial monitor DC Loss of Power test of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  s:1 automatic (? a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 ???)
Start and verify system reports Warmup phase on serial monitor. ___________
Start and verify system reports Normaloperation phase on serial monitor. ___________
Note time and interrupt AC supply to the Stack supply.  
Observe the system Criticalfault and reports loss of AC Power to Stack supply and shutdown on serial monitor  with the and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.
The Stack programmable power supply, monitors the voltage and current directly 
Disconnect the DC power supply, power cable
Observe the system response, did the system correctly detect loss of communication to the power supply?


Cell stack overpower conditions - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin plant except modified to allow a low impedance over current load to be placed on the Stack Power supply.  A load capable of safely conducting > 60 Amps is required as is a 60 Amp switch with which to connect the load.  The switched load will be connected  in parallel with the stack or simulated stack.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_6SOP_rev3.11 to indicate regression test serial monitor Stack Over Power test of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________.
Reset system
System Commands to OEDCS:  s:1 automatic (? a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 ???)
Start and verify system reports Warmup phase on serial monitor. ___________
Start and verify system reports Normaloperation phase on serial monitor. ___________
Note time and apply over current load to the Stack supply.  
Observe the system Criticalfault and reports ???????? supply and system enters shutdown on serial monitor  with the and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.


The system should be in the “WarmUp”, “CoolDown”, or “NormalOperation” state
Connect a dummy load to the programmable power supply that can exceed the 300 watt (per AmOx) limit
Apply the dummy load across the terminals, simulating an over power condition
Observe the system response, did the system correctly detect the overpower simulated stack?

Loss of service power - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin plant through a switch to interrupt all AC power.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_7LSP_rev3.11 to indicate regression test serial monitor Loss of Service Power test of firmware 3.11).

Before the start of the experiment, verify that the battery power to the Due (control V1.1) is charged so that the Due has power throughout this experiment.
Start of experiment:
Start serial monitor capture.  Actual file name: ____________________. 
Reset system
System Commands to OEDCS:  s:1 automatic (? a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 ???)
Start and verify system reports Warmup phase on serial monitor. ___________
Start and verify system reports Normaloperation phase on serial monitor. ___________
Note time and interrupt the AC service power to the system.
Observe the system Criticalfault and reports ???????? supply and system enters shutdown on serial monitor  with the and at what time: ___________
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.


Make sure the UPS switch is engaged
Unplug the OEDCS from the wall outlet
Observe the system response, did the system correctly detect the loss of service power?

Regular Log at 10 Minute Intervals, - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin or simulated plant.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_8RL_rev3.11 to indicate regression test serial monitor RegularLog test of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________. 
Reset system
System Commands to OEDCS:  s:1 automatic (? a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 ???)
Start and verify system reports Warmup phase on serial monitor. ___________
Start and verify system reports Normaloperation phase on serial monitor. ___________
Operate in Normaloperation for TBD minuets / hours.
Check for captured log file(s). 
Review log files for completeness of data.
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.
Cooldown the system.

Observe the system, does the system measure and record system operating parameters – at least, one record every 10 minutes?



History @ Critical Fault, Records at 1 Sec, previous 10 Min.  - Passed by (Initials) on (YYYYMMDD)
Test System Description
Hardware: OEDCS SN 1 with Control V1.1,  Plant: Austin CFC or …
Firmware: exact version
Test preconditions: OEDCS connected to Austin or simulated plant.  Switch on blower PWM line to open PWM control.
Capture the time stamped Serial Monitor output during the test to a file with a descriptive name (regressionSM_8HL_rev3.11 to indicate regression test serial monitor HistoryLog test of firmware 3.11).

Start of experiment:
Start serial monitor capture.  Actual file name: ____________________. 
Reset system
System Commands to OEDCS:  s:1 automatic (? a:1, w: 2.5, f;0.5, h:0.1, r: ??.?,  s:1 ???)
Start and verify system reports Warmup phase on serial monitor. ___________
Start and verify system reports Normaloperation phase on serial monitor. ___________
Operate in Normaloperation for TBD minutes / hours.
Check for normal captured log file(s). 
Note system time and make a critical fault. Switch open the PWM to the blower to make the blower out of control.
Check for critical fault history log. HOW?????  
Review critical fault history log file(s) for completeness of data back ten minuets.
Report success or failure in the named task with the required format. 
Note any unexpected behavior: __________________________________________
Save the serial monitor capture.
Cooldown the system.

Observe the system, does the system measure, and record system operating parameters – one record every 1 second, for the previous 10 minutes.
Observe that the log allows recovery of this data in a reasonable way.
When the test is performed, the tester will make detailed notes on how each of these conditions were tested.



Implementation Notes
List of fault conditions. These are definied in machine.h.

`// WARNING! Do not reorder these!!
// The code currently depends on the 0,1, and 2 being the the thermocouple errors.enum CriticalErrorCondition {
enum CriticalErrorCondition {
POST_HEATER_TC_BAD,
POST_GETTER_TC_BAD,
POST_STACK_TC_BAD,
COULD_NOT_INIT_3_THERMOCOUPLES,
FAN_LOSS_PWR,
PWR_24V_BAD,
FAN_UNRESPONSIVE,
HEATER_UNRESPONSIVE,
HEATER_OUT_OF_BOUNDS,
STACK_LOSS_PWR,
PSU_UNRESPONSIVE,
MAINS_LOSS_PWR
};

constexpr inline static char const *CriticalErrorNames[NUM_CRITICAL_ERROR_DEFINITIONS] = {
"Post Heater TC-A Bad",
"Post Getter TC-B Bad",
"Post Stack TC-C Bad",
"Can not init three TC's",
"Fan Power Loss",
"Lost 24v Power",
"Fan TACH unresponsive",
"Lost control of Heater",
"pid pegged, temp out of bounds",
"Lost control of the Stack",
"Lost control of the programmable PSU",
"Lost mains power, on UPS"
};`





Equipment List
Multi meter for measuring +24V

Notes:
Condition 4: Test with a heat gun
Temporary Test Load for testing a DC power short, for testing Condition 6:
https://www.harborfreight.com/100-amp-612v-battery-load-tester-61747.html

Condition7: Detect in software by ?????
