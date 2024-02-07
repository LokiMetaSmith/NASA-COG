## Event driven code for all four Due UARTS.
The loop() is empty of code.
The hardware under test was a TF800 series power supply on UART`. (Caution obout 5V and 3.3V system apply.)


Here a message typed into the Serial Monitor is forwared to UART1. The UART1 event handeler forwards the UART1 response back to the Serial Monitor  
![image](https://github.com/ForrestErickson/EventUARTS_Due/assets/5836181/81d53360-98f0-4554-ba80-62203b256279)

This measures the time from the LF:
1. at the Serial Monitor input  UART0
2. at the UART 1 from the power supply after processing the "***IDN?**" command

The power supply interpreting the command and reporting the output "SL Power, TF Series" took 86 mS.  At 4800 BAUD the 19 characters time 10 bits is 39.5mS
So the processing time was about 86-39.5 or 46mS.
Then 9 mS later the "=>" was printed.  

The power supply interpreting the command "**DEVI?**" took roughtly the same time
![image](https://github.com/ForrestErickson/EventUARTS_Due/assets/5836181/0b949f6c-586c-44f6-9589-b689ec61bb1d)

