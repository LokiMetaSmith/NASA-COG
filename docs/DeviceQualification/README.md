# Summary
Part of a test plan for qualifying the OEDCS (Oxygen Engine Digital Control System)
Writen during the Task6 deveopment effort which is to deliver a controller for "long duration" reliability testing of the OEDCS when attached to a CFC.

## AC Drop Out / Brown Out Testing
A test specification and procedure for interupted AC input, aka drop out.  
A test specification and procedure for low AC input, aka Brownout.  

## Required Test Equipment and Setup
In addition to a device under test with typical and corner case loads,
AC line auto transformer make:       Model number:       Serial number: 
Multimeter for monitoring RAW AC Line Input
Multimeter for monitoring AC output of Autotransformer
Multimeter for monitoring +24 power supply at Controler TP20: 
Multimeter for monitoring +12 power supply at Controler TP???: 
Multimeter for monitoring Sealed Lead Acit batter at Controler TP???: 



*Describe the solution you'd like*
*A clear and concise description of what you want to happen.*
Set up system in a test state(s)
Monitor system status.
Lower AC input voltage
Journal how the system fails, the sequence and at what voltates supplies gor out of regulation. At what AC input voltage does the controller stop if any?
What is the current drain on the sealed lead acid battery

Raise from zero AC input voltage
Journal how the system starts up, what is the sequence of each supply. 
On what conditions does the controller become active?
Describe how the stack power supply recovers and any introvention required by firmware.

