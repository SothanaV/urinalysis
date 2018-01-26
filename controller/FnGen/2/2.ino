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

void loop() 
{
  
    for(int i = 0;i<4095;)
    {
    dac.setVoltage(i, false);
    delay(1000);
    readvalue = read_adc(1);
    float voltage = readvalue*(3.3/4096);
    dac.setVoltage(i-500, false);
    delay(1000);
    i = i+500;
    Serial.print("Read");
    Serial.print(readvalue);
    Serial.print(" volt");
    Serial.print(voltage);
    Serial.print(" I");
    Serial.println(i);
    }
    
    
    
}
void genwave()
{
    
}

