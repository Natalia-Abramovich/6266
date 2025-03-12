#ifndef __STM3210E_EEPROM_H
#define __STM3210E_EEPROM_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

#ifdef __IAR_SYSTEMS_ICC__
#include "intrinsics.h"
#endif


#define sEE_I2C                          I2C2
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C2
#define sEE_I2C_SCL_PIN                  GPIO_Pin_10                  /* PB.06 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sEE_I2C_SDA_PIN                  GPIO_Pin_11                  /* PB.07 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB


#ifdef CONF6272120100004

#define sEE_WPROM_PIN                    GPIO_Pin_0
#define sEE_WPROM_GPIO_PORT              GPIOA
#define sEE_WPROM_GPIO_CLK               RCC_APB2Periph_GPIOA

#endif


//#ifdef CONF6272120100005
   #if defined(CONF6272120100005) or defined(CONF6272120100005_RTT)

#define sEE_WPROM_PIN                    GPIO_Pin_1
#define sEE_WPROM_GPIO_PORT              GPIOB
#define sEE_WPROM_GPIO_CLK               RCC_APB2Periph_GPIOB

#endif


#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1

/* Time constant for the delay caclulation allowing to have a millisecond
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 72MHz then sEE_TIME_CONST should be 72. */
#define sEE_TIME_CONST                   72

/* For M24C32 and M24C64 devices, E0,E1 and E2 pins are all used for device
  address selection (ne need for additional address lines). According to the
  Harware connection on the board (on STM3210C-EVAL board E0 = E1 = E2 = 0) */

 #define sEE_HW_ADDRESS         0xA0   /* E0 = E1 = E2 = 0 */


#define sEE_PAGELEN (32)

//#define I2C_SPEED               100000
#define I2C_SLAVE_ADDRESS7      0xA0

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))


/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define sEE_MAX_TRIALS_NUMBER     150

/* Defintions for the state of the DMA transfer */
#define sEE_STATE_READY           0
#define sEE_STATE_BUSY            1
#define sEE_STATE_ERROR           2

#define sEE_OK                    0
#define sEE_FAIL                  1



extern void sEE_Init(void);
extern uint16_t sEE_Read(uint8_t *to_mem_ptr,uint32_t ee_adr,uint32_t ee_len);
extern uint16_t sEE_Write(uint8_t *from_mem_ptr,uint32_t ee_adr,uint32_t ee_len);




#ifdef __cplusplus
}
#endif

#endif /* __STM3210E_EVAL_FSMC_SRAM_H */

