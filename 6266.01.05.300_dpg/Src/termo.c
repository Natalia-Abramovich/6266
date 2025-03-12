#include "lib.h"
#include "detector_weather.h"

struct flag_error__ flag_error;

double select_temperature()
{
   double termo_val;
   flag_error.temperature_outside = 0;
   flag_error.temperature_inside = 0;

   if(thresholds.termo) 
 {
   get_ROMid();
   if(devices!=0x00)
   {     
     get_Temperature();  
     return Temp + thresholds.out_t_corr;
   }
   else
   {
     flag_error.temperature_outside=1;     
     termo_val=get_termoval();
     if(flag_error.temperature_inside)
       return 18;
     return termo_val/10 + thresholds.t_corr;     
   }
 }
 else
 {   
   termo_val=get_termoval();
   if(!flag_error.temperature_inside)
       return termo_val/10 + thresholds.t_corr;
   else
   { 
     get_ROMid();
     if(devices!=0x00)
     {
        get_Temperature();  
        return Temp + thresholds.out_t_corr;
     }
     else
     {
       flag_error.temperature_outside=1;
       return 18;
     }      
     
   }
 }
}