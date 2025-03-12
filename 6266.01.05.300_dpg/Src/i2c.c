/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "lib.h"
#include <string.h>
#include <math.h>
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2C;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00101D2D;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = SCL_Pin|SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
  else if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PA9     ------> I2C2_SCL
    PA10     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = SCL2_Pin|SDA2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(SCL_GPIO_Port, SCL_Pin);

    HAL_GPIO_DeInit(SDA_GPIO_Port, SDA_Pin);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
  else if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PA9     ------> I2C2_SCL
    PA10     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(SCL2_GPIO_Port, SCL2_Pin);

    HAL_GPIO_DeInit(SDA2_GPIO_Port, SDA2_Pin);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

uint8_t termo_exchange( uint8_t command,
                   uint8_t *send_data_ptr,
                   uint8_t send_data_len,
                   uint8_t *rcvd_data_ptr,
                   uint8_t rcvd_data_len )
{
  HAL_StatusTypeDef hstd0=HAL_OK,hstd1=HAL_OK,hstd2=HAL_OK;
  uint8_t ostr [16];
  ostr[ 0 ] = command;
  //hstd0 = HAL_I2C_Master_Transmit( &hi2c1, TERMO_ADRS, &command, 1, 20 );
  if ( send_data_ptr != NULL )
  {
    memcpy( &ostr[ 1 ], send_data_ptr, send_data_len );
  }
  hstd0 = HAL_I2C_Master_Transmit( &hi2c1, TERMO_ADRS, ostr, 1 + send_data_len, 30 );//20
  if ( hstd0 == HAL_OK )
  {
    if(hstd0==HAL_OK)
    {
      if ( NULL != rcvd_data_ptr )
      {
        hstd1 = HAL_I2C_Master_Receive( &hi2c1, TERMO_ADRS, rcvd_data_ptr, rcvd_data_len, 30 );//20
      }
    }
  }
  if(hstd0 == HAL_ERROR ||hstd1 == HAL_ERROR) flag_error.temperature_inside=1;
  return ( hstd1 | hstd2 );
}


  union rv_
  {
    uint8_t retbytes[2];
    int16_t retval;
  }rv;
int16_t get_termoval(void)
{
  uint16_t data_a9;
  uint16_t data_a8;
  uint8_t tmpb;
  uint16_t data;
  tmpb=termo_exchange(0xA9,NULL,0,&rv.retbytes[0],1);
  data_a9= rv.retbytes[0];
  tmpb=termo_exchange(0xA8,NULL,0,&rv.retbytes[0],1);
  data_a8= rv.retbytes[0];/**/
  tmpb=termo_exchange(TERMO_READ_TEMP_CMD,NULL,0,&rv.retbytes[0],2);
  if(tmpb != 0)
  {
    flag_error.temperature_inside=1;
    return -32000 | tmpb;
  }
  else
  {
    tmpb=rv.retbytes[0];
    rv.retbytes[0]=rv.retbytes[1];
    rv.retbytes[1]=tmpb;
    return ((rv.retval*5)/128);

/*
    if(tmpb & 0x80)
    {
      tmpb =( ~tmpb + 1);     
      return 0 - tmpb;
    }
    return tmpb; */
  }
}

uint8_t termo_init(void)
{
  uint8_t retval,outval=0;
  //init_i2c();
  retval=termo_exchange(TERMO_ACC_CONFIG_CMD,&outval,1,NULL,0);
  if(retval!=0)
    return retval;
  //delay_ms(10);
  HAL_Delay( 10 );
  retval=termo_exchange(TERMO_START_CONVERT_CMD,NULL,0,NULL,0);
  return retval;
}


uint8_t termo_rdy( void )
{
  uint8_t retval,statreg;
  retval = termo_exchange( TERMO_ACC_CONFIG_CMD, NULL, 0, &statreg, 1 );
  if ( retval==0 )
  {
    return statreg;
  }
  else return 0xff;
}
//                   0      1    2      3    4      5    6      7    8      9    10    11    12
static const float mas_TK[13] = {0.85, 0.88, 0.91, 0.97, 1.00, 0.99, 0.98, 0.97, 0.94, 0.93, 0.91, 0.88, 0.86};
static const int16_t bounds[14] = {-550, -50, 0, 50, 100, 250, 300, 350, 400, 450, 500, 550, 600, 650};


/* USER CODE END 1 */
