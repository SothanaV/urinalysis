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
}

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
                   String v_start = payload.substring(0,4);
                   String v_stop = payload.substring(5,9);
                   String delay = payload.substring(10,14);
                   String opset = payload.substring(15,19);
                   int v_start = v_start.toInt();
                   int v_stop = v_stop.toInt();
                   int delay = delay.toInt();
                   int opset = opset.toInt();
                   Serial.println("V Start : "+v_start+"V Stop : "+v_stop);
                   int v = v_stop;
                   while(v<vmax)
                   {
                    dac.setVoltage(v,false);
                    uint16_t rawV = adc.read(MCP3208::SINGLE_0);
                    uint16_t valV = adc.toAnalog(rawV);
                    Vstr = Vstr+valV+",";
                    delay(100);
                    v = v + opset;
                    
                  }
                   HTTPClient http;
                   http.begin("http://" +String(Server)+":"+String(port2)+"/cari/wemos");
                   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                   //http.POST("title=foo&body=bar&userId=1");
                   http.POST("V="+Vstr);
                   http.writeToStream(&Serial);
                   http.end();
              }
          }
       }
}
