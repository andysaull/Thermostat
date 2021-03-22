# Thermostat
Arduino thermostat using touchscreen LCD



## **About this project** 📜

This project was made for upgrade an old thermostat and control all the possible parameters connecting with a simple natural gas boiler.

The thermostat achieves simple automation with a graphic temperature display.

This was a personal work, and I added custom features that I wanted for me, like a theme editor and a date reminder (like new year or birthdays in my family, so the thermostat can congratulate you 😁)

![image](https://user-images.githubusercontent.com/80455606/112032714-f8004d00-8b3c-11eb-8570-a077faad3405.png)

_Main menu_



## **Components** 🛒
[Arduino Mega 2560](https://store.arduino.cc/arduino-mega-2560-rev3)

[LCD TFT 2.8" (ILI9341)](https://es.banggood.com/2_8-Inch-TFT-LCD-Shield-Touch-Display-Screen-Module-Geekcreit-for-Arduino-products-that-work-with-official-Arduino-boards-p-989697.html?akmClientCountry=ES&rmmds=myorder&cur_warehouse=CN)

[Relay Shield (SRD-05VCD-SL-C)](https://es.banggood.com/5V-1-Channel-Level-Trigger-Optocoupler-Relay-Module-p-915614.html?akmClientCountry=ES&rmmds=myorder&cur_warehouse=CN)

[DHT22 Temperature sensor](https://www.sparkfun.com/products/10167)


## **Assembly and connections** 🔌
The touchscreen is an Arduino especific shield so you just need to connect it on top of the Arduino.

Relay shield must be connected with wires following a simple diagram:


**Relay shield    --    Arduino**

DC+           ->    5V

DC-           ->    GND

IN            ->    31


**Relay shield    --    Boiler**

COM           ->    COM

NO            ->    NO



I made a case for print with a 3D printer but I need to test for myself first. Anyway you can try to connect the thermost where you want.
