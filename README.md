# esp8266-simple

A simple Arduino library for interfacing Arduino Mega with an ESP8266-01. Most read/write functions have the same method as their Ethernet counterparts. For now, comments explain the various functions (examples will be up soon).

Things to note:
- I have used Serial3 since it was the most accessible serial port on the Mega, leaving the others free for use. 
- Remember to change that port if you are using another Serial port for communication with the ESP8266.
- The ESP8266 uses a default baud rate of 115200. Change it if you're using something from another manufacturer. 
