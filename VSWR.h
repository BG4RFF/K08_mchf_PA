

//*******VSWR.h ist eine Bibliothek zur Bedienung der VSWR-Messbrücke nach DJ0ABR *****
//*******  Michael Wild - DL2FW 4.1.2017********


#ifndef __VSWR__
#define __VSWR__

#include <Arduino.h>

float FmultiMap(float val, float * _in, float * _out, uint8_t size);


//const uint8_t NStuetz =18;
//float cal_Uin_fwd[] =     {0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };   //18 Werte
//float cal_Pwr_dBm_fwd[] = {-40.0,  8.0,    11.0,   14.0,   17.0,   20.0,   23.0,   26.0,   29.0,   32.0,   35.0,   38.0,   41.0,   44.0,   47.0,   50.0,   53.0,   56.0 };
//float cal_Uin_ref[] =     {0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };   //18 Werte
//float cal_Pwr_dBm_ref[] = {-40.0,  8.0,    11.0,   14.0,   17.0,   20.0,   23.0,   26.0,   29.0,   32.0,   35.0,   38.0,   41.0,   44.0,   47.0,   50.0,   53.0,   56.0 };

const float FWD_0dBm = 0.82031;
const float FWD_slope = 0.01987291; // mV pro dBm 
const float REF_0dBm = 0.78125;
const float REF_slope = 0.01933755; // mV pro dBm 


// *** getFwdPwr() liefert die aktuelle Forwärtsleistung in mW als float zurück
float getFwdPwr(int input_pin) {
  
  float Uin=0.0;
  float Pwr_dBm=0.0;
  float Pwr_mW=0.0;
  
  Uin=5.0*analogRead(input_pin)/1024; // Eingang kalibrieren?
  
  //if (Uin < cal_Uin_fwd[0]) Uin=cal_Uin_fwd[0]; // untere Grenze = -40dBm 
  //if (Uin > cal_Uin_fwd[17]) Uin=cal_Uin_fwd[17]; // obere Grenze = 56dBm
  
  //Pwr_dBm = FmultiMap(Uin, cal_Uin_fwd,cal_Pwr_dBm_fwd,NStuetz); // zwischen den 18 Stützstellen interpolieren.
  Pwr_dBm=(Uin-FWD_0dBm)/FWD_slope;
  Pwr_mW=pow(10,Pwr_dBm/10); 
  return Pwr_mW;  
}


// *** getRefPwr() liefert die aktuelle Rückwärtsleistung in mW als float zurück
float getRefPwr(int input_pin) {

  float Uin=0.0;
  float Pwr_dBm=0.0;
  float Pwr_mW=0.0;
  
  Uin=5.0*analogRead(input_pin)/1024; // Eingang kalibrieren?
  
//  if (Uin < cal_Uin_ref[0]) Uin=cal_Uin_ref[0]; // untere Grenze = -40dBm 
//  if (Uin > cal_Uin_ref[NStuetz-1]) Uin=cal_Uin_ref[NStuetz-1]; // obere Grenze = 56dBm
//  Pwr_dBm = FmultiMap(Uin, cal_Uin_ref,cal_Pwr_dBm_ref,NStuetz);  // zwischen den 18 Stützstellen interpolieren.
  
  Pwr_dBm=(Uin-REF_0dBm)/REF_slope;
  Pwr_mW=pow(10,Pwr_dBm/10); //Umrechnen von dBm in Watt
  
  return Pwr_mW;  
}


//*** liefert das aktuelle SWR***
float SWR(float Pvor_mw, float Prueck_mw){
  float Uvor=0.0;
  float Urueck=0.0;
  float VSWR=0.0;
  
  Uvor=sqrt(Pvor_mw/1000*50);
  Urueck=sqrt(Prueck_mw/1000*50);
  
  VSWR=abs((Uvor+Urueck)/(Uvor-Urueck)); // um negative Anzeigen zu verhindern (Pref>Pfwd)
  return(VSWR);
}


float FmultiMap(float val, float * _in, float * _out, uint8_t size)
{
   // take care the value is within range
   // val = constrain(val, _in[0], _in[size-1]);
   if (val <= _in[0]) return _out[0];
   if (val >= _in[size-1]) return _out[size-1];

   // search right interval
   uint8_t pos = 1;  // _in[0] allready tested
   while(val > _in[pos]) pos++;

   // this will handle all exact "points" in the _in array
   if (val == _in[pos]) return _out[pos];

   // interpolate in the right segment for the rest
   return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}


#endif

