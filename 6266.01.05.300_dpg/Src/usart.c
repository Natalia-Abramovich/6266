/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "serial.h"
#include <string.h>
#include "typevals.h"
#include <stdio.h>
#include "approximation.h"

//p pprs;
struct putpars__ pprs;
struct calibration_value__ calibration_value;
struct uni_calibration_value__ uni_calibration_value;

struct outbuf___ obf;





volatile uint8_t      iidx     = 0;
volatile bool    ibuf_rdy = false;
volatile uint8_t      oidx     = 17 + 1;

int del = 0;
int out_del = 0;
volatile uint16_t u4rlen   = 0xffff;


/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 1200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 1200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = TXD_Pin|RXD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_UART4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */
    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_UART5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_UART5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PB3     ------> USART2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOC, TXD_Pin|RXD_Pin);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();

    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PB3     ------> USART2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


// idxses for input pkt
// 0 space             0x20
// 1 type of pkt       0x31
// 2 5 digit of MDW
// 3 4 digit of MDW
// 4 3 digit of MDW
// 5 2 digit of MDW
// 6 1 digit of MDW
// 7 5 digit of rsrv
// 8 4 digit of rsrv
// 9 3 digit of rsrv
//10 2 digit of rsrv
//11 1 digit of rsrv
//12 hi chk
//13 lo chk
//14 cr                 0xd


//#pragma vector = USART1_RXC_vect
//__interrupt void USART1_RX( void )
//{
//  uint8_t               stat      = UCSR1A,
//     sym  = UDR1;
////  static uint16_t blocklen;
//  if ( (stat & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0 )
//  {
//    if ( ibuf_rdy == false )
//    {
//      if ( iidx == 0 )
//      {
//        if ( sym != 0 )
//        {
//          if ( sym == 0x20 )
//          {
//            ibuf[ iidx++ ] = sym;
//          }
//        }
//      }
//      else
//      {
//        if ( iidx < sizeof(ibuf) )
//        {
//          ibuf[ iidx++ ] = sym;
//          if ( sym == 0xd )
//          {
//            uint8_t  i;
//            uint16_t chk = 0;
//            for ( i = 1; i < 12; i++ )
//            {
//              chk += ( uint16_t )ibuf[ i ];
//            }
//            chk = (chk & 0xff) + ((chk >> 8) & 0xff);
//            if ( (((chk >> 4) & 0xf) == (ibuf[ 12 ] - 0x40)) &&
//                 ((chk & 0xf) == (ibuf[ 13 ] - 0x40))
//               )
//            {
//              if ( ibuf[ 1 ] == 0x31 )
//              {
//                ibuf_rdy = true;
//                return;
//              }
//              else
//                iidx = 0;
//            }
//            else
//            {
//              iidx = 0;
//              return;
//            }
//          }
//        }
//        else
//        {
//          iidx = 0;
//        }
//      }
//    }
//  }
//}
//

//#pragma vector = USART1_TXC_vect
//
//__interrupt void USART1_TXC( void )
//{
//  UCSR1A_TXC1__ = 1; //clear TXC interrupt flag
//  UCSR1B_TXCIE1__ = 0; //disable TXC interrupt
//  if ( oidx == sizeof(obuf) )
//  {
//    oidx = sizeof(obuf) + 1;
//  }
//}
//



//#pragma vector = USART1_UDRE_vect
//__interrupt void USART1_UDRE( void )
//{
//  if ( oidx < sizeof(obuf) )
//  {
//    UDR1 = obuf[ oidx++ ];
//  }
//  else
//  {
//    UCSR1B_UDRIE1__ = 0;
////    UCSR1A_TXC1__=1;//clear TXC interrupt flag
//    UCSR1B_TXCIE1__ = 1; //enable TXC interrupt
//
//  }
//}
//










void init_serial( void )
{
//// USART0 initialization
//// Communication Parameters: 8 Data, 1 Stop, No Parity
//// USART0 Receiver: On
//// USART0 Transmitter: On
//// USART0 Mode: Asynchronous
//// USART0 Baud Rate: 38400
//  #define BAUD_SIO (1200UL)
//  UCSR1A = 0x00;
////SFR_B_N(0xC1, UCSR0B, RXCIE0, TXCIE0, UDRIE0, RXEN0, TXEN0, UCSZ02, RXB80, TXB80)
////                        1       0       1       1      1      0       0      0
////                        0       0       0       1      1      0       0      0
////  UCSR0B=BIN8(11111000);//0xb8;//0xD8;
//  UCSR1B = BIN8( 00011000 ); //0xb8;//0xD8;
//  UCSR1C = 0x06;
//  #define UBRR_SIO (XTAL_FREQ/16UL/BAUD_SIO-1)
//  UBRR1H = (UBRR_SIO >> 8) & 0xffUL;
//  UBRR1L = (UBRR_SIO & 0xffUL);
////  PDE495=0;
}



 /*
uint8_t crc_ibuf( void )
{
  uint8_t  i;
  uint16_t chk = 0;
  for ( i = 1; i < 14; i++ )
  {
    chk += ( uint16_t )ibuf[ i ];
  }
  return ( uint8_t )((chk & 0xff) + ((chk >> 8) & 0xff));
}  */

/*uint8_t crc_obuf( void )
{
  uint8_t  i;
  uint16_t chk = 0;
  for ( i = 1; i < 12; i++ )
  {
    chk += ( uint16_t )obuf[ i ];
  }
  return ( uint8_t )((chk & 0xff) + ((chk >> 8) & 0xff));
}  */

//  idxses of output pkt
// 0 space                 0x20
// 1 type of pkt   init of 0x31  \
// 2 state                 0x30   |
// 3 mode                  0x31   |
// 4 5 digit of mdw               |
// 5 4 digit of mdw               |
// 6 3 digit of mdw               |
// 7 2 digit of mdw               |
// 8 1 digit of mdw               |
// 9 5 digit of rsrv       0x30   |
//10 4 digit of rsrv       0x30   |
//11 3 digit of rsrv       0x30   |
//12 2 digit of rsrv       0x30   |
//13 1 digit of rsrv       0x30   /
//14 high digit of chk
//15 low  digit of chk
//16 eop                   0xd


void init_obf( uint8_t* obuf )
{
  obuf[ 0 ] = 0x20;
  obuf[ 1 ] = 0x31;
  obuf[ 2 ] = 0x30;
  obuf[ 3 ] = 0x31;
  obuf[9] = obuf[10]=obuf[11] = obuf[12] = obuf[ 13 ] =0x30;
  
  obuf[ 16 ] = 0xd;
  oidx = sizeof(obuf) + 1;
}

void init_serv_obf(uint8_t state, uint8_t mode, uint8_t* obuf)
{
  obuf[0] = 0x20;
  obuf[1] = 0x31;
  obuf[2] = state;
  obuf[3] = mode;
  obuf[16] = 0x0d;
}

//void putpkt(uint8_t state, uint8_t mode, double val)
//{
//  init_serv_obf(state, mode);
//  sprintf((char *)&obuf[4], "%010.0f", val);
//  uint16_t chk = 0;
//  for (i = 1; i < 14; i++)
//  {
//    chk += (uint16_t)obuf[i];
//  }
//  chk = (chk & 0xff) + ((chk >> 8) & 0xff);
//  obuf[14] = ((chk >> 4) & 0xf) + 0x40;
//  obuf[15] = (chk & 0xf) + 0x40;
//  HAL_UART_Transmit(&huart4, obuf, 17, 150);
//  HAL_Delay(25);
//}
//
//
//
//void putpkt(uint32_t send_val, uint16_t pogoda = 0xffff)
//{
//  uint8_t  i;
//  uint16_t chk = 0;
////  while ( oidx < (sizeof(obuf) + 1) )
////    continue;
//  obuf[4] = ((send_val / 10000ul) % 10ul) + '0';
//  obuf[5] = ((send_val / 1000ul) % 10ul) + '0';
//  obuf[6] = ((send_val / 100ul) % 10ul) + '0';
//  obuf[7] = ((send_val / 10ul) % 10ul) + '0';
//  obuf[8] = (send_val % 10ul) + '0';
//  if (pogoda!=0xffff)
//  {
//    obuf[9] = (pogoda >> 8) & 0xff;
//    obuf[10] = pogoda & 0xff;
//  }
//  for (i = 1; i < 14; i++)
//  {
//    chk += (uint16_t)obuf[i];
//  }
//  chk = (chk & 0xff) + ((chk >> 8) & 0xff);
//  obuf[14] = ((chk >> 4) & 0xf) + 0x40;
//  obuf[15] = (chk & 0xf) + 0x40;
//  oidx = 0;
//  //UCSR1B_UDRIE1__ = 1;
//  HAL_UART_Transmit(&huart4, obuf, 17, 150);
//  HAL_Delay(25);
//}
//
void init_obf___( void )
{
  obf.obd.bs.curr_state = 0;
  obf.obd.buf_ptr = &obf.infobuf[ 0 ];
}

struct outbuf___* get_buf_ptr( void )
{
  if ( IB_STATE_IDLE == obf.obd.bs.curr_state )
  {
    return &obf;
  }
  return NULL;
}



uint16_t crc_block( uint8_t *Array, uint16_t len )
{
  register uint8_t carry;
  register uint8_t CRC_Low  = 0;   /* low 8 bits of CRC */
  register uint8_t CRC_High = 0;  /* high 8 bits of CRC */
  while ( 0 != (len--) )
  {
    //WDR();
    carry = CRC_High ^ *Array++;
    carry = carry ^ (carry >> 4);
    CRC_High = CRC_Low ^ (carry << 4) ^ (carry >> 3);
    CRC_Low  = carry ^ (carry << 5);
  }
  return( uint16_t )CRC_Low | ((( uint16_t )CRC_High) << 8);
}


#define HDR_LEN (16)
struct outbuf_descr___* prepare_out_buf( void )
{
  struct outbuf_descr___ *rslt;
  {
    if ( obf.obd.bs.curr_state == OBUF_STATE_FREE )
    {
      rslt = &obf.obd;
    }
    else
      rslt = NULL;
  }
  if ( NULL != rslt )
  {
    rslt->bs.curr_state = OBUF_STATE_PREP;
    memset( &rslt->buf_ptr[ 0 ], 0xff, HDR_LEN );
    *(( uint8_t * )&rslt->buf_ptr[ 0 + HDR_LEN ]) = 0xaa;
    *(( uint8_t * )&rslt->buf_ptr[ 1 + HDR_LEN ]) = 0x55;
    *(( uint16_t * )&rslt->buf_ptr[ 2 + HDR_LEN ]) = 3;
    rslt->buf_ptr[ 4 + HDR_LEN ] = BLK_TYPE_CONTAINER;
    rslt->infolen = rslt->infoidx = 5 + HDR_LEN;
  }
  return rslt;
}



int16_t get_free_size( struct outbuf_descr___ *topkt )
{
  uint16_t maxfreesize = sizeof(obf.infobuf);
  return maxfreesize - topkt->infolen;
}

uint8_t appnd2pkt_str( struct outbuf_descr___ *topkt, uint8_t typ, uint8_t *str_ptr )
{
  uint8_t strsize = strlen( ( char const * )str_ptr ) + 1;

  if ( get_free_size( topkt ) < (strsize + 3) )
    return 1;

  *(( uint16_t * )&topkt->buf_ptr[ 2 + HDR_LEN ]) += 2;
  *(( uint16_t * )&topkt->buf_ptr[ topkt->infoidx ]) = strsize + 3;
  topkt->infoidx += 2;
  topkt->infolen += 2;

  (*(( uint16_t * )&topkt->buf_ptr[ 2 + HDR_LEN ]))++;
  *(( uint8_t * )&topkt->buf_ptr[ topkt->infoidx ]) = typ;
  topkt->infoidx++;
  topkt->infolen++;

  *(( uint16_t * )&topkt->buf_ptr[ 2 + HDR_LEN ]) += strsize;
  memcpy( &topkt->buf_ptr[ topkt->infoidx ], str_ptr, strsize );
  topkt->infoidx += strsize;
  topkt->infolen += strsize;
  return 0;
}

uint8_t appnd2pkt_dble( struct outbuf_descr___ *topkt, uint8_t typ, double dbl )
{
  uint8_t str [32];
  sprintf( ( char * )str, "%.12g", dbl );
  return appnd2pkt_str( topkt, typ, str );
}

uint8_t set_pkt_crc( struct outbuf_descr___ *topkt )
{
  uint16_t crc = crc_block( ( uint8_t * )&topkt->buf_ptr[ 2 + HDR_LEN ], topkt->infolen - 2 - HDR_LEN );
  if ( get_free_size( topkt ) < 2 )
    return 1;
  crc = ((crc >> 8) & 0xff) | ((crc << 8) & 0xff00); // bytesex with hi byte first
  *(( uint16_t * )&topkt->buf_ptr[ topkt->infoidx ]) = crc;
  topkt->infolen += 2;
  return 0;
}

void put_chr( uint8_t chr )
{
    //M0=1;M1=0;T485_EN=1;//tx
//  HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_SET);
  /*M1=0;*/
//  HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET);
  /*T485_EN=1;//tx */
  HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET);
  
  HAL_UART_Transmit(&huart4,&chr,1,10);
}

//extern uint32_t PLFull;
//extern uint32_t PL0__;
//extern uint32_t PL;
//extern uint32_t PGFull;
//extern uint32_t PG0__;
//extern uint32_t PG;
//extern struct append_pars
//{
//extern uint32_t PGFull;
//extern uint32_t PGFull;


void putpkt_debug( /*double MORmsb,
                   double PSmsb,
                   double MORlsb,
                   double PSlsb,
                   double CORR,
                   double SIGMA,
                   double PSfull_momentary,
                   double PS0_momentary,
                   double PSmomentary,
                   double MORmomentary*/
                  struct putpars__ * pprs_ptr
                  )
{
  struct outbuf_descr___ *obptr;
  init_obf___();
  obptr = prepare_out_buf();
  if ( NULL != obptr )
  {
    if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PLFULL, (double)pprs_ptr->PLFull/240))
    {
      if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PL0, (double)pprs_ptr->PL0/240))
      {
        if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PL, (double)pprs_ptr->PL))
        {
          if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PGFULL, (double)pprs_ptr->PGFull/240))
          {
            if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PG0, (double)pprs_ptr->PG0/240))
            {
              if (0 == appnd2pkt_dble(obptr, BLK_TYPE_PG, (double)pprs_ptr->PG))
              {
                if(flag_unification)
                {
                  appnd2pkt_dble(obptr, BLK_TYPE_DBL_MOR_MSB, uni_calibration_value.MORmsb);
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS_MSB, uni_calibration_value.PSmsb); 
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_MOR_LSB, uni_calibration_value.MORlsb);
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS_LSB, uni_calibration_value.PSlsb);
                }
                else
                {
                  appnd2pkt_dble(obptr, BLK_TYPE_DBL_MOR_MSB,calibration_value.MORmsb);
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS_MSB, calibration_value.PSmsb); 
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_MOR_LSB, calibration_value.MORlsb);
                  appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS_LSB, calibration_value.PSlsb);
                }                        
                if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_CORR, delta) )
                {
                  if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_SIGMA, pprs_ptr->SIGMA ) )
                  {
                    if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_PSFULL_MOMENT, pprs_ptr->PS ) )
                    {
                      if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS0_MOMENT, pprs_ptr->PS0 ) )
                      {
                        if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_PS_MOMENT, pprs_ptr->PSmomentary ) )
                        {
                          if ( 0 == appnd2pkt_dble( obptr, BLK_TYPE_DBL_MOR_MOMENT, pprs_ptr->MORstacked ) )
                          {
                            set_pkt_crc( obptr );
                            for ( obptr->infoidx = 0; obptr->infoidx < obptr->infolen; obptr->infoidx++ )
                            {
                              put_chr( obptr->buf_ptr[ obptr->infoidx ] );
                            }
                          }                                
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

}

void u4_rcv_pkt_IT( void )
{
  if ( u4rlen == 0xffff )
  {
    __HAL_UART_CLEAR_IDLEFLAG( &huart4 );
    __HAL_UART_ENABLE_IT( &huart4, UART_IT_IDLE );
    memset( ibuf, 0x55, sizeof(ibuf) );
    HAL_UART_Receive_IT( &huart4, ibuf, sizeof(ibuf) );   
  }
  u4rlen = 0xfffe;
}


/* USER CODE END 1 */
