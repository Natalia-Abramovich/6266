#include "algorithm_contamination.h"
#include <math.h>
#include "push_pop.h"
double PLref; 
double Tref; 
double Kp;
bool flag_termo_compensation;
bool flag_led_deterioratio;
int pollution_status;
bool flag_glass_poll;

void read_pollution_parameters()
{
  Tref = read_in_mem("Tref",(18),3968);
  PLref = read_in_mem("PLref",(200),4480);
  aging_LED = read_in_mem("aging_LED",(0.0),4032);
  flag_glass_poll = read_in_mem("glass_poll", 0.0, 3904);
  flag_termo_compensation = read_in_mem("compensation", 0.0, 3840);
}

void LED_deterioration()
{
  if(aging_LED)
  {
    
  }  
}

void glass_contaminate()
{
  if(flag_glass_poll)
  {
    double value = pop(stk_lvlPG, PG_stack);
    if(value > 277)
    {
      double k_pollution = (value-277)/8200.0;
      double transmittance = 1 - k_pollution;
      calibration_value.PSmsb/= (double)pow(transmittance, 2);
      if(value > 2000)
        pollution_status = 8;
      else 
        pollution_status = 0;
    }   
  } 
}

//                   0      1    2      3    4      5    6      7    8      9    10    11    12
static const float mas_TK[13] = {0.85, 0.88, 0.91, 0.97, 1.00, 0.99, 0.98, 0.97, 0.94, 0.93, 0.91, 0.88, 0.86};
static const int16_t bounds[14] = {-550, -50, 0, 50, 100, 250, 300, 350, 400, 450, 500, 550, 600, 650};

void termo_compensation()
{
   if(flag_termo_compensation)
  {
    double value = termo_val - Tref;
    if(value >= 0) 
      calibration_value.PSmsb += 1.943 * value;
    else
      calibration_value.PSmsb += 0.552564706 * value;
  }
}
/*float count_termo(double MORmomentary_koeff, uint32_t PG, bool glass_poll)
{ 
  
 
  MORmomentary_koeff = MORmomentary_koeff * (double)(mas_TK[i_termo]);
  if (MORmomentary_koeff > 75000.0) MORmomentary_koeff = 75000.0; //15.09.2020 鲯לּ败򱿠⡣onsole
       /* if (eco_mode == 0x04)
        {
            if (MORmomentary_koeff >= 6000.0) T=0;
            else T=1;
        }        */

/*return (float)MORmomentary_koeff;
}*/