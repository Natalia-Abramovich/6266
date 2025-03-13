/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <vector>
#include <algorithm>
#include "push_pop.h"
#include "detector_weather.h"
#include "hcms.h" 
#include "eeprom.h"
#include "lib.h"
#include "algorithm_contamination.h"
#include "approximation.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <numeric>
#include <stdio.h>
#include <math.h>
#include "OneWire.h"
#include "intrinsics.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define MAX_PROC_COUNT   ((uint16_t)(52UL*240UL))
#define MAX_PROC_COUNT   ((uint16_t)(53UL*240UL))

//#define COUNT_MEASURES   (5280UL)



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//static bool sEE_activated = false;




HAL_StatusTypeDef hstd;

uint8_t flag_profile = 0;
uint8_t             flag_out   = 0;
int help=0;

//static int16_t temperature;

struct proc_cntr___
{
  uint8_t proc_count_low;
  uint8_t proc_count_high;
};

struct proc_cntr___ p_cntr     = { (MAX_PROC_COUNT + 1) % 240, (MAX_PROC_COUNT + 1) / 240 };


uint64_t            xxxxPS0    = 0ULL,
xxxxPSFull = 0ULL;
uint32_t xxxxPS0msrs = 0ULL;
uint32_t xxxxPSFullmsrs = 0ULL;

volatile uint8_t    spi_cnt    = 11;
uint16_t   xcoeff     = 1;

volatile bool       dataready  = false;
volatile bool       light_on   = false;

volatile uint32_t   time__;
bool flag_console=0;
int16_t profile[COUNT_MEASURES];
uint64_t average_ps=0;
bool flag_temperature_error=0;

//#define DEBUG_TIMING
#ifndef DEBUG_TIMING
volatile bool       timeflag   = false;
#else
volatile bool       timeflag   = true;
#endif

bool aging_LED=0;
bool flag_Kp=0;

bool flag_t_eco=0;

bool flag_mor_eco=0;
bool flag_energy =0;

bool start_devise=0;

int n=50;


int speed=1200;
int word_length=8;
double   PS0x = 0.0;
uint32_t PL0x = 0, PG0x = 0;    //

uint8_t index_phenocode = 0x0;
uint8_t tim_for_ca = 300;

//extern float Temp[MAXDEVICES_ON_THE_BUS];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

union rslt_                                                                 //SPI_STC_vect
{                                                                           //SPI_STC_vect
  uint8_t bts[4];                                                                //SPI_STC_vect
  uint32_t w;                                                                    //SPI_STC_vect
}
static rslt_un;                                                               //SPI_STC_vect

uint8_t rslt_bt;


uint32_t get_time(void)
{
  return time__;
}


void HAL_SPI_RxCpltCallback(/*SPI_HandleTypeDef*/void *hspi)
{
//  if (&hspi2 == hspi)
//  {
//    __no_operation();
//  }
  if (NULL == hspi)
  {
//    rcnt++;
//    if ( rcnt < 40 )
//    {
//      rbts[ rcnt ] = rslt_bt;
//      HAL_SPI_Receive_IT( &hspi1, &rslt_bt, 1 );
//    }
//    else
//      return;
    uint8_t i=0;
  start_case:
    switch (spi_cnt)
    {
//      case 0:
      /*CS2 = 0;*/
//        HAL_GPIO_WritePin( CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET );                 // for PG0 & PGFull
      /*SPDR = 0;*/
//        HAL_SPI_Receive_IT( &hspi2, &rslt_bt, 1 );
//        rslt_un.w = 0UL;
//        break;
//      case 1:
//      case 4:
    case 8:
      rslt_un.bts[2] = rslt_bt;
      {
        rslt_bt = 0;
        for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
        {
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET);
             __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
          
          if (HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin) == GPIO_PIN_SET)
          {
            rslt_bt |= mask;
          }
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
              __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
        }
        spi_cnt = 9;
        goto start_case;
      }
      /*SPDR = 0;*/
      //HAL_SPI_Receive_IT( &hspi1, &rslt_bt, 1 );   //for bts[1]
//        HAL_SPI_TransmitReceive_IT( &hspi1, ( uint8_t * )&zro_bt, &rslt_bt, 1 );
      break;
//      case 2:
//      case 5:
    case 9:
      rslt_un.bts[1] = rslt_bt;
      /*SPDR = 0;*/
      {
        rslt_bt = 0;
        for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
        {
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET);
              __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
          if (HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin) == GPIO_PIN_SET)
          {
            rslt_bt |= mask;
          }
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
             __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
        }
        spi_cnt = 10;
        goto start_case;
      }
      //HAL_SPI_Receive_IT( &hspi1, &rslt_bt, 1 ); // for bts[0]
//        HAL_SPI_TransmitReceive_IT( &hspi1, ( uint8_t * )&zro_bt, &rslt_bt, 1 );
      break;
//      case 3:
      /*CS2 = 1;*/
//        HAL_GPIO_WritePin( CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET );                 // for PG0 & PGFull
      /*CS3 = 0;*/                         //for PL0   & PLFull
//        HAL_GPIO_WritePin( CS3_GPIO_Port, CS3_Pin, GPIO_PIN_RESET );                 // for PG0 & PGFull
//        rslt_un.bts[ 0 ] = rslt_bt & 0xf0;
      /*SPDR = 0;*/
//        HAL_SPI_Receive_IT( &hspi2, &rslt_bt, 1 );
//        if ( p_cntr.proc_count_high == 0 )
//          PG0__ += rslt_un.w;
//        else
//          PGFull += rslt_un.w;
//        rslt_un.w = 0UL;
//        break;
//      case 6:
      /*CS3 = 1;*/
//        HAL_GPIO_WritePin( CS3_GPIO_Port, CS3_Pin, GPIO_PIN_SET );                 // for PG0 & PGFull
//        rslt_un.bts[ 0 ] = rslt_bt & 0xf0;
//        if ( p_cntr.proc_count_high == 0 )
//          PL0__ += rslt_un.w;
//        else
//          PLFull += rslt_un.w;
//        rslt_un.w = 0UL;
//        break;
    case 7:
 //     HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET); 
   
       HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET);     
           __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
       rslt_un.w = 0;
       for (uint32_t mask = 0x80000; mask != 0; mask >>= 1)
       {
         HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
         for(i=0;i<1;i++) __no_operation();
         if (HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin) == GPIO_PIN_SET)
         {
            rslt_un.w |= mask;
         }
        // for(i=0;i<1;i++) __no_operation();
         HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET);
             __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
            __no_operation();
       }
       HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
       spi_cnt = 10;
    
      goto start_case;
      break;
    /*  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET);
      {
        rslt_un.w = 0;
        for (uint32_t mask = 0x800000; mask != 0; mask >>= 1)
        {
          __no_operation();
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET);
          if (HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin) == GPIO_PIN_SET)
          {
            rslt_un.w |= mask;
          }
          HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
          __no_operation();
        }
HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
        spi_cnt = 10;
      } 
      goto start_case;
      break;*/
    case 10:
      {
        /*CS1 = 1;*/
        HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
      //  rslt_un.bts[0] = rslt_bt; //& 0xf0; // low four bits of third byte ADC ignored


         if(p_cntr.proc_count_high<=45)  // ((p_cntr.proc_count_high != 0)&& xxxxPSFullmsrs<10320)
        {                    
           xxxxPSFull += ((rslt_un.w ));//(rslt_un.w); ((rslt_un.w & 0xffff0ul) >> 4);
         // xxxxPSFull += (uint64_t)((rslt_un.w & 0xffff0ul) >> 4);
           xxxxPSFullmsrs++;
          
        }
        
        if ( (p_cntr.proc_count_high >= 8) &&
            (p_cntr.proc_count_high <= 45)
            // &&(xxxxPSFullmsrs < 9120)
              )
        {
           profile[(p_cntr.proc_count_high - 8) * 240 + p_cntr.proc_count_low] = ((rslt_un.w ));//((rslt_un.w )); //(uint16_t)(((rslt_un.w & 0xfffffff0))> 65534)? (65534):((rslt_un.w & 0xfffffff0));  /* */
        }
         if (p_cntr.proc_count_high >= 47) 
        {  
         

         xxxxPS0 += ((rslt_un.w ));// rslt_un.w;
        xxxxPS0msrs++;
 
          break;
        }
        else spi_cnt = 11;
        break;
      }
    case 11:
      return;
    }
    spi_cnt++;

  }
}
bool tim8_off=0;
bool calibration_low=0;
bool calibration_hight=0;
bool calibration_successful=0;

volatile uint32_t ccr2 = 0; //3000;
uint8_t c_t_8=0;
uint8_t c_t_15=0;
uint16_t c_t_24=0;

void stop_tim()
{
   huart4.RxXferCount = 256;
   huart5.RxXferCount = 256;
   c_t_15=0;
   if(!defrosting)
   {
     HAL_TIM_Base_Stop_IT(&htim15); 
     c_t_24=0;
   }
   flag_console=0;
   HAL_UART_AbortReceive(&huart4);
   HAL_UART_AbortReceive(&huart5);
 // HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_SET);/*M0=0;*/         
 // HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET); /*M1=1;*/         
 // HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET);
}
  
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim8)
  {
 if (c_t_8>=13 && flag_out == 1) /**/
   /* if ((c_t_8>=5 && flag_profile == 1) || (c_t_8>=13 && flag_out == 1))*/
     {
        flag_out = 0;  
        c_t_8=0;
        tim8_off=0;
        HAL_TIM_Base_Stop_IT(&htim8);
 //       NVIC_SystemReset();
     } 
    /* if(c_t_8>=14)
     {
       if(calibration_hight==1 || calibration_low==1 )
       {
         c_t_8=0;
         tim8_off=0;
         HAL_TIM_Base_Stop_IT(&htim8); 

         calibration_successful=calibration_hight = calibration_low = 0;
       }
     }*/
  else c_t_8++;
  }
   if (htim == &htim15)
  {
     if(flag_console)    
     {
       if(c_t_15 >= 15 ) 
         stop_tim();    
       else c_t_15++;   
     }
     if (flag_heating && defrosting)
     {
        if(c_t_24>=1440) 
        {
          c_t_24=0;
          stop_defrosting(flag_console); 
        }           
        else c_t_24++; 
     }
  }
  
if (htim == &htim1)
  {
//    perel_cntr++;
    if (light_on)
    {
      /*CA = 0;*/     //light on -> 0 // above zones0 & 1
      HAL_GPIO_WritePin(CA_GPIO_Port, CA_Pin, GPIO_PIN_RESET);
    }
    static uint16_t x_proc_count;
    x_proc_count = ((((uint16_t)p_cntr.proc_count_high) * 240) + ((uint16_t)p_cntr.proc_count_low));
    if (
//      proc_count<MAX_PROC_COUNT
//     (p_cntr.proc_count_high < (MAX_PROC_COUNT/240))||
//     (p_cntr.proc_count_low < (MAX_PROC_COUNT % 240))
//(((( uint16_t )p_cntr.proc_count_high) * 240) + (( uint16_t )p_cntr.proc_count_low)) < MAX_PROC_COUNT
        x_proc_count < 12720 //12960 //13200 //// 13680 //13920// 12720
       )
    {

      ccr2 = htim1.Instance->CCR2 = 300;// tim_for_ca;
      HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_2);
         
      switch (p_cntr.proc_count_high)
      {
      case 0x2:
      case 0x3:
        
//      k_img=4;// x4     k3=1 k2=0 k1=0
        
       HAL_GPIO_WritePin(K3_GPIO_Port, K3_Pin, GPIO_PIN_SET);
     
       HAL_GPIO_WritePin(K2_GPIO_Port, K2_Pin, GPIO_PIN_RESET);
      
       HAL_GPIO_WritePin(K1_GPIO_Port, K1_Pin, GPIO_PIN_RESET);
       xcoeff = 4;
        break;
      case 0x4:
      case 0x5:
//      k_img=5;// x40    k3=1 k2=0 k1=1
        
      HAL_GPIO_WritePin(K3_GPIO_Port, K3_Pin, GPIO_PIN_SET);
        
         HAL_GPIO_WritePin(K2_GPIO_Port, K2_Pin, GPIO_PIN_RESET);
        
         HAL_GPIO_WritePin(K1_GPIO_Port, K1_Pin, GPIO_PIN_SET);
         xcoeff = 40;
        break;
      case 0x6:
      case 0x7:
//      k_img=6;// x400   k3=1 k2=1 k1=0
       
        HAL_GPIO_WritePin(K3_GPIO_Port, K3_Pin, GPIO_PIN_SET);
        
        HAL_GPIO_WritePin(K2_GPIO_Port, K2_Pin, GPIO_PIN_SET);
        
        HAL_GPIO_WritePin(K1_GPIO_Port, K1_Pin, GPIO_PIN_RESET);
        xcoeff = 400;
        break;
      }
    }
    time__++;
    static uint32_t ytime;
    ytime = time__ % sending_period;
    if (ytime == 0x0)
    {
      __no_operation();

        timeflag = true;

    }
//
//  if (
//     (dataready==false)&&
//     (timeflag==false)&&
////     (proc_count==(MAX_PROC_COUNT+1))&&
//     (
//     (p_cntr.proc_count_high==((MAX_PROC_COUNT+1)/240))&&
//     (p_cntr.proc_count_low==((MAX_PROC_COUNT+1) % 240))
//     )&&
//     (obf.obd.bs.curr_state==OBUF_STATE_FREE)
//     )
//    timeflag=true;
#define OCF1A__ TIFR_Bit4
    ////
    ////*TIFR_*/OCF1A__ = 1; //preventive clear interrupt flag on
    ///ocr1a compare
  }
}

//#define TEN_PERCENT_LIMIT (((65536UL/10UL)*240UL)<<4)
#define TEN_PERCENT_LIMIT (65536ul/10ul)

//int c1600cntr=0,c300ex1600=0;
#ifdef DWTTM
struct dwttm__
{
  uint32_t cycnt;
  uint32_t evnt;
}evnts[64];
int evidx = 0;
#endif

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim1)
  {
    /*switch ( OCR1B )*/
    switch (/*htim1.Instance->CCR2*/ccr2)
    {
    case 300: //150 us
      if (
          (p_cntr.proc_count_high < 2) &&
          (p_cntr.proc_count_low < 240)
         )
      {
        //  spi_cnt = 0;
//      SPDR=0;    // start SPI_SEQUENCE from pos0 to pos6
        rslt_un.w = 0;
        //uint32_t w;
        //uint8_t  i;
        /*CLK2 = CS1 = CS2 = CS3 = 1;*/
//          HAL_GPIO_WritePin( CLK2_GPIO_Port, CLK2_Pin, GPIO_PIN_SET );
        HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CS3_GPIO_Port, CS3_Pin, GPIO_PIN_SET);
        //w = 0ul;
        //__delay_cycles( 6 );
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        /*CS2 = 0;*/
        HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
        for (int i = 0; i < 24; i++)
        {
          rslt_un.w <<= 1;
          /*if ( DATA2 != 0 )*/
          if (HAL_GPIO_ReadPin(DATA2CPU_GPIO_Port, DATA2CPU_Pin) != GPIO_PIN_RESET)
          {
            rslt_un.w |= 1ul;        //------------------------------------
          }
          /*CLK2 = 0;*/
          HAL_GPIO_WritePin(CLK2_GPIO_Port, CLK2_Pin, GPIO_PIN_RESET);
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
//          __delay_cycles(6);
          /*CLK2 = 1;*/
          HAL_GPIO_WritePin(CLK2_GPIO_Port, CLK2_Pin, GPIO_PIN_SET);
        }
        /*CS2 = 1;*/
//        HAL_SPI_Receive(&hspi2, rslt_un.bts, 3, 1)
        HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
//        rslt_bt = rslt_un.bts[0];
//        rslt_un.bts[0] = rslt_un.bts[2];
//        rslt_un.bts[2] = rslt_bt;
        if (p_cntr.proc_count_high == 0)
          pprs.PG0 += (rslt_un.w & 0xffff0) >> 4;
        else
          pprs.PGFull += (rslt_un.w & 0xffff0) >> 4;
        rslt_un.w = 0ul;
        //__delay_cycles( 6 );
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        /*CS3 = 0;*/
        rslt_un.w = 0;
        HAL_GPIO_WritePin(CS3_GPIO_Port, CS3_Pin, GPIO_PIN_RESET);
        //HAL_SPI_Receive(&hspi2, rslt_un.bts, 3, 1);
//        HAL_GPIO_WritePin(CS2_GPIO_Port, CS3_Pin, GPIO_PIN_SET);
//        rslt_bt = rslt_un.bts[0];
//        rslt_un.bts[0] = rslt_un.bts[2];
//        rslt_un.bts[2] = rslt_bt;
        for (int i = 0; i < 24; i++)
        {
          rslt_un.w <<= 1;
          /*if ( DATA2 != 0 )*/
          if (HAL_GPIO_ReadPin(DATA2CPU_GPIO_Port, DATA2CPU_Pin) != GPIO_PIN_RESET)
          {
            rslt_un.w |= 1ul;        //--------------------------------------------
          }
          /*CLK2 = 0;*/
          HAL_GPIO_WritePin(CLK2_GPIO_Port, CLK2_Pin, GPIO_PIN_RESET);
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
//          __delay_cycles(6);
          /*CLK2 = 1;*/
          HAL_GPIO_WritePin(CLK2_GPIO_Port, CLK2_Pin, GPIO_PIN_SET);
        }
        /*CS3 = 1;*/
        HAL_GPIO_WritePin(CS3_GPIO_Port, CS3_Pin, GPIO_PIN_SET);
        if (p_cntr.proc_count_high == 0)
        {
          pprs.PL0 += (rslt_un.w & 0xffff0) >> 4;
          ccr2 = htim1.Instance->CCR2 = 1400;
        }
        else
        {
          pprs.PLFull += (rslt_un.w & 0xffff0) >> 4;
        /*OCR1B = 1600;*/ //for advance proc_count
        ccr2 = htim1.Instance->CCR2 = 1600;
        }

        __no_operation();
        break;

      }
      /*OCR1B = 600;*/
      ccr2 = htim1.Instance->CCR2 = 600;
      __no_operation();
      break;
    case  600: //300 uS
      //PD = 1; // PD_ON
      HAL_GPIO_WritePin(PD_GPIO_Port, PD_Pin, GPIO_PIN_SET);
      /*OCR1B = 1000;*/ //next event on 0,5ms
      ccr2 = htim1.Instance->CCR2 = 1000;
      __no_operation();
      break;
    case 1000: //500 uS
               //CA = 1;                 //laser off
     HAL_GPIO_WritePin(CA_GPIO_Port, CA_Pin, GPIO_PIN_SET);
      /*OCR1B = 1400;*/
      ccr2 = htim1.Instance->CCR2 = 1400;
      __no_operation();
      break;
    case 1400: //700 uS
      if (
          ((p_cntr.proc_count_high == 0) ||
           (p_cntr.proc_count_high == 3) ||
           (p_cntr.proc_count_high == 5) ||
           (
            (p_cntr.proc_count_high >= 7) &&
            (p_cntr.proc_count_high <= 53)
           )
          ) &&
          (p_cntr.proc_count_low < 240)
         )
      {
        spi_cnt = 7;
        //<><><><><><>SPDR = 0;    // start SPI_SEQUENCE from pos7 to pos10
        {
          rslt_bt = 0;
//          for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
//          {
//            HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_RESET);
//            if (HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin) == GPIO_PIN_SET)
//            {
//              rslt_bt |= mask;
//            }
//            HAL_GPIO_WritePin(SPI1_CLK_GPIO_Port, SPI1_CLK_Pin, GPIO_PIN_SET);
//          }
          HAL_SPI_RxCpltCallback(NULL);
        }
        // HAL_SPI_Receive_IT( &hspi1, &rslt_bt, 1 );
        //HAL_SPI_TransmitReceive_IT( &hspi1, ( uint8_t * )&zro_bt, &rslt_bt, 1 );


      }
      /*OCR1B = 1600;*/
      ccr2 = htim1.Instance->CCR2 = 1600;
      __no_operation();
      break;
    case 1600: //800 uS
               //PD = 0; //PORTA=k_img;//PD_OFF
      //HAL_GPIO_WritePin(CA_GPIO_Port, CA_Pin, GPIO_PIN_SET);
      p_cntr.proc_count_low++;
      HAL_GPIO_WritePin(PD_GPIO_Port, PD_Pin, GPIO_PIN_RESET);
//      proc_count++;
      
       
        if (p_cntr.proc_count_low == 240)
        {
          p_cntr.proc_count_low = 0;
          p_cntr.proc_count_high++;
        }
     
       if (p_cntr.proc_count_high == 7) __no_operation();
      if (p_cntr.proc_count_low == 0)
      { // goto apply
        if (
            (p_cntr.proc_count_high == 4) ||
            (p_cntr.proc_count_high == 6)
           )
        {
          if ((xxxxPSFull / /*240ul*/xxxxPSFullmsrs) >= TEN_PERCENT_LIMIT)
          {
            p_cntr.proc_count_low = 0;
            p_cntr.proc_count_high = 8;
          }
       //   xxxxPS0 = xxxxPSFull = xxxxPS0msrs = xxxxPSFullmsrs= 0ULL;
           xxxxPSFull = xxxxPSFullmsrs= 0ULL;
        }
      }

      if (
          (p_cntr.proc_count_low == (MAX_PROC_COUNT % 240)) &&
          (p_cntr.proc_count_high == (MAX_PROC_COUNT / 240))
         )
      {

        p_cntr.proc_count_low++;
        light_on = false;
        dataready = true;
      }
      else
      {        
       if (
            (
             (p_cntr.proc_count_high >= 1) &&
             (p_cntr.proc_count_high <= 7)
            )
            ||
            (
              (p_cntr.proc_count_high >= 8)&&
             (p_cntr.proc_count_high <= 45) //&&
                                           //((p_cntr.proc_count_high & 1) == 0)
            )
           )
        {
         
          light_on = true;//reset
        /*  if(p_cntr.proc_count_high>=53)
            __no_operation();*/
        }
        else
        {
          light_on = false;//set
        }
      }
      /*OCR1B = 3000;*/ //never exist
      ccr2 = htim1.Instance->CCR2 = 0; //3000;
      __no_operation();
      break;
    default:
      break;
    }
//  DDRA=oldddra;
//  PORTA=oldporta;
#define OCF1B__ TIFR_Bit3
    /*TIFR_*/ //<><><><><><><><><> OCF1B__ = 1; //preventive clear interrupt flag on ocr1b compare
  }
}


#define MAX_STK_LVL (4)




/*double  PSstack[MAX_STK_LVL] = {NAN, NAN, NAN, NAN};

uint8_t stkidxPS   = 0;
uint8_t stk_lvlPS  = 0;*/

double  MORstack[MAX_STK_LVL] = {NAN, NAN, NAN, NAN};

uint8_t stkidxMOR  = 0;
uint8_t stk_lvlMOR = 0;



double  PS_0_stack[MAX_STK_LVL]  = {NAN, NAN, NAN, NAN};

uint8_t stkidxPS_0   = 0;
uint8_t stk_lvlPS_0  = 0;

double  vaisala_MORstack[10]  = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};

uint8_t vaisala_stkidxMOR  = 0;
uint8_t vaisala_stk_lvlMOR = 0;


void uint32_t_to_segleds_with_dspoints(int32_t val, uint8_t sign, char mark)
{  
  hcms_clear();        

  char tmpbuf[7];
  if(sign != ' ')
    //output string for temperature
    sprintf(tmpbuf, "% +3d.%02d%c", val/1000, abs(val / 10) % 100, mark); 
  else
    //for visibility
    sprintf(tmpbuf, "%2d.%03u", val/1000, val % 1000);
  
  hcms_putchars((char *)tmpbuf, 6); 
  hcms_putbuf();
}

double termo_val = 18;
uint8_t xbuf[256];



double get_double_from_pkt(uint8_t *datebts)
{
  double retval = 0.0;

  sscanf((char const *)&datebts[1],"%10lg",&retval);
  return retval;
}

uint16_t calculating_the_checksum (uint8_t *buf, int index)
{
  uint16_t chk = 0;
  
  for (int i = 1; i < index; i++)
    chk += (uint16_t)buf[i];     
     
  chk = (chk & 0xff) + ((chk >> 8) & 0xff);
 
  return chk;
}

void putpkt(uint8_t state, uint8_t mode, double val)
{
  uint8_t obuf [17];
  init_serv_obf(state, mode, obuf);
  uint16_t chk =  calculating_the_checksum (obuf, 14);

  obuf[14] = ((chk >> 4) & 0xf) + 0x40;
  obuf[15] = (chk & 0xf) + 0x40;
  HAL_UART_Transmit(&huart4, obuf, 17, 150);
  HAL_UART_Transmit(&huart5, obuf, 17, 150);

  HAL_Delay(25);
}

bool verification_checksum(int index)
{
  uint8_t chkhi;
  uint8_t chklo;
  uint16_t chk = calculating_the_checksum (ibuf, index);
  
     
  chk = (chk & 0xff) + ((chk >> 8) & 0xff);
  chkhi = ((chk >> 4) & 0xf) + 0x40;
  chklo = (chk & 0xf) + 0x40;
  if ((ibuf[index] == chkhi) && (ibuf[index+1] == chklo))
    return 1;
  else
    return 0;
}


void getpkt(void)
{   
  HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET);
  
  if ( u4rlen >=3) //if (ibuf_rdy)
  {

      if (verification_checksum(6))
      {   
       if (ibuf[1]==0x43 && ibuf[2]==0x4C && ibuf[3]==0x4F && ibuf[4]==0x53) 
         flag_out = 0;        
      }
     u4rlen = 0;
     huart4.RxXferCount = 0x100;
     huart4.RxXferSize = 0x100;
     huart4.pRxBuffPtr = ibuf;
  }  
}


int count_retry=0;
uint8_t lost_answers=0;
//uint8_t report[4]={0x01,0x00,0x06,0x0D};
void put_profile(uint8_t index, uint16_t  size_profile)
{
  static uint8_t buf_profile[265];
  uint16_t  i=0;
  uint16_t  j=5;
  uint16_t  size_package=0x105;//???
  
  uint16_t  buf_index=index*128; //???

  buf_profile[0] = 0x01;
  buf_profile[1] = ((size_profile-1)/128) - ((size_profile % 128 == 0)? (1) : (0));//0x50;

  buf_profile[2] = index;
  buf_profile[3] = 400/xcoeff;
  buf_profile[4] = 0x02;
  for ( j = 5; j < size_package;j+=2,buf_index+=1)
  {
    if ( buf_index <= size_profile )
    {
      buf_profile[j+1] = profile[ buf_index ];
      buf_profile[ j ] = profile[ buf_index ] >> 8 ;
    }
    else
      break;
  }

  buf_profile[ j ] = 0x03;
  
  uint16_t chk =  calculating_the_checksum (buf_profile, j);
  
  buf_profile[ j+1 ] = ((chk >> 4) & 0xf) + 0x40;
  buf_profile[ j+2 ] = (chk & 0xf) + 0x40;
  buf_profile[ j+3 ] = 0xd;
  oidx = 0;
    //M0=1;M1=0;T485_EN=1;//tx
//  HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_SET);
  /*M1=0;*/
//  HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET);
  /*T485_EN=1;//tx */
  HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET);
  
  HAL_UART_Transmit( &huart4, buf_profile, j+4,110);   
  HAL_UART_Transmit( &huart5, buf_profile, j+4,110);   

 // HAL_Delay(20);           //rjvtyn
}
 uint8_t ibuf[256]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 void mode_profile()
 {
  int size_profile=0;
  for ( int i = COUNT_MEASURES-1;  i >=0 ;i--)
  {
    if( profile[i]!=0x00)
    {
      size_profile=i;
      break;
    }
  }
  uint8_t index = 0;
  for ( int i = size_profile;  i > 0;)
  {
   
    HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
    put_profile( index, size_profile);
//    HAL_GPIO_WritePin( M0_GPIO_Port, M0_Pin, GPIO_PIN_RESET );
//    HAL_GPIO_WritePin( M1_GPIO_Port, M1_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
    huart4.RxXferCount = 0x100;
    huart4.RxXferSize = 0x100;
    huart4.pRxBuffPtr = ibuf;
    HAL_UART_Receive_IT( &huart4, ibuf, 4);
  
  //  HAL_UART_Receive( &huart5, report, 4, 70);

    

    if(ibuf[1]== 0x3f && ibuf[2]==0x3f)
    {
     NVIC_SystemReset();
    }
      if ( ibuf[ 2 ] == 0x05 )
      {
       
       if ( count_retry >= 2 )
       {
         
         count_retry=0;
         break;
       }
       count_retry += 1;
      }
      else 
      { 
        count_retry = 0;
        i -= 128;
        index += 1;
        HAL_Delay(1);
        if (ibuf[2] == 0x0) //the package was lost
       {
         if(tim8_off==0 && lost_answers>=120)
         {
           HAL_TIM_Base_Start_IT(&htim8);
           tim8_off=1;        
         }
         lost_answers+=1;
       }
       else
       {
         if(ibuf[2]==0x06)
         lost_answers=0;
         if(tim8_off==1)
         {
           HAL_TIM_Base_Stop_IT(&htim8); 
           tim8_off=0;
         } 
       }
         
      }
    ibuf[2] = 0x06;
    HAL_Delay(70);  

//    HAL_GPIO_WritePin( M0_GPIO_Port, M0_Pin, GPIO_PIN_SET );
//    HAL_GPIO_WritePin( M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET );
//    HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
   }
}

//
/*void test_profile( FILE *file_profile, int  count_profile, int sym)            ////////test
{
  uint32_t numb1=0x01;
  uint32_t numb2=0x01;
  uint32_t numb3=0x01;
  uint32_t numb4=0x01;
  int time=0;

  char buffer1[1000];
  char* end;

  int count=0;
  for(int i=0; i<10560;i++)
     profile[i] = 0;

  int j=0;
  while (sym!=EOF)
  {
      fgets(buffer1,815, file_profile);
      if(buffer1[15-time]=='0' && buffer1[16-time]=='0' && count>0)
      {
          fseek (file_profile,-900,SEEK_CUR);
           fgets(buffer1,100, file_profile);
           break;
      }
      else
    {
    count++;

    int index=24-time;
    while(1)
    {

      numb1= strtol(&buffer1[index],&end,16);
      index+=3;
      numb2=strtol(&buffer1[index],&end,16);
      index+=3;

       if(numb1==0x03)
       {
         numb4 = strtol(&buffer1[index+3],&end,16);
         if(numb4==0x0D)
         {
           index+=6;
           break;
         }
       }
       profile[j]=(numb1<<8)+numb2;
       j++;
    }
     while(buffer1[index]!=0x0A)
     {
       if((buffer1[index])=='\0')
       {
          sym = fgetc (file_profile);
          break;
       }
       index+=1;
     }
  }
  }

  if(sym == EOF)
  {
    rewind(file_profile);
    count_profile=0;
    sym=0;
  }
  else
    count_profile++;
}*/////end test

/*struct Thresholds
{
 
  uint16_t Clearthreshold = 150;
 /* uint16_t Noisethreshold = 75;
  uint16_t Hazethreshold = 50;
  uint16_t Fogthreshold = 400;
  uint16_t Diffdrizzlethreshold=125;
  uint16_t Diffprecipitationthreshold=300;*/
/*   Thresholds(uint16_t b = 150, uint16_t=75)
        : Clearthreshold(b)
        { }
 
};

*/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_UART4_Init();
  MX_TIM1_Init();
  MX_IWDG_Init();
  MX_TIM8_Init();
  MX_TIM15_Init();
  MX_USART2_UART_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
  hcms_init();
  hcms_clear();
  hcms_putchars((char *)"strt", 5);
  hcms_putbuf();
  //init_obf();
  termo_init();
 
//  do
//  {
//    for(int i=0 ;i<16;i++)
//      xbuf[i]=i;
//    sEE_Write(xbuf,0,16);
//  sEE_Read(xbuf,0,256);
//  }while(1);
//MyEeprom        eeprom;       // Eeprom-derived device driver
/*
#if ERASE_EEPROM
 eeprom.erase();
#endif
 
  NvStore         nvStore(eeprom, EeAddr(0), EeAddr(65535));      // EeAddr(1024), EeAddr(65535)      // storage layout manager
//Nv < Commands > commands( GET_STARTP_PROFILE, nvStore, "commands" );*/


    
  flag_unification =  read_in_mem("unification",(0),384);  
  flag_cs =  read_in_mem("CS",(0),4672);   



  pprs.CORR = read_in_mem("CORR_val",(-3.5),256);
  //Nv<sIGMA>    SIGMA(-3.14159265358979323, nvStore, "SIGMA");
  pprs.SIGMA = read_in_mem("SIGMA",(405000),320);
  
  //Nv <Scattering_coefficient> Scattering(1.0, nvStore, "Scattering");
 // thresholds.scattering_coefficient = read123("Scattering",(1.0),384);
  
  //Nv <Vaisala> vaisalaflag(0, nvStore, "vaisalaflag");
  int type_package= (int)read_in_mem("type",(0),1984);//384);
   flag_peleng = flag_vaisala =  flag_biral = flag_vaisala_fs= 0;
   if(type_package!=5 && !(type_package>9 && type_package<38))
     flag_peleng=1;
   if(type_package==5)
      flag_biral=1;
   if(type_package>9 && type_package<28)
   {
     ms_adr[0] = 'P'; ms_adr[1] = 'W'; 
     ms_adr[2] = ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
     ms_adr[3] = rs_adr % 10 + '0';
     message = type_package%10; 
     flag_vaisala = 1;
     if(type_package>19)
       PWandFD=1;
   }
    if(type_package>30 && type_package<38)
    {
      ms_adr[0] = 'F'; ms_adr[1] = 'D'; 
      ms_adr[2] = ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
      ms_adr[3] = rs_adr % 10 + '0';
      message = 2;
      flag_vaisala_fs = 1;
    }
         
   
 
  //Nv <Clearthreshold> Clear(70, nvStore, "Clear");  //150
  thresholds.clear = read_in_mem("Clear",(65),640);
 // Nv <Noisethreshold> Noise(40, nvStore, "Noise");  //75
  thresholds.noise =  read_in_mem("Noise",(35),704);
 // Nv <Hazethreshold> Haze(30, nvStore, "Haze");
  thresholds.haze = read_in_mem("Haze",(25),768);
 // Nv <Fogthreshold> Fog(201, nvStore, "Fog");
  thresholds.fog = read_in_mem("Fog",(150),832);
  
 // Nv <Drizzlethreshold> Drizzle(64, nvStore, "Drizzle");//125
  thresholds.drizzle =  read_in_mem("Drizzle",(90),896);
//  Nv <Precipitationthreshold> Precipitation(160, nvStore, "Precipitation");
  thresholds.precipitation = read_in_mem("Precipitation",(200),960);
 
 // Nv <Limitcipitation> Limit(70, nvStore, "Limit");
  thresholds.limitcipitation = read_in_mem("Limit",(60),1024);
 // Nv <Sumlimit> Sumlim(250, nvStore, "Sumlim");
  thresholds.sumlimit =  read_in_mem("Sumlim",(450),1088);

  thresholds.slp = read_in_mem("SLP",(400),1152);
 // double PLref= 1;//read_in_mem("PLref",(1),1216);  
  
  flag_energy = read_in_mem("energy",(0),1216) ; 
  
  thresholds.limsnow = read_in_mem("limsnow",(1.6),1280);  
  thresholds.minduration = read_in_mem("minduration",(4),1600);
  thresholds.minderivative = read_in_mem("minderivative",(200),1664) ;
  thresholds.clearp = read_in_mem("clearp",(0.5),1728);
  thresholds.l = 0.02;//read123("L",(0.02),1792);

  thresholds.flag_hail= read_in_mem("hail",(1),3136);
  thresholds.limspeedhail= read_in_mem("speed_hail",(15),3200);
  thresholds.mindurationhail= read_in_mem("duration_hail",(2),3264);
  
  pprs.scattering_coefficient = read_in_mem("Scattering",(1),2560);
  thresholds.t_corr = read_in_mem("t_corr",(-20),1408);
  thresholds.out_t_corr = read_in_mem("out_t_corr",(-3.5), 3776);
  //    double SIGMA = read123("SIGMA",(-3.14159265358979323),320);
  //    double CORR = read123("CORR_val",(-3.14159265358979323),256);
      double PM=0.1;
      
  pprs.PSoffset = read_in_mem("PSoffset", (0),3584);

  thresholds.termo = read_in_mem("Termo", (0),3712);
  
  read_pollution_parameters();
  read_approximation_parameters();

 
  //MX_UART4_Init();
  get_ROMid();
  get_Temperature();  
  HAL_TIM_Base_Start_IT(&htim1);
  uint32_t tmeas_started_time = 0UL;


  

 
  
  termo_val=select_temperature();
 
  if(flag_error.temperature_outside==1 && flag_error.temperature_inside==1)
  {
    hcms_putchars("error", 5); 
    hcms_putbuf();
  }
  else
  {
    if((thresholds.termo && !flag_error.temperature_outside)||
       (!thresholds.termo && flag_error.temperature_inside))
      uint32_t_to_segleds_with_dspoints(termo_val*1000, 0x2B,  0x6F);
    else
      uint32_t_to_segleds_with_dspoints(termo_val*1000, 0x2B,  0x69);
  }
  enter_heating(termo_val);
  
  HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET);
  HAL_Delay(20);
  first_putpkt();
    
  HAL_GPIO_WritePin( M0_GPIO_Port, M0_Pin, GPIO_PIN_SET );
  HAL_GPIO_WritePin( M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET );
          /*M0=0;*/
//    HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_RESET);
          /*M1=1;*/
//    HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_SET);
          /*T485_EN=0;//rx */
  HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET);

 
  p_cntr.proc_count_low = p_cntr.proc_count_high = 0;
 //test value
 // FILE *file_profile =  fopen("C:\\Profile.txt", "r");
 // if(flag_vaisala)
 // huart4.Init.BaudRate=9600;
 //test value

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
    uint32_t send_val=0;

   /* if (((get_time() - tmeas_started_time) >= 1000UL) && ( //proc_count==(MAX_PROC_COUNT+1)
                                                           (p_cntr.proc_count_low == ((MAX_PROC_COUNT + 1) % 240)) &&
                                                           (p_cntr.proc_count_high == ((MAX_PROC_COUNT + 1) / 240))))
    {
        tmeas_started_time = get_time();      
    }*/
    
    if (timeflag && (p_cntr.proc_count_high==53 && !dataready) )
    {

      pprs.PL0 = pprs.PG0 = xxxxPS0 =  pprs.PLFull = pprs.PGFull = xxxxPSFull = xxxxPS0msrs = xxxxPSFullmsrs= 0ULL;
      timeflag = false;

      hcms_clear();

      p_cntr.proc_count_low = p_cntr.proc_count_high = 0;
      termo_val = select_temperature();
      start_heating_according_to_temp(termo_val);
      if(flag_error.temperature_outside==1 && flag_error.temperature_inside==1) 
        termo_val=18;
  
    }
    if ( dataready ) // check for ready of accumulated data
    {     
      if ( flag_profile == 1 )
      {      
        mode_profile();
      }
      if(flag_profile==0  )
     { 
       pprs.PL = (( pprs.PLFull/*>>4*/) / 240UL);   //  make output values from accumulated data
       PL0x = ((pprs.PL0/*>>4*/) / 240UL);
       if ( pprs.PL < PL0x ) //check if is negative value
       {
         pprs.PL = PL0x;                //correct it
       //  flag_Kp=1;
     //    flag_pl=1;
       }
       pprs.PL = pprs.PL - PL0x;
    //   push(stk_lvlPL,4, stkidxPL, pprs.PL, PLstack );
       pprs.PG = ((pprs.PGFull/*>>4*/) / 240UL);
       PG0x = ((pprs.PG0/*>>4*/) / 240UL);
       if ( pprs.PG < PG0x )             // check if negative value
         pprs.PG = PG0x;                //correct  it
       pprs.PG = pprs.PG - PG0x;
       push(stk_lvlPG, 10, stkidxPG, pprs.PG, PG_stack);
       pprs.PS = (xxxxPSFull/*/16ULL)*/) / (( double )/*COUNT_MEASURES*/xxxxPSFullmsrs);
       // push(stk_lvlPSfull, 4, stkidxPSfull, pprs.PS, PSfull_stack );
       // pprs.PS = pop(stk_lvlPSfull, PSfull_stack);
       pprs.PS0 = ( double )(xxxxPS0) / (( double )xxxxPS0msrs);
            
      // push(stk_lvlPSfull, 4, stkidxPSfull, pprs.PS, PSfull_stack );
       push(stk_lvlPS_0, 4, stkidxPS_0, pprs.PS0, PS_0_stack );
       pprs.PS0 = pop(stk_lvlPS_0, PS_0_stack);
     

       pprs.PSmomentary =( double ) ( (400UL / xcoeff) *(pprs.PS - pprs.PS0));
       pprs.PSmomentary -= delta;
       
       if(flag_cs) 
         pprs.PSmomentary *=pprs.scattering_coefficient;
       
        
        pprs.MORmomentary = calculate_visibility(pprs.PSmomentary);  
    //    push(stk_lvlPS,4, stkidxPS, pprs.PSmomentary, PSstack );
        
        calibration_value.PSmsb = pprs.PSmomentary;
        termo_compensation();
        LED_deterioration();
        glass_contaminate();
        calibration_value.MORmsb = calculate_visibility(calibration_value.PSmsb);

/**********************************************************************************/ 

  
  if( pprs.MORmomentary > limit_visibility)
     pprs.MORmomentary = limit_visibility;
   if( pprs.MORmomentary >=0)
   push(stk_lvlMOR,4, stkidxMOR, pprs.MORmomentary, MORstack );
 
   uint32_t_to_segleds_with_dspoints( ( uint32_t )pprs.MORmomentary, 0x20, 0x20);
   
   pprs.MORstacked = pop(stk_lvlMOR, MORstack);
   if( pprs.MORstacked > limit_visibility)
     pprs.MORstacked=limit_visibility;
   
   if(stkidxMOR==3)
     push(vaisala_stk_lvlMOR,10, vaisala_stkidxMOR, pprs.MORstacked, vaisala_MORstack );
    send_val= pprs.MORstacked;

//  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   index_phenocode=weather( pprs.MORstacked, termo_val); 
     //  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  
  } //end no profile    
      
      if (flag_out == 0 && flag_profile==0 && flag_console==0 )
      { 
                     
        if (huart4.RxXferCount <= 253 || huart5.RxXferCount <= 253 ) //if (ibuf_rdy)       
        {
          __HAL_UART_CLEAR_IDLEFLAG( &huart4 );
         __HAL_UART_ENABLE_IT( &huart4, UART_IT_IDLE );
         huart4.RxXferCount=256;
         huart5.RxXferCount=256;
         HAL_UART_AbortReceive(&huart4);
         HAL_UART_AbortReceive(&huart5);

          //checking crc
         if (verification_checksum(14))
          {
            if (ibuf[3] == 0x33)//mode=COMMAND //set mode
            {                
                uint32_t MORinput = 0ul;
                                  
                for (uint16_t k = 1, j = 13; j > 4; k *= 10, j--)
                  if (ibuf[j] >= '0')
                    MORinput += (uint32_t)(ibuf[j] - '0') * (uint32_t)k;                     
                
                if (MORinput == 0x5a5a) 
                {
                  HAL_UART_AbortReceive(&huart4);
                  HAL_UART_AbortReceive(&huart5);

                  flag_out = 1;
                  HAL_TIM_Base_Start_IT(&htim8);
                }
                
                if (MORinput == 0x5b5b )
                {
                  flag_profile = 1;
                   if ( huart4.Init.BaudRate != 57600 )  
                   {                      
                     HAL_UART_DeInit( &huart4 );
                     HAL_UART_DeInit( &huart5 );
                     huart4.Init.BaudRate = 57600;
                     huart5.Init.BaudRate = 57600;
                     HAL_UART_Init( &huart4 );
                     HAL_UART_Init( &huart5 );
                   }
                    ibuf[1]= 0x00; ibuf[2] = 0x06;
                }
            }
          } 
          hstd= HAL_UART_Receive_IT(&huart4, ibuf, 256);
          hstd= HAL_UART_Receive_IT(&huart5, ibuf, 256);                  
        }
       if (flag_out == 0 && flag_profile==0 && flag_console==0 )
         putpkt(send_val, ms_adr, index_phenocode); // send prepared buffer
     }     
         
      else
      {
        
       
        delayed_recording();
        if(flag_out==1)
        {  
           pprs.PS = (double) 400/xcoeff* pprs.PS;     
         
          putpkt_debug(/*MORmsb,
                     PSmsb,
                     MORlsb,
                     PSlsb,
                     CORR_val,
                     SIGMA,
                     ((double)(400UL / xcoeff_)) * PS,
                     ((double)(400UL / xcoeff_)) * PS0x,
                     PSmomentary,
                     MORmomentary*/
                     &pprs
                    );
         getpkt();

        }
      }
    
      dataready = false;   
      
    }
   
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_TIM15|RCC_PERIPHCLK_TIM8;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim15ClockSelection = RCC_TIM15CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
