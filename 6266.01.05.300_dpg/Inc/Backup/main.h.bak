/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
  **/

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
  #ifdef __IAR_SYSTEMS_ICC__
//#ifndef __ARMCOMPILER_VERSION
    #include "intrinsics.h"
  #else
    #define __no_operation __nop
  #endif
  #include "typedefs_6266.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define COUNT_MEASURES  9120 //  (5160UL*2)
extern int16_t profile[COUNT_MEASURES];
extern int sending_period;

extern bool aging_LED;
extern bool flag_Kp;


extern bool flag_unification;
extern bool flag_cs;
extern void stop_tim();
//extern int console(double SIGMA, uint8_t c_t_15);

extern bool tim8_off;
extern bool calibration_low;
extern bool calibration_hight;
extern bool calibration_successful;
extern void calibration(uint32_t MORinput, bool reset);
extern void calibration_with_console(uint8_t *b, uint8_t begin, uint8_t end);
extern void unification();


//extern uint32_t PL0x;
//extern uint32_t PL;

//extern uint32_t PG0x;
//extern uint32_t PG;

extern double PS0x;
extern double PLref;
extern double Tref;
extern double termo_val;
//extern double PSx_stack[4];
extern double PS_0_stack[4];
//extern double PSstack[4];
extern double MORstack[4];
extern double vaisala_MORstack[10];
extern void delayed_recording();
extern uint32_t rs_adr;
extern uint32_t version;
extern uint8_t stk_lvlPS;
extern uint8_t stk_lvlMOR;
extern uint8_t vaisala_stk_lvlMOR;
extern double reading_value(uint8_t *read_value, uint32_t memAddr_value, double value);
extern void write_value_in_buf(uint8_t* buf, double write_value);
extern int write123(const char write_name[], double write_value, uint16_t memAddr) ;
extern double read_in_mem(const char name[], double  value, uint16_t memAddr);
extern uint16_t write (uint8_t *, uint32_t , uint32_t );
extern uint8_t read (uint8_t *, uint32_t);

extern int wait_command(uint8_t *console_buffer);
extern void correct_command(uint8_t *console_buffer);
extern void communicator(char text[], uint8_t size);

extern bool flag_console;
extern bool flag_peleng;
extern bool flag_vaisala;
extern bool flag_vaisala_fs;
extern bool PWandFD;
extern uint8_t message;
extern int limit_visibility;
extern bool request;
extern uint8_t ms_adr[4];
extern bool flag_request;
extern int index_minute;
extern int index_hour;
extern bool flag_biral;
extern uint8_t flag_out;
extern bool set_weather;
extern bool flag_temperature_error;
extern void putpkt(uint32_t, uint8_t [], uint8_t );
extern void request_vaisala(uint8_t* ibuf,uint8_t* ms_adr, uint8_t size);
extern void request_peleng(uint8_t* ibuf);
extern void first_putpkt();
extern uint8_t ibuf[256];
extern float Temp;
extern uint8_t devices;
extern uint8_t c_t_15;
extern uint8_t index_phenocode;

//heating
extern bool flag_energy;
extern bool flag_mor_eco;
extern int heatingpresent;
extern bool defrosting;
extern bool flag_heating;
extern float Thigh;
extern float Tlow;
extern int hysteresis;
extern bool run_of_heating(uint8_t * command);
extern void start_heating_according_to_temp(double);
extern void enter_heating(double);
extern void stop_defrosting(bool);
extern  uint16_t   xcoeff;
//extern void EEPROM_Write_ByteBlock(uint8_t *address, uint8_t *data, uint8_t quantity);


//extern void calibration();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define T_Pin GPIO_PIN_13
#define T_GPIO_Port GPIOC
#define SPI1_CLK_Pin GPIO_PIN_5
#define SPI1_CLK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define CS1_Pin GPIO_PIN_7
#define CS1_GPIO_Port GPIOA
#define PD_Pin GPIO_PIN_4
#define PD_GPIO_Port GPIOC
#define K3_Pin GPIO_PIN_5
#define K3_GPIO_Port GPIOC
#define K2_Pin GPIO_PIN_0
#define K2_GPIO_Port GPIOB
#define K1_Pin GPIO_PIN_1
#define K1_GPIO_Port GPIOB
#define CS3_Pin GPIO_PIN_2
#define CS3_GPIO_Port GPIOB
#define CA_Pin GPIO_PIN_10
#define CA_GPIO_Port GPIOB
#define IDIN_Pin GPIO_PIN_11
#define IDIN_GPIO_Port GPIOB
#define IRS_Pin GPIO_PIN_12
#define IRS_GPIO_Port GPIOB
#define CLK2_Pin GPIO_PIN_13
#define CLK2_GPIO_Port GPIOB
#define DATA2CPU_Pin GPIO_PIN_14
#define DATA2CPU_GPIO_Port GPIOB
#define CS2_Pin GPIO_PIN_15
#define CS2_GPIO_Port GPIOB
#define ICLK_Pin GPIO_PIN_6
#define ICLK_GPIO_Port GPIOC
#define ICE_Pin GPIO_PIN_7
#define ICE_GPIO_Port GPIOC
#define BLANK_Pin GPIO_PIN_8
#define BLANK_GPIO_Port GPIOC
#define RSTLED_Pin GPIO_PIN_9
#define RSTLED_GPIO_Port GPIOC
#define WPEE_Pin GPIO_PIN_8
#define WPEE_GPIO_Port GPIOA
#define SCL2_Pin GPIO_PIN_9
#define SCL2_GPIO_Port GPIOA
#define SDA2_Pin GPIO_PIN_10
#define SDA2_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define TXD_Pin GPIO_PIN_10
#define TXD_GPIO_Port GPIOC
#define RXD_Pin GPIO_PIN_11
#define RXD_GPIO_Port GPIOC
#define RDE_Pin GPIO_PIN_4
#define RDE_GPIO_Port GPIOB
#define M0_Pin GPIO_PIN_6
#define M0_GPIO_Port GPIOB
#define M1_Pin GPIO_PIN_7
#define M1_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_9
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
//#define true 1
// #define false 0

//typedef unsigned char bool;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
