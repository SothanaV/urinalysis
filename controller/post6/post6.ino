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
const char* Server   = "192.168.1.7";           //set Server Domain or Server ip
const char* port     = "5000";                       //set server port
const char* port2     = "8000";                       //set server port
ESP8266WiFiMulti WiFiMulti;

int led = 16;

String Vstr = "" ; 
String Istr = "" ;

int abs(int x) 
{
  if(x<0){
    return -x;
  }
  return x;
}

int* read_sensor(uint16 *valueI, uint16 *valueV, int n) 
{
  int i;
  for(i=0; i<n; ++i) {
    //value[i] = 1; //read_analog_here
    valueI[i] = adc.toAnalog( adc.read(MCP3208::SINGLE_1) );
    valueV[i] = adc.toAnalog( adc.read(MCP3208::SINGLE_0) );
//    Serial.print("Read Value");
//    Serial.println(valueI[i]);
    //delay(1);
  }
}

float ransac(uint16  *value, int n, int t) 
{
  int i, j, maxi=0;
  int cnt[n], sum[n];
  for(i=0; i<n; ++i) {
    sum[i] = 0;
    cnt[i] = 0;
  }
  for(i=0; i<n; ++i) {
    for(j=0; j<n; ++j) {
      if(i!=j) {
        if(abs(value[i]-value[j]) < t){
          cnt[i] += 1;
          sum[i] += value[j];
        }
      }
    }
    if(cnt[i] > maxi) {
      maxi = i;
    }
  }
  Serial.print(sum[maxi]);
  Serial.print("   ");
  Serial.println(cnt[maxi]);
  return float(sum[maxi])/float(cnt[maxi]);
}
void readSensor(float *res)
{
  uint8 n = 50;
  uint8 ti = 50, tv = 50;
  uint16 valueI[n], valueV[n];
  read_sensor(valueI, valueV, n);
  res[0] = ransac(valueI, n, ti); 
  res[1] = ransac(valueV, n, tv);
}

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
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  for(int t=5;t>0;t--)
  {
    Serial.println("[SETUP]:"+t);
    delay(1000);
  }
  Serial.println(WiFi.localIP());
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
                digitalWrite(led,HIGH);
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
                   Serial.print(Istr);
                   http.POST("V="+Vstr+"&"+"I="+Istr);
                   http.writeToStream(&Serial);
                   http.end();
//                   break();
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
//      uint16_t valI1,valI2, valV, dI;
      float dI, valV;
      float res1[2], res2[2];
      int DI;
      int VI;
      while(1)
      {
        digitalWrite(led,HIGH);
        v = v-(2*a)-e; if(v<vmin) break;
        int x = (int)((v/vcc)*4095.0f);
        dac.setVoltage(x, false);
        readSensor(res1);
        delay(pw);
        v=v+(2*a); if(v<vmin) break;
        x = (int)((v/vcc)*4095.0f);
        dac.setVoltage(x, false);
        delay(t-pw);
        digitalWrite(led,LOW);
        readSensor(res2);
        dI = res1[0] - res2[0];
        DI = (int)(dI*100);
        valV = (res1[1]+res2[1])/2;
        VI =(int)(valV*100);
        Serial.println();
        Serial.print(dI);
        Serial.print("    ");
        Serial.println(valV);
        Istr = Istr + DI + ",";
        Vstr = Vstr + VI + ",";
        
      } 
}






