#include "main.h"
#include "usart.h"
#include "push_pop.h"
//#include "stm32f3xx_hal_tim.h"
#include "tim.h"
#include "algorithm_contamination.h"
uint32_t MOR_ch=0;
double PS_ch=0;


uint32_t MOR_cl=0;
double PS_cl=0;
bool flag_unification;
bool flag_cs;
#define MORet_MSB  26000     //параметры идеального прибора		//console  в переспективе
#define PSet_MSB  13		  //параметры идеального прибора	//console  в переспективе
#define MORet_LSB  285      //параметры идеального прибора		//console  в переспективе
#define PSet_LSB 1400 		   //параметры идеального прибора			//console  в переспективе

/*void calibration(uint32_t MORinput, bool reset)
{                     
 if(!reset)  
  {
    if(tim8_off==0)
    HAL_TIM_Base_Start_IT(&htim8);
 
  if (MORinput >= 5000ul)
  {
    calibration_hight=1;
    pprs.MORmsb = MORinput;
    write123("MORmsb", (pprs.MORmsb),0);
    for(uint8_t i=0; i<100; i++){__no_operation();}  
    pprs.PSmsb =  pop(stk_lvlPS, PSstack);
    write123("PSmsb", (pprs.PSmsb),64);
    for(uint8_t i=0; i<100; i++){__no_operation();}
  }
  else
  { 
    calibration_low=1;
    pprs.MORlsb = MORinput;
    write123("MORlsb", (pprs.MORlsb),128);
    for(uint8_t i=0; i<100; i++){__no_operation();} 
    pprs.PSlsb = pop(stk_lvlPS, PSstack);
    write123("PSlsb", (pprs.PSlsb),192);
    for(uint8_t i=0; i<100; i++){__no_operation();}
  }
 
      if(calibration_hight==1 && calibration_low==1 )
      {
        HAL_TIM_Base_Stop_IT(&htim8);
       
       // pprs.MORmsb = MOR_ch;
       // pprs.PSmsb =  PS_ch;      
      //  pprs.MORlsb = MOR_cl;
      //  pprs.PSlsb =  PS_cl;
                            
        calibration_hight=0;
        calibration_low=0;  
        calibration_successful=1;
                            
       // PLref=pprs.PL;
        pprs.CORR = (pprs.PSlsb * (double)pprs.MORlsb - pprs.PSmsb * (double)pprs.MORmsb) /
            ((double)pprs.MORlsb - (double)pprs.MORmsb);
        write123("CORR_val", (pprs.CORR),256);
        for(uint8_t i=0; i<100; i++){__no_operation();}
        pprs.SIGMA = pprs.MORlsb * (pprs.PSlsb - pprs.CORR);
        write123("SIGMA", (pprs.SIGMA),320);
        for(uint8_t i=0; i<100; i++){__no_operation();}
      }
  }
  else
  {
    HAL_TIM_Base_Stop_IT(&htim8); 
    pprs.MORmsb = 26000;//50000;
    write123("MORmsb", (pprs.MORmsb),0);
    for(uint8_t i=0; i<100; i++){__no_operation();}
    pprs.PSmsb = 13; //0.2;
    write123("PSmsb", (pprs.PSmsb),64);
    for(uint8_t i=0; i<100; i++){__no_operation();}
    pprs.MORlsb = 285;//280;
    write123("MORlsb", (pprs.MORlsb),128);
    for(uint8_t i=0; i<100; i++){__no_operation();} 
    pprs.PSlsb =  1400.0;
      write123("PSlsb", (pprs.PSlsb),192);
    for(uint8_t i=0; i<100; i++){__no_operation();}
                            
    calibration_hight=0;
    calibration_low=0;  
    calibration_successful=1;
                        
   // PLref=pprs.PL;
    pprs.CORR = (pprs.PSlsb * pprs.MORlsb - pprs.PSmsb * pprs.MORmsb) /
        (pprs.MORlsb - pprs.MORmsb);
    pprs.SIGMA = pprs.MORlsb * (pprs.PSlsb - pprs.CORR);
  }
}
*/
void unification()
{
  double CORR = (calibration_value.PSlsb * calibration_value.MORlsb - calibration_value.PSmsb * calibration_value.MORmsb) /
            (calibration_value.MORlsb - calibration_value.MORmsb);
  double SIGMA = calibration_value.MORlsb * (calibration_value.PSlsb - CORR);
 
  if(flag_unification) 
  { 
    if(pprs.PSoffset==0 && flag_cs)
    {
        double PS_50000 = (SIGMA + CORR *50000)/50000;        
          
        pprs.CORR = (uni_calibration_value.PSlsb * uni_calibration_value.MORlsb - uni_calibration_value.PSmsb * uni_calibration_value.MORmsb) /
            (uni_calibration_value.MORlsb - uni_calibration_value.MORmsb);
        write123("CORR_val", (pprs.CORR),256);
        for(uint8_t i=0; i<100; i++){__no_operation();}
        pprs.SIGMA = uni_calibration_value.MORlsb * (uni_calibration_value.PSlsb - pprs.CORR);
        write123("SIGMA", (pprs.SIGMA),320);
        for(uint8_t i=0; i<100; i++){__no_operation();}
        
        /*   pprs.scattering_coefficient =  uni_calibration_value.PSlsb / calibration_value.PSlsb;
        write123("Scattering",(pprs.scattering_coefficient),2560);*/
        
        double U_PS_50000 = (pprs.SIGMA + pprs.CORR *50000)/50000;
        
        pprs.PSoffset = PS_50000 - U_PS_50000;      
        write123("PSoffset", (pprs.PSoffset),3584);
        for(uint8_t i=0; i<100; i++){__no_operation();}

    }       
  }
  else
  {
       /* pprs.scattering_coefficient =  1;
        write123("Scattering",(pprs.scattering_coefficient),2560);*/
        
        pprs.PSoffset =0;      
        write123("PSoffset", (pprs.PSoffset),3584);
        for(uint8_t i=0; i<100; i++){__no_operation();}
        
        pprs.CORR = CORR;
        write123("CORR_val", (pprs.CORR),256);
        for(uint8_t i=0; i<100; i++){__no_operation();}
        pprs.SIGMA = SIGMA;
        write123("SIGMA", (pprs.SIGMA),320);
        for(uint8_t i=0; i<100; i++){__no_operation();}
  }
}

void calibration(uint32_t MORinput, bool reset)
{                     
 if(!reset)  
  {
    if(tim8_off==0)
    HAL_TIM_Base_Start_IT(&htim8);
   
  if (MORinput >= 5000ul)
  {
    calibration_hight=1;
    calibration_value.MORmsb = MORinput;
    write123("MORmsb", (MORinput),0);
    for(uint8_t i=0; i<100; i++){__no_operation();}  
   // calibration_value.PSmsb =  pop(stk_lvlPS, PSstack);
    write123("PSmsb", (calibration_value.PSmsb),64);
    for(uint8_t i=0; i<100; i++){__no_operation();}
   
  }
  else
  { 
    calibration_low=1;
    calibration_value.MORlsb = MORinput;
    write123("MORlsb", (MORinput),128);
    for(uint8_t i=0; i<100; i++){__no_operation();} 
   // calibration_value.PSlsb = pop(stk_lvlPS, PSstack);
  
    write123("PSlsb", (calibration_value.PSlsb),192);
    for(uint8_t i=0; i<100; i++){__no_operation();}
   // ppr33s.scattering_coefficient =  uni_calibration_value.PSlsb / calibration_value.PSlsb;
    //    write123("Scattering",(pprs.scattering_coefficient),2560);
  }
 
      if(calibration_hight==1 && calibration_low==1 )
      {
        HAL_TIM_Base_Stop_IT(&htim8);       
                            
        calibration_hight=0;
        calibration_low=0;  
        calibration_successful=1;
        Tref = termo_val;
        write123("Tref",(Tref),3968);                    
       // PLref=pprs.PL;
        unification();
      }
     
  }
  else
  {
    HAL_TIM_Base_Stop_IT(&htim8); 
    calibration_value.MORmsb = 39000;//50000;
    write123("MORmsb", (calibration_value.MORmsb),0);
    for(uint8_t i=0; i<100; i++){__no_operation();}
    calibration_value.PSmsb = 13; //0.2;
    write123("PSmsb", (calibration_value.PSmsb),64);
    for(uint8_t i=0; i<100; i++){__no_operation();}
    calibration_value.MORlsb = 285;//280;
    write123("MORlsb", (calibration_value.MORlsb),128);
    for(uint8_t i=0; i<100; i++){__no_operation();} 
    calibration_value.PSlsb =  1400.0;
      write123("PSlsb", (calibration_value.PSlsb),192);
    for(uint8_t i=0; i<100; i++){__no_operation();}
                            
    unification();
  }
}


// 
void calibration_with_console(uint8_t *b, uint8_t begin, uint8_t end)
{
     uint32_t MORinput = 0ul;
          uint16_t j, k;
          for (k = 1, j = begin; j > end; j--)
          { 
            if(b[j]>='0' && b[j]<='9')
            {
            MORinput += (uint32_t)(b[j] - '0') * (uint32_t)k;
            k *= 10;
            }            
            else
              if(b[j]=='\r')
              {
                  MORinput=0;
                  k = 1;
              }           
          }
          calibration(MORinput, 0);
          
}

