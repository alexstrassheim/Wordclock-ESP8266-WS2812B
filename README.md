# Wordclock-ESP8266-WS2812B

Wordclock implemented on an ESP8266 with WS2812B LED-Stripes

This is an implementation of the famous Wordclock on an ESP8266 using WS2812B LED-Stripes and LDR. The time is syncronized via NTP.
The ESP8266 provides a simple color picker mobile web application. 

It's based on the Arduino core for ESP8266 and needs these libraries:

- DNSServer
- ESP8266WiFi
- ESP8266WebServer
- ESP8266mDNS
- WiFiManager
- NTPClient
- FS
- WebSocketsServer

Used Font:
- Taurus Monospace Stencil

Frame for the 11x11 Matrix:
- [WS2812B 11x11 Frame on Thingiverse](#)

![alt text][https://raw.githubusercontent.com/alexstrassheim/Wordclock-ESP8266-WS2812B/master/Supplement/Screenshots/prev1.png]