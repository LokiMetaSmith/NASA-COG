## Overview

The `Wiring Harness` diagrams allows for constructing cable harnesses to interconnect various components.
The BOM for the 'Wiring Harness' consists of all components needed to construct a wiring harness.
This can be used to construct a new O:

##  BOM

### Sub-Assemblies
#### Front Panel Assembly
##### Order of Assembly:
* Press fit ON-OFF pump/standby switch
* Press fit ON-OFF Power rocker switch
* Install the four Thermocouple connectors
    * Cut TC hookup wire, Yellow + and Red -
        1. 6"
        2. 8" 
        3. 9.5"
        4. 11.5"
* Install connector labeled "Pressure"
* Install connector labeled "Fan"
    * 
* Install connector labeled "Stack"
* Install connector labeled "Heater"
* Install connector labeled "Fault" LED
* Install On/Flashing/Off LED
* Install connector labeled "LAN"
* Install connector labeled "USB"
* Install ground lugs

#### Rear Panel Assembly
##### Order of Assembly:
* Install C20 using two ''#6-32 x '¾ IN' flat philips screws.
* C20 should be in line with the PSU attached to the Base-Plate
* Add WH1 to panel mounted C20
* Install fan assembly the order is Fan shroud -> Rear Panel -> Fan
* Use plastic screws for the fan assmebly
* Fan pushes air into the case, the arrow goes towards the inside of the case
* Orient the fan such that the wire exits towards the Base-Plate
* Install Ground Lug, such that the ground lug is below C20

#### Base-Plate and Rails Assembly
##### Order of Assembly:
* Install PSU, using three screws
* Install brass lugsfor OEDCS control board
* Install DIN rail
* Install SSR and DIN mounted heatsink
* Install battery holder
* Install 12v and 24v PSU on DIN rail
* Prewire Din rail segments
* Install OEDCS control board
* Install DIN rail segments
* Install 12v SLA batttery
* Hook up wires
* Install side panels
* Install rack mount rail system


#### Top-Plate Assembly
##### Order of Assembly:
* On O2 handling systems, feed through 12v power cable 

#### Order of Assembly
* Base-Plate and Rails Assembly
* Rear Panel Assembly
* Front Panel Assembly
* Top-Plate Assembly


```mermaid
flowchart TD
    oedcs---|Heater|srva
    oedcs---|Fan|blower
    oedcs-.-|USB|usboedcs
    oedcs-.-|Ethernet|ethoedcs
    omega-.-|USBtoSerial|usbtoserial
    rack-.-|USB|usbdaq
    
    subgraph sbc[Single Board Computer]
    usbdaq[DATAQ PN]-.-pyscript["python script to log"]
    sbc[data logging-server]---backend["Backend data logger"]
    end
    
    subgraph omega[Omega Paramagnetic Analyzer]
    sbc[data log-server]---pyscript["python script to log"]
    end
    
    subgraph oedcs[OEDCS]
    [data logging-server]---backend["Backend data logger"]
    end
    
    subgraph rack[6U Rack]
    ggml[llama.cpp]---rpcb[RPC backend]
    end
    style hostn stroke:#66,stroke-width:2px,stroke-dasharray: 5 5
```