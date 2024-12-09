# Dec. 7th High temperature test

After improving the RPM envelope safety feature, I performed a high-temperature test on Dec. 6th.
Upon awaking on Dec. 7th, the system was operating but appeared to be stuck, with post-getter temperature at 474 and unable to climb.
The SetPoint seemed to keep rising, which appears to be a bug.

```
Timestamp: 74545653
Machine State: Warmup
Target     C: 750.00
Setpoint   C: 645.63
Target Ramp C: 0.50
Max Stack   A: 30.00
Max Stack   W: 250.00
Fan PWM   0-1: 0.65
Fan Target % : 80.00
Post Heater C: 535.50
Post Getter C: 473.75
Post Stack  C: 505.25
Heater DC 0-1: 1.00000
Stack amps  A: 0.07
Stack watts W: 0.11
Stack volts V: 1.64
Stack ohms  O: 23.43
Fan RPM      : 8479.00
analogRead(SENSE_12V)= 845
13.63
12V bus voltage = 13.63
```

The system is stuck because the Post Stack temperature - Post Getter Temperature is greater than 30 degrees, the edege difference limit.
Note that the Post Getter temperature is lower than the Post Stack temperature, which I believe is because of air leaks around the stack in the Austin CFC.

I am also truouble by the fact that the stack volts are only 1.89. We should at this temperature be able to put more head directoy into the stack.

It is possible that as the general system warms up, it may be able to get "unstuck", but since the setpoint is 100 degrees higher than the stack temperature,
I believe it has been stuck for 200 minutes = 3 hours and 20 minutes.

I decide to increase the stack edge limit to 40C with the absolute limit set to 60C.

At 7:30 I realized I had the Post Stack and the Post Getter thermocouples mistaken for each other. I swapped them.

at 7:31, the system Stad "STARTING DUMPING LOG RECORDS: 10" but abarted in the middle of that -- this is unexplained buggy behavior.

After rearranging the thermocouples, I let the system run. I am concerned that the stack is not receiving 12Volts, even though at this point in the algorithm I think it should.

at 10:00 pm I enterred cooldown mode. I was unable to get above these temperatures:

```
Timestamp: 50970480
Machine State: Cooldown
Target     C: 50.00
Setpoint   C: 582.17
Target Ramp C: 0.50
Max Stack   A: 30.00
Max Stack   W: 250.00
Fan PWM   0-1: 0.63
Fan Target % : 40.00
Post Heater C: 576.50
Post Getter C: 549.50
Post Stack  C: 510.75
Heater DC 0-1: 1.00000
Stack amps  A: 1.24
Stack watts W: 6.08
Stack volts V: 4.90
Stack ohms  O: 3.95
Fan RPM      : 8391.00
analogRead(SENSE_12V)= 845
```

Monday morning, I tried to excute this command:

```
pio device monitor --filter=direct --baud=115200 2>&1 | tee -a oedcs.logfile.txt
```

After unplugging and replugging the cable and performing "make" it connected.  They ssytem was down to 50C:

```
Timestamp: 884342
Machine State: Off
Target     C: 30.00
Setpoint   C: 30.00
Target Ramp C: 0.50
Max Stack   A: 0.00
Max Stack   W: 0.00
Fan PWM   0-1: 0.00
Fan Target % : 0.00
Post Heater C: 51.25
Post Getter C: 50.50
Post Stack  C: 44.25
Heater DC 0-1: 0.00000
Stack amps  A: 0.00
Stack watts W: 0.00
Stack volts V: 0.00
Stack ohms  O:  N/A
Fan RPM      : 0.00
analogRead(SENSE_12V)= 846
13.65
12V bus voltage = 13.65
```




