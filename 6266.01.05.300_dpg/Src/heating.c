#include "main.h"
#include "tim.h"
#include "usart.h"
#include <stdio.h>

//uint8_t timedefrosting = 24;
//uint8_t deltaT = -18;
float Thigh = 6;
float Tlow = -15;

bool flag_heating = 1;
bool defrosting = 0;
int heatingpresent = 2;
int hysteresis = 5;

void diagnostics_of_flags()
{
  if (flag_heating) //working with new algorithm //main flag control heating
  {
    if (defrosting)
      { 
        HAL_TIM_Base_Start_IT(&htim15);    
        HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET);     
      }
    else
      if (heatingpresent>0)    
        start_heating_according_to_temp(termo_val);     
  }
  else 
  {
    if(flag_energy)
      HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET);
    else
      HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET); 
  }
}

bool run_of_heating(uint8_t * command)
{
  bool  flag_realization = 0;
  if(find_subarray(command, "HEAT HOOD ACT\r", 14))
  {
    flag_heating = 1;
  //  write123("f_heat",(1),4544);
    communicator("CMD OK!\n\r", 9);
    heatingpresent = 2;
   // write123("heatingpresent",(2),4608);
    flag_realization=1;
  }
  
  if(find_subarray(command, "HEAT HOOD DEACT\r", 16))
  {
     flag_heating = 0;
 //    write123("f_heat",(0),4544); 
     heatingpresent = 0;
 //   write123("heatingpresent",(0),4608);
     communicator("CMD OK!\n\r", 9);
     flag_realization=1;
     stop_defrosting(flag_console);
  }
  
  if(find_subarray(command, "HEAT OFF\r",9))
  {
     flag_realization=1;
    if(flag_heating)
    {
      heatingpresent = 0;
 //     write123("heatingpresent",(0),4608);
      communicator("CMD OK!\n\r", 9);
    }  
    else
      communicator("HOOD HEATER DEACT\n\r", 19);
  }
  
  if(find_subarray(command, "HEAT ON\r", 8))
  {
    flag_realization=1;
    if(flag_heating)
    {
      heatingpresent = 1;
   //   write123("heatingpresent",(1),4608);
      communicator("CMD OK!\n\r", 9);
    }  
    else
    communicator("HOOD HEATER DEACT\n\r", 19);
  } 

  if(find_subarray(command, "HEAT T\r", 7))
  {
    flag_realization=1;
    if(flag_heating)
    {
      heatingpresent = 2;
    //  write123("heatingpresent",(2),4608);
      communicator("CMD OK!\n\r", 9);
    }  
    else
    communicator("HOOD HEATER DEACT\n\r", 19);
  }
 
  diagnostics_of_flags();
  return  flag_realization;
}

void stop_defrosting(bool flag_console)
{
   defrosting=0;
   if(!flag_console)
     HAL_TIM_Base_Stop_IT(&htim15);
   HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET);  
}

void start_heating_according_to_temp(double termo_val)
{
 
  if (flag_heating && !defrosting)
  {
    if(heatingpresent > 0)
    {  
      //all time on or temperature
      if(heatingpresent == 1 || 
        (heatingpresent == 2 && (Thigh > termo_val  && termo_val  > Tlow)))     
        HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET);
     
      else 
        if(termo_val > (Thigh + hysteresis)  || termo_val < (Tlow - hysteresis))
          HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET); 
    
    }
    else
      HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET); //mode off //mode 
  }
    else
    {
      if(!flag_energy)
        if(flag_mor_eco && pprs.MORmomentary < 6000)
          HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET);
        else
              HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET);
    }  
    
}  

void enter_heating(double termo_val)
{
  //flag_heating = read123("f_heat",(1),4544);
 // defrosting = read123("defrosting",(0),4608); //4672
 // heatingpresent = (int)read123("heatingpresent",(2),4608);
  Thigh = read_in_mem("Thigh",(6),4736);
  Tlow = read_in_mem("Tlow",(-15), 4800);
  flag_mor_eco = read_in_mem("mor_eco",(0), 4864);
  hysteresis = read_in_mem("hysteresis",(5), 4928);
  
  diagnostics_of_flags();
}
