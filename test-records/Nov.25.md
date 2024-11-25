# Test of Nov. 24th-Nov. 25th

Hardware Context: OEDCS V1.1 SN9 control board, Austin CFC, plant interface fully wired.

On Nov. 24th and Nov. 25th, we attempted to run a full test to temperature and back down. 

In the middle of the night, at about 500C, 


In the morning, during cooldown, the system erroenously turned itself off due to a TEMPERATURE_OUT_OF_BOUNDS
error. The threshold for this was set to only 20C. 

I believe this limit may be too low---this is an example of something that is likely to be 
quite different for two different CFC designs. I intend to open it.

After waking at 7:00 in the morning and finding it off and the temperature about 150C, (further 
documentation will be provided later), I turned it on and put it into CoolDown mode to 30C.
However, this ALSO turned itself off prematurely due to TEMPERATURE_OUT_OF_BOUNDS.

Upon reflection, this made me believe in a significant improvement. I now believe many
of our errors should enter a controlled CoolDown. This may require us to create a new state,
"CoolDownToOff".  I believe most of these errors, (with the exception of those associated with
a failure of mains power), should take the system to "OFF" which shouold require a human intervention
to restart---this is where our previous creation of the state for just that purpose will pay off.


The failure in the middle of the night was caused by the FanUnresponsive error:

```{verbatim}
Timestamp: 57690153
Machine State: Warmup
Target     C: 750.00
Setpoint   C: 503.18
Target Ramp C: 0.50
Max Stack   A: 30.00
Max Stack   W: 250.00
Fan PWM   0-1: 0.73
Fan Target % : 80.00
Post Heater C: 502.25
Post Getter C: 440.75
Post Stack  C: 471.50
Heater DC 0-1: 1.00000
Stack amps  A: 0.07
Stack watts W: 0.16
Stack volts V: 2.25
Stack ohms  O: 32.14
Fan RPM      : 9031.00
analogRead(SENSE_12V)= 843
13.60
12V bus voltage = 13.60
=======================Fan Inputs : 0.73
9104.00
Fan Fault FAN_UNRESPONSIVE Present: 0
fan_pwm_ratio: 0.73
rpms: 9104.00
rpm_actual: 6060.67
rpm_difference: -3043.33
rpm_tested: 3764.87
Fan Fault Present
BEFORE RUN GENERIC!
Enter s:0 to Turn Off, s:1 to Enter Troubleshooting Mode (NOT SUPPORTED FOR PRODUCTION),
Enter s:3 to Turn Enter Automatic Control (at 750C) or s:2 (set temp with h).
Enter a:XX.X to set (a)mperage limit, (w)attage limit, (h)eater set p.
WILL AUTOMATICALLY SHUTDOWN IF NOT RESTORED IN 179.77 SECONDS DUE TO : Fan TACH unresponsive
calling update Fan Speed!
73.31
XXXX HI_DELTA: 1
XXXX Mode: 1
XXXX Fan Action: Increase
```

Here is the failure at the cool temperature, the post heater more than 20 degrees less than the post getter of 50.5 degrees:

```{verbatim}
Updating Stack Voltage (BBB): 12.00
2320543
Done (BBB): 
2320754
Updating Stack Amperage (CCC):
0.10
AFTER RUN GENERIC!
Free Memory: 28567

Timestamp: 2330004
Machine State: Cooldown
Target     C: 25.00
Setpoint   C: 38.46
Target Ramp C: 0.50
Max Stack   A: 30.00
Max Stack   W: 250.00
Fan PWM   0-1: 0.40
Fan Target % : 40.00
Post Heater C: 43.50
Post Getter C: 50.50
Post Stack  C: 44.00
Heater DC 0-1: 0.00000
Stack amps  A: 0.00
Stack watts W: 0.00
Stack volts V: 11.96
Stack ohms  O: inf
Fan RPM      : 5929.00
analogRead(SENSE_12V)= 845
13.63
12V bus voltage = 13.63
=======================Fan Inputs : 0.40
6031.00
Fan Fault FAN_UNRESPONSIVE Present: 0
fan_pwm_ratio: 0.40
rpms: 6031.00
rpm_actual: 4263.21
rpm_difference: -1767.79
rpm_tested: 3106.08
TESTING ENVELOPE
TEMP BOUND EXCEEDED
5.04
Heater Fault Present
BEFORE RUN GENERIC!
Enter s:0 to Turn Off, s:1 to Enter Troubleshooting Mode (NOT SUPPORTED FOR PRODUCTION),
Enter s:3 to Turn Enter Automatic Control (at 750C) or s:2 (set temp with h).
Enter a:XX.X to set (a)mperage limit, (w)attage limit, (h)eater set p.
WILL AUTOMATICALLY SHUTDOWN IF NOT RESTORED IN 179.77 SECONDS DUE TO : pid pegged, temp out of bounds
Cooldown Mode!
calling update Fan Speed!
39.90
```


