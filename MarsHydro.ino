#include <dht22.h>
#include <Ultrasonic.h>
#include <stdio.h>
#include <stdlib.h>
#include <OneWire.h>

// Define pins
int DHT22_1_Pin = 3; // all on digital pins
int DHT22_2_Pin = 1; // not currently in use
int DS18S20_Pin = 2;
int Dist1_Trig = 0;
int Dist1_Echo = 6;
int Dist2_Trig = 8;
int Dist2_Echo = 9;
int Dist3_Trig = 11;
int Dist3_Echo = 12;
int SoilMoisture_Pin = 0; //A0 for clarification

//Define value variables
float AirTemp1;
float Hum1;
float AirTemp2;
float Hum2;
float WatTemp;
float Dist1;
float Dist2;
float Dist3;
float SoilMoisture;
long microsec;

//setup DHT22 instance
dht22 DHT22;

//setup Water Temperature Sensor
OneWire ds(DS18S20_Pin);  // on digital pin 2

//set timer to 0
long previousMillis = 0;

void setup() 
{
  // start serial port
  Serial.begin(9600);
  
  // setup for DHT22
  DHT22.attach(DHT22_1_Pin);
  delay(2000);
}

void loop() 
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > 2000) {
  previousMillis = currentMillis;  
  
  //Read water temperature sensor
  WatTemp = getWaterTemp();
  
  //read soil moisture
  SoilMoisture = analogRead(SoilMoisture_Pin);
  
  //read air temperature and humidity
  int chk = DHT22.read();
  
  /*switch (chk) //error handling for DHT22
  {
    case 0: break;
    case -1: Serial.println("DHT22Checksumerror"); break;
    case -2: Serial.println("DHT22Timeouterror"); break;
    default: Serial.println("DHT22Unknownerror"); break;
  }
  */
  
  AirTemp1 = DHT22.temperature;
  Hum1 = DHT22.humidity;
  
  //Read ultrasonic distance sensor 1
  pinMode(Dist1_Trig,OUTPUT);
  pinMode(Dist1_Echo,INPUT);
  digitalWrite(Dist1_Trig, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(Dist1_Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Dist1_Trig,LOW);
  microsec = pulseIn(Dist1_Echo,HIGH);
  Dist1 = microsec * (0.01657 + 0.00004 * AirTemp1); //temperature corrected speed-of-sound/2

  //Read ultrasonic distance sensor 2
  pinMode(Dist2_Trig,OUTPUT);
  pinMode(Dist2_Echo,INPUT);
  digitalWrite(Dist2_Trig, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(Dist2_Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Dist2_Trig,LOW);
  microsec = pulseIn(Dist2_Echo,HIGH);
  Dist2 = microsec * (0.01657 + 0.00004 * AirTemp1); //temperature corrected speed-of-sound/2
  
  //Read ultrasonic distance sensor 3
  pinMode(Dist3_Trig,OUTPUT);
  pinMode(Dist3_Echo,INPUT);
  digitalWrite(Dist3_Trig, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(Dist3_Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Dist3_Trig,LOW);
  microsec = pulseIn(Dist3_Echo,HIGH);
  Dist3 = microsec * (0.01657 + 0.00004 * AirTemp1); //temperature corrected speed-of-sound/2
  
  //print out readings for testing
  /*Serial.println(WatTemp);
  Serial.println(SoilMoisture);
  Serial.println(AirTemp1);
  Serial.println(Hum1);
  Serial.println(Dist1);
  Serial.println(Dist2); 
  Serial.println(Dist3);
  */
  
  //print out for .csv
  Serial.print(WatTemp);
  Serial.print(",");
  Serial.print(SoilMoisture);
  Serial.print(",");
  Serial.print(AirTemp1);
  Serial.print(",");
  Serial.print(Hum1);
  Serial.print(",");
  Serial.print(Dist1);
  Serial.print(",");
  Serial.print(Dist2);
  Serial.print(",");
  Serial.println(Dist3);
  //Serial.print("trigger"); 
  }
  
}

float getWaterTemp() //define function for DS18B20 water temp sensor
{
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }


  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
