#include <SPI.h>
#include <Mcp3208.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define SPI_CS      15       // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz

MCP3208 adc(ADC_VREF, SPI_CS);
Adafruit_MCP4725 dac;
void readA();
void SendData(uint16_t val_current,uint16_t val_volt);

const char* ssid     = "iot2";                         //Set ssid
const char* password = "12345678";                    //Set Password
const char* Server   = "192.168.137.1";           //set Server Domain or Server ip
const char* port     = "5000";                       //set server port
const char* port2     = "8003";                       //set server port
ESP8266WiFiMulti WiFiMulti;

String Vstr = "0" ; 
String Istr = "0" ;
//String json = ''' {"v":"1","i":"2"} ''' ;

void setup() 
{
  Serial.begin(115200);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
  dac.begin(0x64);
  WiFiMulti.addAP(ssid, password);    //Set SSID and Password (SSID, Password)
  WiFi.begin(ssid, password);         //Set starting for Wifi
  Serial.println(WiFi.localIP());
  for(int t=5;t>0;t--)
  {
    Serial.println("[SETUP]:"+t);
    delay(1000);
  }
  String str = "Hello";
}

void loop() 
{
 uint16_t r_current = adc.read(MCP3208::SINGLE_0);
 uint16_t r_volt = adc.read(MCP3208::SINGLE_1);
 uint16_t val_current = adc.toAnalog(r_current);
 uint16_t val_volt = adc.toAnalog(val_current);
 Serial.print("Volt");Serial.print(val_volt);Serial.print(" Current");Serial.println(val_current);
 SendData(val_current,val_volt);
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

void SendData(uint16_t val_current,uint16_t val_volt)
{
  Serial.println("Send_data");

//float vmin = 0;
//float vmax = 800;
//float vcc = ADC_VREF;
//int pw = 50;
//int t = 100;
//float a = 50;
//float e = 10;
//float v = vmin;

    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        int l = 3;
        //int n = 0;
        for(int n = 0;n<=l;n++)
        {
        Serial.print("n : ");
        Serial.println(n);
        
        HTTPClient http;
        //String str = "http://" +String(Server)+":"+String(port)+"/log/"+Vstr+"/"+Istr;
        if(n==l)
        {
        String str = "http://" +String(Server)+":"+String(port2)+"/data/wemos/"+Vstr+"/"+Istr;
        }
        if(n=!l)
        {
        String str = "http://" +String(Server)+":"+String(port2)+"/data/wemos/"+"0"+"/"+"0"; 
        }
        //n = n+1;
        String str = "http://" +String(Server)+":"+String(port2)+"/data/wemos/"+"0"+"/"+"0";
        //String str = "Hello";
        Serial.println(str);
        http.begin(str);
        int httpCode = http.GET();
        Serial.print("[HTTP] GET... code:");
        Serial.println(httpCode);
          if(httpCode > 0) 
          {
            if(httpCode == HTTP_CODE_OK) 
              {
                String payload = http.getString();
                Serial.println(payload);
                String vming = payload.substring(0,4);
                String vmaxg = payload.substring(5,9);
                String vccg  = payload.substring(10,14);
                String pwg   = payload.substring(15,19);
                String tg    = payload.substring(20,24);
                String ag    = payload.substring(25,29);
                String eg    = payload.substring(30,34);
                String lg    = payload.substring(35,39);
                float vmin  = vming.toFloat();
                float vmax  = vmaxg.toFloat();
                float vcc   = vccg.toFloat();
                float pw    = pwg.toFloat();
                float t     = tg.toFloat();
                float a     = ag.toFloat();
                float e     = eg.toFloat();
                int l       = lg.toInt();
                //Serial.println(vmin,vmax,vcc,pw,t,a,e);
                Serial.print("Vmin:");Serial.println(vmin);
                Serial.print("Vmax:");Serial.println(vmax);
                Serial.print("Vcc:");Serial.println(vcc);
                Serial.print("PulseWidth:");Serial.println(pw);
                Serial.print("Period:");Serial.println(t);
                Serial.print("Amplitude:");Serial.println(a);
                Serial.print("StepE:");Serial.println(e);
                Serial.print("Loop:");Serial.println(l);
                GenWave(vmin,vmax,vcc,pw,t,a,e,l);
              }
          }
       }
    }
    //Vstr = "0";
    //Istr = "0";
    
}

void GenWave(float vmin,float vmax,float vcc,float pw, float t,float a,float e,int l)
{
    float v = vmin;
    for(int k =0;k<=l;k++)
    {
      Serial.print("Loop No : ");
      Serial.print(k);
      while(v<vmax)
      {
        v = v+a;
        int x = (int)((v/vcc)*4095.0f);
        dac.setVoltage(x, false);
        uint16_t rawV = adc.read(MCP3208::SINGLE_0);
        uint16_t valV = adc.toAnalog(rawV);
        uint16_t rawI = adc.read(MCP3208::SINGLE_1);
        uint16_t valI = adc.toAnalog(rawI);
        Vstr+= String(valV)+"," ;
        Istr+= String(valI)+"," ;
        delay(pw);
        v=v-(a-e);
        x = (int)((v/vcc)*4095.0f);
        dac.setVoltage(x, false);
        uint16_t rawV2 = adc.read(MCP3208::SINGLE_0);
        uint16_t valV2 = adc.toAnalog(rawV);
        uint16_t rawI2 = adc.read(MCP3208::SINGLE_1);
        uint16_t valI2 = adc.toAnalog(rawI);
        Vstr+= String(valV)+"," ;
        Istr+= String(valI)+"," ;
        delay(t-pw);
        k = k+1;
      }
       
    } 
}


