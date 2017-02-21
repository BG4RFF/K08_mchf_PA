#include <OneWire.h>

#include <LiquidCrystal.h>
#include <FreqCount.h>
#include "VSWR.h"
#include "K08PA.h"
#include "ATU.h"

PA_State PA;

int Pvor_input = A1;
int Prueck_input = A0;
float Pvor = 0; 
float Prueck = 0; 
float Pvor_mw=0;
float Prueck_mw=0;
float VSWR,Uvor,Urueck;
int current_band;  //aktuelles Band basierend auf aktuell gemessener Frequenz



void setup() {
  
  Serial.begin(9600);
  Serial.println("+++Test+++");

  init_CBank();
  init_LBank();
    
  
  for (int a=22; a<43; a++) //definiere 6 Ausgänge (22-27) für den TX_Filter
  {                         //definiere 8 Ausgänge (28-35) für die Kondensatoren-Bank und den vorn/hinten-Schalter
    pinMode(a, OUTPUT);     // definiere 7 Ausgänge (36-42) für die Spulen-Bank   
  }
  for (int a=22; a<28; a++)
  {
    digitalWrite(a, HIGH); //setze die Ausgänge für den TX-Filter auf High, da Relais mit 0 angesteuert werden
  }

  switch_band(PA.band); //setze initial den höchsten TX-Filter! 
  
  for (int a=29; a<43; a++)
  {
    digitalWrite(a, LOW); //setze die Ausgänge für die bistabilen ATU-Relais auf LOW
  }

  //pinMode(48, INPUT); //Eingang fuer den Temperatur Sensor
  pinMode(49, INPUT); //Eingang fuer PTT
  pinMode(50, OUTPUT); //Ausgang fuer PTT zur PA
  digitalWrite(50, LOW); //BIAS abschalten - PTT OFF

  
  lcd.begin(16, 4);  //Display mit 4 Zeilen a 16 Zeichen
  lcd.print("K08 mchfPA");
  
  set_Kapa(127);
  set_Kapa(0);
  set_Kapa_Position(0);
  
  set_Indu(127);
  set_Indu(0);
  
  
  FreqCount.begin(1); //Den Frequenzzaehler starten  Torzeit=1ms -> 7500 entspr. 7.500MHZ
  analogReference(DEFAULT);  //Nutzen der 5V als Analogreferenzquelle

}

void loop() {

  static unsigned long lcd_timer=0;
  
  PA.Fwd_Pwr_mW = getFwdPwr(Pvor_input);
  PA.Ref_Pwr_mW = getRefPwr(Prueck_input);
  if (PA.Fwd_Pwr_mW > 5) PA.VSWR=SWR(PA.Fwd_Pwr_mW,PA.Ref_Pwr_mW);
  PA.current=get_current();
     
    
  
      
    if ((millis()-lcd_timer) > 200) //alle 200ms das LCD refreshen
      { 
      PA.temp=getTemp();  
      refresh_lcd(PA.VSWR,PA.Fwd_Pwr_mW,PA.Ref_Pwr_mW, PA.freq, PA.band, PA.current, PA.temp, PA.PTT_in, PA.active);
      lcd_timer=millis();
      }
    
    PA.PTT_in=get_PTT_status();
      
    if (PA.PTT_in == true) //
      {
        if (PA.PTT_was_on==false)
        { 
          PA.PTT_was_on=true;
          //delay(10);
          
          PA.freq=get_freq();
          //PA.freq=3.6;
          if (PA.freq > 0.0)
          {
          
            current_band=get_band(PA.freq); //Achtung anpassen!!!!! - aktuelles Band der gemessenen Frequenz ermitteln
            if (current_band!=PA.band)      // hat die aktuelle Frequenz den aktiven Filterbereich verlassen?
            { 
              digitalWrite(50,LOW);
              PA.active=false;            //Vermeiden, dass in das falsche Filter gearbeitet wird
              delay(30);
              switch_band(current_band);  //Filter: Bandwechsel!
              PA.band=current_band;       // im PA-Status neues Band speichern
              delay(30);
            PA.active=true;
            digitalWrite(50, HIGH);
            }
            else
            {
            //Band ist noch korrekt
            PA.active=true;
            digitalWrite(50, HIGH);   
            }
          }
        }
        
    }
  else
    {
      PA.PTT_was_on=false; 
      PA.active=false;
      digitalWrite(50, LOW);
    
    }
}



