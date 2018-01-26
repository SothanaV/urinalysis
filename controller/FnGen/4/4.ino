#include <SPI.h>
#include <Mcp3208.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#define SPI_CS      15       // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz

MCP3208 adc(ADC_VREF, SPI_CS);
Adafruit_MCP4725 dac;
void readA();
void setup() 
{
   pinMode(SPI_CS, OUTPUT);
   digitalWrite(SPI_CS, HIGH);
   Serial.begin(115200);
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
  dac.begin(0x64);

}

void loop() 
{
float vmin = 0;
float vmax = 800;
float vcc = ADC_VREF;
int pw = 50;
int t = 100;
float a = 50;
float e = 10;
    float v = vmin;
    while(v<vmax)
    {
      v = v+a;
      int x = (int)((v/vcc)*4095.0f);
      dac.setVoltage(x, false);
      Serial.print(v);
      readA();
      delay(pw);
      v=v-(a-e);
      x = (int)((v/vcc)*4095.0f);
      dac.setVoltage(x, false);
      Serial.print(v);
      readA();
      delay(t-pw);
    }

}
void readA()
{
  uint16_t raw = adc.read(MCP3208::SINGLE_0);
  uint16_t val = adc.toAnalog(raw);
  //Serial.print("value: ");
  //Serial.print(raw);
  Serial.print(" (");
  Serial.print(val);
  Serial.println(" mV) ");
}

