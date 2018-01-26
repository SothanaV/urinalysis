#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;
void setup() 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  dac.begin(0x64);
  Serial.println("Generating a triangle wave");
}

void loop() 
{
  
    dac.setVoltage(4095, false);
    delay(500);
    dac.setVoltage(0, false);
    delay(500);   
}
