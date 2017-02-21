//*******K08PA.h ist eine Bibliothek zum handling der K08PA *****
//*******  Michael Wild - DL2FW 4.1.2017********


#ifndef __K08PA__
#define __K08PA__

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

LiquidCrystal lcd(2,3,4,5,6,7);

int DS18B20_Pin = 48;
OneWire ds(DS18B20_Pin);


typedef struct PA_State
{
  boolean active = false;
  boolean PTT_in = false;
  float freq = 0.0;
  int band = 1210;
  unsigned long TX_on_timer = 0;  //Timer while TX on in milli sec.
  unsigned long TX_off_timer = 100000; // Timer while TX off in milli sec.
  float Fwd_Pwr_mW=0.0;
  float Ref_Pwr_mW=0.0;
  float VSWR=1.0;
  float current=0;
  float temp;
  boolean PTT_was_on = false;
  
  
} PA_State;


void refresh_lcd(float VSWR, float Pvor_mw, float Prueck_mw, float freq, int band, float current, float temp, bool PTT_in, bool active){

  lcd.setCursor(0, 1);
  lcd.print("SWR=      ");
  lcd.setCursor(5, 1);
  
  if (VSWR > 9.9)
    lcd.print(VSWR,0);
   else
   lcd.print(VSWR,1);
  //lcd.setCursor(9, 1);
  //lcd.print("       ");
  
  lcd.setCursor(0, 2);
  lcd.print("Pv=       ");
  lcd.setCursor(4, 2);
  lcd.print(Pvor_mw/1000);
  lcd.setCursor(10, 2);
  lcd.print("W");
  
  lcd.setCursor(0, 3);
  lcd.print("Pr=       ");
  lcd.setCursor(4, 3);
  lcd.print(Prueck_mw/1000);
  lcd.setCursor(10, 3);
  lcd.print("W"); 
  
  lcd.setCursor(10, 0);
  lcd.print("     "); 
  lcd.setCursor(10, 0);
  lcd.print(freq); 
  
  lcd.setCursor(0, 0);
  lcd.print("     "); 
  lcd.setCursor(0, 0);
  lcd.print(band); 

  lcd.setCursor(5, 0);
  lcd.print("     "); 
  lcd.setCursor(5, 0);
  lcd.print(current,1); 

  lcd.setCursor(10, 1);
  lcd.print("   "); 
  lcd.setCursor(10, 1);
  lcd.print(temp,1); 
  lcd.setCursor(14, 1);
  lcd.print("\337C"); 

  lcd.setCursor(12, 2);
  lcd.print("   "); 
  lcd.setCursor(12, 2);
  if (PTT_in == true) lcd.print("PTT"); 

}

float get_freq()  
{
  float count;
  float count_buff[3];
  int num=0;
  
  do //solange den Zaehler einlesen, bis HF groesser 1.6MHZ anliegt oder 200ms (200 * 1ms Torzeit) verstrichen sind
  {
    do; while (!FreqCount.available());  
    
     count_buff[0] = 8.0*((float)(FreqCount.read())/1000.0);  // zunaechst einmal den Frequenzzaehler leeren, um Teilzaehlung zu vermeiden
     num++; 
    
  } while ((count_buff[0]<1.6)&&num<200);
  
  if (count_buff[0] >= 1.6)  //wurde HF gesehen, dann gehts weiter...
  {
    for (int i=0; i<3; i++) // drei mal den Zaehler lesen
      {
        
        do; while (!FreqCount.available()); 
        count_buff[i] = 8.0*((float)(FreqCount.read())/1000.0);    
      
      }
  
    count = (count_buff[0]+count_buff[1]+count_buff[2])/3;
    if ((abs(count_buff[0]-count)+abs(count_buff[1]-count)+abs(count_buff[2]-count))/3.0/count>0.05) count=-2.0; // bei mittlerer Abweichnung
                                                                                                                //der Einzelmessungen > 5%, Messung ungültig, return -2 
      
  }
  else count=-1.0; //innerhalb 200ms keine HF gesehen
  Serial.println(count,5);

  
  return (count);
    
}

boolean get_PTT_status()
{
  boolean PTT_in;
    if (digitalRead(49) == HIGH) //PTT Anforderung?
      {
        PTT_in=false;
      }
      else
      {
        PTT_in=true;
      }
  return PTT_in;

}


int get_band(float freq){
  int band=160;
  if (freq < 2.0) band =160;
  else if (freq < 3.8) band =80;
    else if (freq < 7.2) band =40;
      else if (freq < 14.5) band =3020;
        else if (freq < 21.5) band =1715;
          else band =1210;
return band;
}



void switch_band(int band){  // braucht aktuell 20ms für den Umschaltvorgang
  unsigned long switch_start=0;
  
  for (int a=22; a<28; a++)
    {
      digitalWrite(a, HIGH); //setze die Ausgänge für den TX-Filter auf High, da Relais mit 0 angesteuert werden
    }                        //-> alle Relais abfallen lassen
  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<12);  //warte 11ms zum Abfallen des Relais - 10ms lt. Datenblatt  
    
  if (band == 160) digitalWrite(22, LOW);
    else if (band == 80) digitalWrite(23, LOW);
      else if (band == 40) digitalWrite(24, LOW);
        else if (band == 3020) digitalWrite(25, LOW);
          else if (band == 1715) digitalWrite(26, LOW);
            else digitalWrite(27, LOW);  // in allen anderen Fällen höchsten Filter verwenden
            
switch_start=millis(); //"starte" den Timer
do ; while ((millis()-switch_start)<17);  //warte 16ms zum Anziehen des Relais - 15ms lt. Datenblatt  

}

float get_current()
{
  float I_dc=0;

  I_dc= (1.0*abs(513-analogRead(2)))/13.3;

   return I_dc;

}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
 
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
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




#endif
