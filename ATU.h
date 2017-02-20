

//*******ATU.h ist eine Bibliothek zur Bedienung der ATU-Erweiterung  nach DL2FW *****
//*******  Michael Wild - DL2FW 15.1.2017********


#ifndef __ATU__
#define __ATU__

#include <Arduino.h>

float C[7]={10.0, 20.0, 39.0, 82.0, 160.0, 330.0, 660.0}; //in pF  noch nicht nachgemessen
float L[7]={0.139, 0.185, 0.406, 0.730, 1.287, 2.559, 5.424};    //in µH  noch nicht nachgemessen

float CBank[128];
float LBank[128];

void init_CBank() //Aufbau der 128 möglichen Kapazitäten
{
  
  for (int i=0; i<128; i++)
    {
      CBank[i]=(i&1)*C[0] + ((i&2)>>1)*C[1] + ((i&4)>>2)*C[2] + ((i&8)>>3)*C[3] +
              ((i&16)>>4)*C[4] + ((i&32)>>5)*C[5] + ((i&64)>>6)*C[6];    
    //Serial.println(CBank[i],2);
    }
  
}

void init_LBank() // Aufbau der 128 möglichen Induktivitäten
{
  
  for (int i=0; i<128; i++)
    {
      LBank[i]=(i&1)*L[0] + ((i&2)>>1)*L[1] + ((i&4)>>2)*L[2] + ((i&8)>>3)*L[3] +
              ((i&16)>>4)*L[4] + ((i&32)>>5)*L[5] + ((i&64)>>6)*L[6];    
    //Serial.println(LBank[i],2);
    }
  
}

void reset_CRelay(int i)
{
  unsigned long switch_start=0;
  
  for(int a=28;a<36;a++)
  {
    digitalWrite(a,LOW);
  }
  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 
  
  digitalWrite(29+i,HIGH);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<15);  //warte 15ms zum Umschalten des Relais - 10ms lt. Datenblatt

  digitalWrite(28+i,LOW);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 

}

void set_CRelay(int i)
{
  unsigned long switch_start=0;
  
  for(int a=28;a<36;a++)
    {
      digitalWrite(a,HIGH);
    }
  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 
  
  digitalWrite(29+i,LOW);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<15);  //warte 15ms zum Umschalten des Relais - 10ms lt. Datenblatt

  for(int a=28;a<36;a++)
    {
      digitalWrite(a,HIGH);
    }

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 

  
}

void reset_LRelay(int i)
{
  unsigned long switch_start=0;
  
  for(int a=36;a<43;a++)  // 36 .. 42 sind die Ausgänge für die L-Relays
  {
    digitalWrite(a,LOW);
  }
  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 
  
  digitalWrite(36+i,HIGH);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<15);  //warte 15ms zum Umschalten des Relais - 10ms lt. Datenblatt

  digitalWrite(36+i,LOW);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 

}

void set_LRelay(int i)
{
  unsigned long switch_start=0;
  
  for(int a=36;a<43;a++)
    {
      digitalWrite(a,HIGH);
    }
  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 
  
  digitalWrite(36+i,LOW);

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<15);  //warte 15ms zum Umschalten des Relais - 10ms lt. Datenblatt

  for(int a=36;a<43;a++)
    {
      digitalWrite(a,HIGH);
    }

  switch_start=millis(); //"starte" den Timer
  do ; while ((millis()-switch_start)<2);  //warte 2ms zum Einpegeln 

  
}



void set_Kapa(int index) // setzt die Kapazität mit index 0..127

{
  static int current_index=0;

  if (index!=current_index) // es muss geschaltet werden
    {
      if (index > current_index) // neue Kapazität ist größer als alte -> von oben nach unten suchen und schalten
      {
        for (int i=6; i>-1; i--)
          {
            
            
            if ((index&(1<<i))>(current_index&&(1<<i)))
            {
              set_CRelay(i);   // ist im neuen index C[i] ein und vorher aus, dann schalte Relais ein
              //Serial.println("Setze Relais");
              //Serial.println(i);
              
            }
            if ((index&(1<<i))<(current_index&&(1<<i))) 
             {
              reset_CRelay(i); // ist im neuen index C[i] aus und vorher ein, dann schalte Relais aus
              //Serial.println("Ruecksetze Relais");
              //Serial.println(i);
             }
          }
      }
      if (index < current_index) // neue Kapazität ist kleiner als alte -> von unten nach oben suchen und schalten
      {
        for (int i=0; i<7; i++)
          {
                        
            if ((index&(1<<i))>(current_index&(1<<i)))  
            {
              set_CRelay(i);   // ist im neuen index C[i] ein und vorher aus, dann schalte Relais aus
              Serial.println("Setze Relais");
              Serial.println(i);
              
            }
                  
            if ((index&(1<<i))<(current_index&(1<<i))) 
            {
              reset_CRelay(i); // ist im neuen index C[i] aus und vorher ein, dann schalte Relais ein
              Serial.println("Ruecksetze Relais");
              Serial.println(i);
             }  
          
          }
      }
    }
  current_index=index;
}

void set_Indu(int index) // setzt die Induktivität mit index 0..127

{
  static int current_index=0;

  if (index!=current_index) // es muss geschaltet werden
    {
      if (index > current_index) // neue Induktivität ist größer als alte -> von oben nach unten suchen und schalten
      {
        for (int i=6; i>-1; i--)
          {
            
            
            if ((index&(1<<i))>(current_index&&(1<<i)))
            {
              set_LRelay(i);   // ist im neuen index L[i] ein und vorher aus, dann schalte Relais aus
              Serial.println("Setze Relais");
              Serial.println(i);
              
            }
            if ((index&(1<<i))<(current_index&&(1<<i))) 
             {
              reset_LRelay(i); // ist im neuen index L[i] aus und vorher ein, dann schalte Relais ein
              Serial.println("Ruecksetze Relais");
                Serial.println(i);
             }
          }
      }
      if (index < current_index) // neue Induktivität ist kleiner als alte -> von unten nach oben suchen und schalten
      {
        for (int i=0; i<7; i++)
          {
                        
            if ((index&(1<<i))>(current_index&(1<<i)))  
            {
              set_LRelay(i);   // ist im neuen index L[i] ein und vorher aus, dann schalte Relais aus
              Serial.println("Setze Relais");
              Serial.println(i);
              
            }
                  
            if ((index&(1<<i))<(current_index&(1<<i))) 
            {
              reset_LRelay(i); // ist im neuen index L[i] aus und vorher ein, dann schalte Relais ein
              Serial.println("Ruecksetze Relais");
              Serial.println(i);
             }  
          
          }
      }
    }
  current_index=index;
}

void set_Kapa_Position(int index) // setzt die Kondensatoren nach vorne (TRX=0) oder hintern (Antenne=1) ??
{
  static int current_index=2;

  if (index!=current_index) // es muss geschaltet werden
    {
      if (index > current_index) // Kondensatoren an die Antenne schalten
      {
              set_CRelay(-1); //-1 ist Ausgang 28!!!  
              Serial.println("Setze C Position Relais an die Antenne");
              
      }
      if (index < current_index) // // Kondensatoren an den TRX schalten
       {
              reset_CRelay(-1); //-1 ist Ausgang 28!!!  
              Serial.println("Setze C Position Relais an den TRX");
              
              
      
    }
  current_index=index;
}
}


void Tune()
{

  // VSWR0 bestimmen
  // PFWD0 und PREF0 ablegen
  
  // ~50 Ohm reaktanz zuschalten
  // dazu nahe Induktivität aus Frequenz bestimmen und wahre Reaktanz errechnen (XH1)
  // VSWR1 bestimmen
  // PFWD1 und PREF1 ablegen
  
  // ~100 Ohm reaktanz zuschalten
  // dazu nahe Induktivität aus Frequenz bestimmen und wahre Reaktanz errechnen (XH2)
  // VSWR2 bestimmen
  // PFWD2 und PREF2 ablegen

  // Komplexe Antennenimpedanz abschätzen
  // Anpassnetzwerk ermitteln -> Position Kondensator schalten
  // geeignete Induktivität und Kapazität ermitteln
  // Zuschalten und SWR ermitteln, ggf. wiederholen

  
}


#endif
