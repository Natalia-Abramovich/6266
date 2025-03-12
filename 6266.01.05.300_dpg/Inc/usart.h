/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
#define   IB_STATE_IDLE   (0)
#define   IB_STATE_SECOND_SYNC (1)
#define   IB_STATE_FIRST_LEN (2)
#define   IB_STATE_SECOND_LEN (3)
#define   IB_STATE_CONTENT (4)
#define   IB_STATE_FIRST_CHK (5)
#define   IB_STATE_SECOND_CHK (6)
#define   IB_STATE_PKT_RDY (7)

struct istruct__
{
  uint8_t  hdr_spc;
  uint8_t  hdr_typeofunit;
  uint8_t  stsbt;
  uint8_t  modebt;
  uint8_t  date0bt;
  uint8_t  date1bt;
  uint8_t  date2bt;
  uint8_t  date3bt;
  uint8_t  date4bt;
  uint8_t  date5bt;
  uint8_t  date6bt;
  uint8_t  date7bt;
  uint8_t  date8bt;
  uint8_t  date9bt;
  uint8_t  crc0bt;
  uint8_t  crc1bt;
  uint8_t  eopbt;
};


extern volatile uint8_t iidx;
extern volatile bool ibuf_rdy;

//extern uint8_t obuf [17];
extern volatile uint8_t oidx;
/*
class p
{
public: 
 
    uint32_t PLFull;
  uint32_t PL0;
  uint32_t PL;
  uint32_t PGFull;
  uint32_t PG0;
  uint32_t PG;
  uint32_t MORmsb;
  uint32_t PSmsb;
  uint32_t MORlsb;
  double   PSlsb;
  double   CORR;
  double   SIGMA;
  double   PSfull_momentary;
  double   PS0_momentary;
  double   PSmomentary;
  double   MORmomentary;
p() {  }
};

extern p pprs;*/
struct putpars__
{
  uint32_t PLFull=0;
  uint32_t PL0=0;
  uint32_t PL=0;
  uint32_t PGFull=0;
  uint32_t PG0=0;
  uint32_t PG;
  double   CORR;
  double   SIGMA;
  double   PS=0;
  double   PS0=0;
  double   PSmomentary=0;
  double   MORmomentary;
  double   MORstacked;
  double   scattering_coefficient;
  double  PSoffset;
};

extern struct putpars__ pprs;

struct calibration_value__
{
  double MORmsb;
  double PSmsb;
  double MORlsb;
  double   PSlsb;
};

extern struct calibration_value__ calibration_value;


struct uni_calibration_value__
{
  double MORmsb;
  double PSmsb;
  double MORlsb;
  double   PSlsb;
};


extern struct uni_calibration_value__ uni_calibration_value;

extern volatile uint16_t u4rlen;
extern int speed; 
extern int del;
extern int out_del;
extern int word_length; 
//extern void putpkt( uint32_t send_val,uint16_t pogoda );
extern void init_serial( void );
extern void init_obf( uint8_t* obuf );
extern void init_serv_obf(uint8_t state, uint8_t mode, uint8_t* obuf);
//extern void putpkt(uint8_t state, uint8_t mode, double val);
extern void putpkt_debug(struct putpars__ * pprs_ptr/* double MORmsb,
                           double PSmsb,
                           double MORlsb,
                           double PSlsb,
                           double CORR,
                           double SIGMA,
                           double PSfull_momentary,
                           double PS0_momentary,
                           double PSmomentary,
                           double MORmomentary*/
                         );
struct outbuf_descr___
{
   struct buf_state___
   {
      uint8_t curr_state :2; // 00 buffer is free, !=0 busy, 01 - prepare, 02 - in output process
   }bs;
   uint8_t *buf_ptr;
   uint16_t infolen;
   uint16_t infoidx;
};

struct outbuf___
{
   struct outbuf_descr___ obd;
   uint8_t infobuf [256];
};

extern void u4_rcv_pkt_IT( void );
extern int find_subarray( uint8_t *, char *, int);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

