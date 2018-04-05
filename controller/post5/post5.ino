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

void SendData(uint16_t val_current,uint16_t val_volt);

const char* ssid     = "ois";                         //Set ssid
const char* password = "ilovestudy";                    //Set Password
const char* Server   = "192.168.1.5";           //set Server Domain or Server ip
const char* port     = "5000";                       //set server port
const char* port2     = "8000";                       //set server port
ESP8266WiFiMulti WiFiMulti;

String Vstr = "" ; 
String Istr = "" ;

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
  Serial.println("Send_data");
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;
        Serial.println(Vstr);
        String str = "http://" +String(Server)+":"+String(port2)+"/data/wemos/"+"0"+"/"+"0";
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
                int L       = lg.toInt();
                //Serial.println(vmin,vmax,vcc,pw,t,a,e);
                Serial.print("Vmin:");Serial.println(vmin);
                Serial.print("Vmax:");Serial.println(vmax);
                Serial.print("Vcc:");Serial.println(vcc);
                Serial.print("PulseWidth:");Serial.println(pw);
                Serial.print("Period:");Serial.println(t);
                Serial.print("Amplitude:");Serial.println(a);
                Serial.print("StepE:");Serial.println(e);
                Serial.print("Loop:");Serial.println(L);
                if(L>0)
                {
                  for(int k=0;k<L;k++)
                  {
                    Serial.print("Loop NO :");
                    Serial.println(k);
                    GenWave(vmin,vmax,vcc,pw,t,a,e);
                  }
                   delay(1000);
                   Serial.println("END LOOP");
                   HTTPClient http;
                   http.begin("http://" +String(Server)+":"+String(port2)+"/data/wemos2/");
                   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                   //http.POST("title=foo&body=bar&userId=1");
                   http.POST("V="+Vstr+"&"+"I="+Istr);
                   http.writeToStream(&Serial);
                   http.end();
                }
                Vstr = "";
                Istr = "";
              }
          }
       }
}

void GenWave(float vmin,float vmax,float vcc,float pw, float t,float a,float e)
{
      float v = vmax;
      uint16_t valI1,valI2, valV, dI;
      while(1)
      {
        v = v-(2*a)-e; if(v<vmin) break;
        int x = (int)((v/vcc)*4095.0f);
        Serial.print(x);
        Serial.println("  ");
        dac.setVoltage(x, false);
        delay(pw);
        //uint16_t rawI1 = ;
        valI1 = adc.toAnalog( adc.read(MCP3208::SINGLE_1) );
        v=v+(2*a); if(v<vmin) break;
        x = (int)((v/vcc)*4095.0f);
        Serial.println(x);
        dac.setVoltage(x, false);
        delay(t-pw);
        //uint16_t rawV = adc.read(MCP3208::SINGLE_0);
        //uint16_t rawI2 = adc.read(MCP3208::SINGLE_1);
        valV = adc.toAnalog(adc.read(MCP3208::SINGLE_7));
        valI2 = adc.toAnalog(adc.read(MCP3208::SINGLE_1));
        dI = valI1-valI2;
        Vstr = Vstr+valV+",";
        Istr = Istr+dI+",";
        
       
        
      } 
}


