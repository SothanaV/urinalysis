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
const char* Server   = "192.168.1.6";           //set Server Domain or Server ip
const char* port     = "5000";                       //set server port
const char* port2     = "8000";                       //set server port
ESP8266WiFiMulti WiFiMulti;

String V_read = "" ;
String V_write = "";
String V_dac = "";
void Gen(int v_start,int v_stop,int delayi,int opset);
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



void loop() 
{
 Serial.println("Send_data");
    if((WiFiMulti.run() == WL_CONNECTED)) 
    {
        HTTPClient http;
        //Serial.println(Vstr);
        String str = "http://" +String(Server)+":"+String(port2)+"/cari/wemos/";
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
                   String v_startr = payload.substring(0,4);
                   String v_stopr = payload.substring(5,9);
                   String delayr = payload.substring(10,14);
                   String opsetr = payload.substring(15,19);
                   int v_start = v_startr.toInt();
                   int v_stop = v_stopr.toInt();
                   int delayi = delayr.toInt();
                   int opset = opsetr.toInt();
                   //Serial.println("V Start : "+v_start+"V Stop : "+v_stop);
                   Serial.print("Vstart");Serial.print(v_start);
                   Serial.print("Vstop");Serial.print(v_stop);
                   Serial.print("Delay");Serial.print(delayi);
                   Serial.print("Opset");Serial.println(opset);
                   Gen(v_start,v_stop,delayi,opset);
                   HTTPClient http;
                   http.begin("http://" +String(Server)+":"+String(port2)+"/cari/wemos2/");
                   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                   //http.POST("title=foo&body=bar&userId=1");
                   http.POST("read="+V_read+"&"+"write="+V_write+"&"+"dac="+V_dac);
                   http.writeToStream(&Serial);
                   http.end();
              }
              V_read = "";
              V_write = "";
              V_dac = "";
          }
       }
}
void Gen(int v_start,int v_stop,int delayi,int opset)
{
  Serial.println("Fn Gen");
  float v = v_start;
  float vcc = 3300;
                   while(v<v_stop)
                   {
                    v = v+0;
                    int x = (int)((v/vcc)*4095.0f);
                    dac.setVoltage(x,false);
                    Serial.print("V:");Serial.println(v);
                    Serial.print("X:");Serial.println(x);
                    uint16_t rawV = adc.read(MCP3208::SINGLE_0);
                    uint16_t valV = adc.toAnalog(rawV);
                    //int v = v;
                    V_read = V_read+valV+",";
                    V_write = V_write+v+",";
                    V_dac = V_dac+x+",";
                    delay(delayi);
                    v = v + opset;
                  }
}

