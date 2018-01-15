#include <Arduino.h>
#include <ESP8266WiFi.h>
int r = A0;
void setup() 
{
  Serial.begin(9600);
  pinMode(r,INPUT);
}

void loop() 
{
  float x = analogRead(r);
  Serial.print("AnalogRead : ");
  Serial.print(x);
  float voltage= x * (5.0 / 1023.0);
  Serial.print("  Volt : ");
  Serial.println(voltage);
}
