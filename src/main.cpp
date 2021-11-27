// WordClock with ESP8266 NodeMCU V1.0 ESP-12E WiFI 

// Inspiration
// https://tttapa.github.io/ESP8266/Chap14%20-%20WebSocket.html 
// https://github.com/wouterdevinck/wordclock/blob/master/firmware/wordclock.ino#L35
// https://github.com/basti79/ESP8266-Wordclock/blob/master/ESP8266-Wordclock.ino
// https://werner.rothschopf.net/201802_arduino_esp8266_ntp.htm


#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <WebSocketsServer.h>
#include <time.h>
#include "WClock.h"

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

/* Configuration of NTP */
#define MY_NTP_SERVER "de.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3" // Change timezone https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv


ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81); // create a websocket server on port 81

bool debug = true;
#define SerialDebug(text) Serial.print(text);
#define SerialDebugln(text) Serial.println(text);

char MyIp[16];
char MyHostname[16];
char MyRoom[32] = "";

const char *mdnsName = "wordclock"; // Domain name for the mDNS responder

WClock wclock;
WiFiManager wifiManager;

time_t now;
tm tm;    

int ldrPIN = A0;
int oldLDR = 0;
int ldr = 0;

int updateTime = 0;

int resetPin = 0; // NodeMCU Flash PIN 
int buttonState = 0; 
int lastButtonState = HIGH; 
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time

/*____________________________________________FORWARD DECLARATION__________________________________________________________*/
void startWiFi();
void startSPIFFS();
void startWebSocket();
void startMDNS();
void startServer();
void startClock();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t lenght);
String getContentType(String filename);
bool handleFileRead(String path);
String formatBytes(size_t bytes);
void handleNotFound();
void brightness();
void resetCredentials();


/*__________________________________________________________SETUP__________________________________________________________*/
void setup()
{
  Serial.begin(9600);
  delay(100);

  configTime(MY_TZ, MY_NTP_SERVER);

  startWiFi();
  startSPIFFS();
  startWebSocket();
  startMDNS();
  startServer();
  startClock();

  pinMode(resetPin, INPUT_PULLUP);

  time(&now);   
  localtime_r(&now, &tm);

}

/*__________________________________________________________LOOP__________________________________________________________*/
void loop()
{
  MDNS.update();
  webSocket.loop(); // constantly check for websocket events
  server.handleClient();

  brightness();
  
  static const unsigned long REFRESH_INTERVAL = 1000; // ms
  static unsigned long lastRefreshTime = 0;

    if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
        lastRefreshTime += REFRESH_INTERVAL;
        time(&now);                       // read the current time
        localtime_r(&now, &tm);
        if(updateTime != tm.tm_min){
          Serial.print(tm.tm_hour);
          Serial.print(":");
          Serial.println(tm.tm_min);
        }
        updateTime = tm.tm_min;      
        wclock.showTime(tm.tm_hour, tm.tm_min, tm.tm_sec);
        wclock.update();
    }



 

  

  resetCredentials();
}

/*__________________________________________________________SETUP_FUNCTIONS__________________________________________________________*/
void startWiFi()
{
  wifiManager.setDebugOutput(debug);
  wifiManager.setTimeout(3 * 60);

  if (!wifiManager.autoConnect("WordClock"))
  {
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  IPAddress MyIP = WiFi.localIP();
  snprintf(MyIp, 16, "%d.%d.%d.%d", MyIP[0], MyIP[1], MyIP[2], MyIP[3]);
  snprintf(MyHostname, 15, "ESP-%08x", ESP.getChipId());
  SerialDebug("ESP-Hostname: ");
  SerialDebugln(MyHostname);

}

void startSPIFFS()
{                 // Start the SPIFFS and list all contents
  SPIFFS.begin(); // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    { // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

void startWebSocket()
{                                    // Start a WebSocket server
  webSocket.begin();                 // start the websocket server
  webSocket.onEvent(webSocketEvent); // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}

void startMDNS()
{
  if (!MDNS.begin(mdnsName))
  { // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer()
{
  server.onNotFound(handleNotFound); // if someone requests any other file or page, go to function 'handleNotFound'
                                     // and check if the file exists

  server.begin(); // start the HTTP server
  Serial.println("HTTP server started.");
}

void startClock()
{
  Serial.println("Start Clock");
  wclock.begin();
  wclock.resetAndBlack();
}

/*__________________________________________________________SERVER_HANDLERS__________________________________________________________*/
void handleNotFound()
{ // if the requested file or page doesn't exist, return a 404 not found error
  if (!handleFileRead(server.uri()))
  { // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";                    // If a folder is requested, send the index file
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {                                                     // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                      // If there's a compressed version available
      path += ".gz";                                    // Use the compressed verion
    File file = SPIFFS.open(path, "r");                 // Open the file
    size_t sent = server.streamFile(file, contentType); // Send it to the client
    file.close();                                       // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path); // If the file doesn't exist, return false
  return false;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t lenght)
{ // When a WebSocket message is received
  // Serial.println("Message income!");
  switch (type)
  {
  case WStype_DISCONNECTED: // if the websocket is disconnected
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  { // if a new websocket connection is established
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
  }
  break;
  case WStype_TEXT: // if new text data is received
    Serial.printf("[%u] get Text: %s\n", num, payload);
    if (payload[0] == '#')
    {                                                                       // we get RGB data
      uint32_t rgb = (uint32_t)strtol((const char *)&payload[1], NULL, 16); // decode rgb data
      int r = rgb >> 16;
      int g = rgb >> 8 & 0xFF;
      int b = rgb & 0xFF;

      // Serial.print("RGB: (");
      // Serial.print(r);
      // Serial.print(", ");
      // Serial.print(g);
      // Serial.print(", ");
      // Serial.print(b);
      // Serial.println(")");
      wclock.setRGBColor(r, g, b);
      webSocket.sendTXT(0, "OK");
      delay(20);
      wclock.update();
    }
    break;
  }
}

/*__________________________________________________________HELPER_FUNCTIONS__________________________________________________________*/
String formatBytes(size_t bytes)
{ // convert sizes in bytes to KB and MB
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
   return "";
}

String getContentType(String filename)
{
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void brightness()
{
  // https://arduino.stackexchange.com/questions/19787/esp8266-analog-read-interferes-with-wifi
  // https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/
  //https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/
  if (millis() % 50 != 0)
    return;

  ldr = analogRead(ldrPIN);
  ldr = map(ldr, 0, 1023, 95, 255);

  if (ldr < (oldLDR * 0.9) || ldr > (oldLDR * 1.1))
  {
    oldLDR = ldr;
    wclock.setBrightness(ldr);
    Serial.print("LDR: ");
    Serial.println(ldr);
  }

}

void resetCredentials(){
  // Use  Flash button on NodeMCU to reset the wifi credential
  // https://arduino.stackexchange.com/questions/55235/nodemcu-use-flash-button-as-input-in-loop/55236
  //https://www.arduino.cc/en/tutorial/debounce

  int reading = digitalRead(resetPin);

 if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
         Serial.println("Reset WiFi Credentials");
         wifiManager.resetSettings();
         ESP.restart();
      }
    }
  }
  
  lastButtonState = reading;
}
