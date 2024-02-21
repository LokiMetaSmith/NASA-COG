# OEDCS
Oxygen Engine Digital Control System, enclosure for.

## Notes on Unit Fabrication
* Serial number 1  AKA the Austin Unit.
LAN TYPE: SEEEDS STUDIO W5200
MAC Address: FE:ED:1B:70:08:77  
See images in folder at: https://github.com/PubInv/NASA-COG/tree/develop/docs/OEDCS_June_2023_Version_0.1/Photos
This is the Control V1.0 board. Taken out of service at Austin on about ?20240215?  
Control V1.1 SN 09 installed on 20240220. See its MAC below.

* Serial number 2  
![image](https://github.com/PubInv/NASA-COG/assets/5836181/655e3bbb-fb48-4e31-902a-e10b74c27fa8)  
LAN TYPE: ????
MAC Address: FE:ED:10:69:73:70  
This is the Control V1.0 board. Taken out of service at AmOx on about 20240220  
Control V1.1 SN 12 installed on 20240220. See its MAC below

* Stage II controler  
![image](https://github.com/PubInv/NASA-COG/assets/5836181/a146b12b-d7b1-4ecf-a571-1e2d6c8f414f)  
LAN TYPE: ????
MAC Address: FE:ED:14:71:02:77  
ID = 1361064480,1313945940,842021169,959459378


* Serial Number 9  
LAN TYPE: ARDUINO  
MAC Address: FE:ED:1D:70:05:6D
Control V1.1 SN 09 installed on OEDCS SN1 about ?20240215? 

* Serial Number 10  
LAN TYPE: ARDUINO  
MAC Address: FE:ED:12:71:06:65
Control V1.1 in Austin as of about Feb 2024.

* Serial Number 11 Formerly the Mockup Of Maryville
LAN TYPE: SEEEDS STUDIO W5200
![image](https://github.com/PubInv/NASA-COG/assets/5836181/63be5e1f-a040-488d-ab4e-60636bda7cb7)  
MAC Address: FE:ED:16:6B:72:67

* Serial Number 12
LAN TYPE: ARDUINO  
MAC Address: FE:ED:17:73:0E:69
Located at AmOx as of about 20240220 in OEDCS SN2.

* Serial Number 13  
LAN TYPE: ARDUINO 
MAC Address: FE:ED:1A:71:09:7F
Control V1.1 in Oklahoa City as of about ?January 2024.?

## Mechanical Files

* The "frontpanel" folder has FreeCAD drawing which generate the DXF and other files for fabrication of the frontpanel, bottompanel and rear panel.
* The "enclosure" folder has an OpenSCAD drawing of the Unit 1 enclosure. With in the folder is a folder "FreeCADAssy" which has a FreeCAD drawing using the A2plus work bench and duplicates roughly the Unit 1 assembly but which has the Unit 2 frontpanel, bottompanel and rear panel files.

## Front Panel and Enclosure Folders  Unit 2
Place holder for July-August 2023 Begin Construction of Unit 2. 

* Fabrication of Version 2 unit Frontpanel, Bottompanel and Rearpanel not by hand in house. ** DONE!**  
* The Frontpanel Version 2 has enlarged AC receiptical opehnings and will require reciptical cover plates.  **DONE!**  


## Front Panel and Enclosure Folders  Unit 1
A March 2023 enclosure for prototyping, proof of concept for the COG controller. 
This prototype has:
* A programmable power supply (Voltage and current). Power Electrinics, TF800K12A. URL: https://slpower.com/downloads-and-tech-support
* Solid state relays for switching AC power to preheaters. Sparkfun # COM-13015. URL: https://www.sparkfun.com/products/13015  
* Power and PWM control for fans to force air into the COG
* Air Temperature: Adafruit MAX31850 amplifier assemblies. Adafruit Product ID: 1727. URL: https://www.adafruit.com/product/1727
* Air flow rate sensors  NOT MOUNTED
* An Arduino Due for system control.
Navigate into the folders to see more.


Overhead view of Unit 1 wiring as of June - July 2023

![https://user-images.githubusercontent.com/5836181/250552512-68546480-32a5-4593-8a75-600ff2462f9f.png](https://user-images.githubusercontent.com/5836181/250552512-68546480-32a5-4593-8a75-600ff2462f9f.png)
