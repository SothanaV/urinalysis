/*
 WiFiEsp example: WebClient
 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.
 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// Emulate Serial1 on pins 6/7 if not present


const char* ssid     = "iot2";                         //Set ssid
const char* password = "12345678";                    //Set Password
const char* Server   = "192.168.137.1";           //set Server Domain or Server ip
const char* port     = "5000";                       //set server port
const char* port2     = "8003";                       //set server port
ESP8266WiFiMulti WiFiMulti;

// Initialize the Ethernet client object


void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  for(int t=5;t>0;t--)
  {
    Serial.println("[SETUP]:"+t);
    delay(1000);
  }
  WiFiMulti.addAP(ssid, password);    //Set SSID and Password (SSID, Password)
  WiFi.begin(ssid, password);         //Set starting for Wifi
  Serial.println(WiFi.localIP());
  Serial.println("You're connected to the network");
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, port2)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    String content = "Hey, just testing a post request.";
    client.println("POST YOUR_RESOURCE_URI HTTP/1.1");
    client.println("Host: SERVER:PORT");
    client.println("Accept: */*");
    client.println("Content-Length: " + content.length());
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    client.println(content);
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}



