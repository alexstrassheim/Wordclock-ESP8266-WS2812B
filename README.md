# Simple Wordclock-ESP8266-WS2812B

Simple Wordclock - implemented on an ESP8266 with WS2812B LED-Stripes

This is an implementation of the famous Wordclock on an ESP8266 using WS2812B LED-Stripes and LDR. The time is syncronized via NTP.
The ESP8266 provides a simple color picker mobile web application on http://wordclock.local after initial setup. 

It's based on the Arduino core for ESP8266 and needs these libraries:

```
[env:nodemcuv2]
platform = espressif8266@2.6.3
board = nodemcuv2
framework = arduino
lib_deps = 
	fastled/FastLED@^3.4.0
	tzapu/WiFiManager@^0.16.0
	arduino-libraries/NTPClient@^3.1.0
	links2004/WebSockets@^2.3.6
	jchristensen/Timezone@^1.2.4
	paulstoffregen/Time@^1.6.1

```

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
