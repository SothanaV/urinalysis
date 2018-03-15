#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Mcp3208.h>
#include <Wire.h>

#define SPI_CS      15       // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz

MCP3208 adc(ADC_VREF, SPI_CS);

void setup() 
{
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  Serial.begin(115200);
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
}

void loop() 
{
 uint16_t r_current = adc.read(MCP3208::SINGLE_0);
 uint16_t r_volt = adc.read(MCP3208::SINGLE_1);
 uint16_t val_current = adc.toAnalog(r_current);
 uint16_t val_volt = adc.toAnalog(val_current);
 Serial.print("Volt");Serial.print(val_volt);Serial.print(" Current");Serial.println(val_current);
}
