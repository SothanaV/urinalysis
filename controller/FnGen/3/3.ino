#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

#define SELPIN 15 //Selection Pin   //D8
#define DATAOUT 13//MOSI            //D7
#define DATAIN  12//MISO            //D6
#define SPICLOCK  14//Clock         //D5
int readvalue; 
void setup() 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  dac.begin(0x64);


   //set pin modes 
 pinMode(SELPIN, OUTPUT); 
 pinMode(DATAOUT, OUTPUT); 
 pinMode(DATAIN, INPUT); 
 pinMode(SPICLOCK, OUTPUT); 
 //disable device to start with 
 digitalWrite(SELPIN,HIGH); 
 digitalWrite(DATAOUT,LOW); 
 digitalWrite(SPICLOCK,LOW); 
 
}


int read_adc(int channel){
  int adcvalue = 0;
  byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

  //allow channel selection
  commandbits|=((channel-1)<<3);

  digitalWrite(SELPIN,LOW); //Select adc
  // setup bits to be written
  for (int i=7; i>=3; i--){
    digitalWrite(DATAOUT,commandbits&1<<i);
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);    
  }

  digitalWrite(SPICLOCK,HIGH);    //ignores 2 null bits
  digitalWrite(SPICLOCK,LOW);
  digitalWrite(SPICLOCK,HIGH);  
  digitalWrite(SPICLOCK,LOW);

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(DATAIN)<<i;
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }
  digitalWrite(SELPIN, HIGH); //turn off device
  return adcvalue;
}
float vmin = 0;
float vmax = 800;
float vcc = 3320;
int pw = 50;
int t = 100;
float a = 50;
float e = 10;
void loop() 
{
    float v = vmin;
    while(v<vmax){
      v = v+a;
      int x = (int)((v/vcc)*4095.0f);
      dac.setVoltage(x, false);
      Serial.print("V");
      Serial.println(v);
       Serial.print(" a");
      Serial.println(a);
       Serial.print(" e");
      Serial.println(e);
      delay(pw);
      v=v-(a-e);
      x = (int)((v/vcc)*4095.0f);
      dac.setVoltage(x, false);
      Serial.print(" V");
      delay(t-pw);
    }
    
      
}



