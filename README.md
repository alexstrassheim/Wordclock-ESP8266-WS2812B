# Simple Wordclock-ESP8266-WS2812B

Simple Wordclock - implemented on an ESP8266 with WS2812B LED-Stripes

This is an implementation of the famous Wordclock on an ESP8266 using WS2812B LED-Stripes and LDR. The time is syncronized via NTP.
The ESP8266 provides a simple color picker mobile web application on http://wordclock.local after initial setup. 

It's based on the Arduino core for ESP8266 and needs these libraries:

- DNSServer
- ESP8266WiFi
- ESP8266WebServer
- ESP8266mDNS
- WiFiManager
- NTPClient
- FS
- WebSockets
- Timezone

## Used Font:
- Taurus Monospace Stencil

## Frame for the 11x11 Matrix:
- [WS2812B 11x11 Frame on Thingiverse](https://www.thingiverse.com/thing:4088421)

## Some Images:
![Alt text](Supplement/Screenshots/preview.gif?raw=true "Title")
![Alt text](Supplement/Screenshots/prev1.png?raw=true "Title")
![Alt text](Supplement/Screenshots/WebApp4.PNG?raw=true "Title")
![Alt text](Supplement/Screenshots/face.png?raw=true "Title")
![Alt text](Supplement/Screenshots/Breadboard.png?raw=true "Title")
