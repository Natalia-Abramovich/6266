#include "eeprom.h"
#include "i2c.h"
#include "gpio.h"

void I2C2_Read_ByteBlock(uint8_t *address, uint8_t *data, uint8_t quantity);
void I2C2_Read_HalfWord(uint8_t *address, uint8_t *data);
void I2C2_Write_HalfWord(uint8_t *address, uint8_t *data);



uint8_t I2C2_Wait_Bus_Busy(void);
uint8_t I2C2_Wait_Transmit_Data_Register_Not_Empty(void);
uint8_t I2C2_Wait_Transfer_Not_Complete(void);
uint8_t I2C2_Wait_Receive_Data_Register_Empty(void);
uint8_t I2C2_Wait_Stop_Not_Detection(void);

void EEPROM_Write_HalfWord(uint32_t address, uint16_t data);
void EEPROM_Read_HalfWord(uint32_t address, uint16_t *data);
 
#define US_OK 1;
#define US_ERROR  0;

/*************************************************************************************************/

#define I2C2_OFF                                (I2C2->CR1 &= ~(1<<0))
#define I2C2_ON                                 (I2C2->CR1 |= (1<<0))

#define I2C2_PEC_T_R_Requested                  ((I2C2->CR2 & 0x04000000) == 0x04000000)
#define I2C2_PEC_T_R_NOT_Requested              ((I2C2->CR2 & 0x04000000) == 0x00000000)

#define I2C2_NACK_GENERATION                    (I2C2->CR2 |= (1<<15)) // NACK is sent after current received byte
#define I2C2_ACK_GENERATION                     (I2C2->CR2 &= ~(1<<15)) // ACK is sent after current received byte

#define I2C2_STOP_GENERATION                    (I2C2->CR2 |= (1<<14)) // STOP generation after current byte transfer
#define I2C2_NO_STOP_GENERATION                 (I2C2->CR2 &= ~(1<<14)) // NO STOP generation after current byte transfer

#define I2C2_START_GENERATION                   (I2C2->CR2 |= (1<<13)) // Restart/Start generation after current byte transfer
#define I2C2_NO_START_GENERATION                (I2C2->CR2 &= ~(1<<13)) // No Restart/Start generation after current byte transfer

#define I2C2_WR_TRANSFER                        (I2C2->CR2 &= ~(1<<10))
#define I2C2_RD_TRANSFER                        (I2C2->CR2 |= (1<<10))

#define I2C2_Number_Bytes_Clear                 (I2C2->CR2 &= ~(0xFF<<16))
#define I2C2_Slave_Address_Clear                (I2C2->CR2 &= ~(0x3FF<<0))

#define I2C2_Address_Match_Code                 ((I2C2->ISR & 0x00FE0000) == 0x00FE0000)
#define I2C2_Transfer_Direction_W               ((I2C2->ISR & 0x00010000) == 0x00000000)
#define I2C2_Transfer_Direction_R               ((I2C2->ISR & 0x00010000) == 0x00010000)
#define I2C2_Bus_Busy                           ((I2C2->ISR & 0x00008000) == 0x00008000)
#define I2C2_Bus_Not_Busy                       ((I2C2->ISR & 0x00008000) == 0x00000000)
#define I2C2_SMBus_Alert                        ((I2C2->ISR & 0x00002000) == 0x00002000)
#define I2C2_Timeout_Tlow_Detection             ((I2C2->ISR & 0x00001000) == 0x00001000)
#define I2C2_PEC_Error_In_Reception             ((I2C2->ISR & 0x00000800) == 0x00000800)
#define I2C2_Overrun_Underrun_Error_Occurs      ((I2C2->ISR & 0x00000400) == 0x00000400)
#define I2C2_Arbitration_Lost                   ((I2C2->ISR & 0x00000200) == 0x00000200)
#define I2C2_Bus_Error                          ((I2C2->ISR & 0x00000100) == 0x00000100)

#define I2C2_Transfer_Complete_Reload           ((I2C2->ISR & 0x00000080) == 0x00000080)
#define I2C2_Transfer_Not_Complete_Reload       ((I2C2->ISR & 0x00000080) == 0x00000000)

#define I2C2_Transfer_Complete                  ((I2C2->ISR & 0x00000040) == 0x00000040)
#define I2C2_Transfer_Not_Complete              ((I2C2->ISR & 0x00000040) == 0x00000000)

#define I2C2_Stop_Detection                     ((I2C2->ISR & 0x00000020) == 0x00000020)
#define I2C2_Stop_Not_Detection                 ((I2C2->ISR & 0x00000020) == 0x00000000)

#define I2C2_Not_Acknowledge_Received           ((I2C2->ISR & 0x00000010) == 0x00000010)
#define I2C2_Address_Matched                    ((I2C2->ISR & 0x00000008) == 0x00000008)

#define I2C2_Receive_Data_Register_Not_Empty    ((I2C2->ISR & 0x00000004) == 0x00000004)
#define I2C2_Receive_Data_Register_Empty        ((I2C2->ISR & 0x00000004) == 0x00000000)
#define I2C2_Transmit_Interrupt_Status_Set      ((I2C2->ISR & 0x00000002) == 0x00000002)
#define I2C2_Transmit_Interrupt_Status_Not_Set  ((I2C2->ISR & 0x00000002) == 0x00000000)
#define I2C2_Transmit_Interrupt_Generate_Event  (I2C2->ISR |= (1<<1))
#define I2C2_Transmit_Data_Register_Empty       ((I2C2->ISR & 0x00000001) == 0x00000001)
#define I2C2_Transmit_Data_Register_Not_Empty   ((I2C2->ISR & 0x00000001) == 0x00000000)
#define I2C2_Transmit_Data_Register_Flush       (I2C2->ISR |= (1<<0))

#define I2C2_Alert_Flag_Clear                   (I2C2->ICR |= (1<<13))
#define I2C2_Timeout_Detection_Flag_Clear       (I2C2->ICR |= (1<<12))
#define I2C2_PEC_Error_Flag_Clear               (I2C2->ICR |= (1<<11))
#define I2C2_Overrun_Underrun_Flag_Clear        (I2C2->ICR |= (1<<10))
#define I2C2_Arbitration_Lost_Flag_Clear        (I2C2->ICR |= (1<<9))
#define I2C2_Bus_Error_Flag_Clear               (I2C2->ICR |= (1<<8))
#define I2C2_Stop_Detection_Flag_Clear          (I2C2->ICR |= (1<<5))
#define I2C2_Not_Acknowledge_Flag_Clear         (I2C2->ICR |= (1<<4))
#define I2C2_Address_Matched_Flag_Clear         (I2C2->ICR |= (1<<3))

/*************************************************************************************************/


//void sEE_LowLevel_DeInit(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
////  NVIC_InitTypeDef NVIC_InitStructure;
////  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
//
//  /* sEE_I2C Peripheral Disable */
//  I2C_Cmd(sEE_I2C, DISABLE);
//
//  /* sEE_I2C DeInit */
//  I2C_DeInit(sEE_I2C);
//
//  /*!< sEE_I2C Periph clock disable */
//  RCC_APB1PeriphClockCmd(sEE_I2C_CLK, DISABLE);
//
//  /*!< GPIO configuration */
//  /*!< Configure sEE_I2C pins: SCL */
//  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< Configure sEE_I2C pins: SDA */
//  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
//  GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
//
//  /* Configure and enable I2C DMA TX Channel interrupt */
////  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
////  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
////  NVIC_Init(&NVIC_InitStructure);
//
//  /* Configure and enable I2C DMA RX Channel interrupt */
////  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
////  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
////  NVIC_Init(&NVIC_InitStructure);
//
//  /* Disable and Deinitialize the DMA channels */
////  DMA_Cmd(sEE_I2C_DMA_CHANNEL_TX, DISABLE);
////  DMA_Cmd(sEE_I2C_DMA_CHANNEL_RX, DISABLE);
////  DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
////  DMA_DeInit(sEE_I2C_DMA_CHANNEL_RX);
//}
//

void sEE_DeInit(void)
{
  //sEE_LowLevel_DeInit();
}


/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
//void sEE_LowLevel_Init(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
////  NVIC_InitTypeDef NVIC_InitStructure;
//
//  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
//  RCC_APB2PeriphClockCmd(sEE_I2C_SCL_GPIO_CLK | sEE_I2C_SDA_GPIO_CLK, ENABLE);
//
//  /*!< sEE_I2C Periph clock enable */
//  RCC_APB1PeriphClockCmd(sEE_I2C_CLK, ENABLE);
//
//  /*!< GPIO configuration */
//  /*!< Configure sEE_I2C pins: SCL */
//  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//  GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
//
//  /*!< Configure sEE_I2C pins: SDA */
//  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
//  GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
//
//  RCC_APB2PeriphClockCmd(sEE_WPROM_GPIO_CLK, ENABLE);
//
//  GPIO_InitStructure.GPIO_Pin = sEE_WPROM_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(sEE_WPROM_GPIO_PORT, &GPIO_InitStructure);
//  GPIO_ResetBits(sEE_WPROM_GPIO_PORT,sEE_WPROM_PIN);
//
//
//  /* Configure and enable I2C DMA TX Channel interrupt */
////  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
////  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////  NVIC_Init(&NVIC_InitStructure);
//
//  /* Configure and enable I2C DMA RX Channel interrupt */
////  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
////  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
////  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
////  NVIC_Init(&NVIC_InitStructure);
//
//  /*!< I2C DMA TX and RX channels configuration */
//  /* Enable the DMA clock */
////  RCC_AHBPeriphClockCmd(sEE_I2C_DMA_CLK, ENABLE);
//
//  /* I2C TX DMA Channel configuration */
////  DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
////  sEEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sEE_I2C_DR_Address;
////  sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
////  sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
////  sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
////  sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
////  sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
////  sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
////  sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
////  sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
////  sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
////  sEEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
////  DMA_Init(sEE_I2C_DMA_CHANNEL_TX, &sEEDMA_InitStructure);
//
//  /* I2C RX DMA Channel configuration */
////  DMA_DeInit(sEE_I2C_DMA_CHANNEL_RX);
////  DMA_Init(sEE_I2C_DMA_CHANNEL_RX, &sEEDMA_InitStructure);
//
//  /* Enable the DMA Channels Interrupts */
////  DMA_ITConfig(sEE_I2C_DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
////  DMA_ITConfig(sEE_I2C_DMA_CHANNEL_RX, DMA_IT_TC, ENABLE);
//}
//



/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_Init(void)
{
//I2C_InitTypeDef  I2C_InitStructure;
//
//sEE_LowLevel_Init();
//
///*!< I2C configuration */
///* sEE_I2C configuration */
//I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
//I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
//
///* sEE_I2C Peripheral Enable */
//I2C_Cmd(sEE_I2C, ENABLE);
///* Apply sEE_I2C configuration after enabling it */
//I2C_Init(sEE_I2C, &I2C_InitStructure);
//
///* Enable the sEE_I2C peripheral DMA requests */
//  I2C_DMACmd(sEE_I2C, ENABLE);

  /*!< Select the EEPROM address according to the state of E0, E1, E2 pins */
//  sEEAddress = sEE_HW_ADDRESS;
}


uint16_t sEE_Read( uint8_t *to_mem_ptr, uint32_t ee_adr, uint32_t ee_len )
{
  HAL_StatusTypeDef hstd = HAL_I2C_Mem_Read( &hi2c2, sEE_HW_ADDRESS, ee_adr, I2C_MEMADD_SIZE_16BIT, to_mem_ptr, ee_len, 100 );
//  uint32_t sEETimeout;
//  //sEEDataReadPointer = NumByteToRead;
//
//  /*!< While the bus is busy */
//  sEETimeout = sEE_LONG_TIMEOUT;
//  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
//  {
//    if ((sEETimeout--) == 0) return 0x100;
//  }
//
//  /*!< Send START condition */
//  I2C_GenerateSTART(sEE_I2C, ENABLE);
//
//  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//  {
//    if ((sEETimeout--) == 0)
//      return 0x200;
//  }
//
//  /*!< Send EEPROM address for write */
//  I2C_Send7bitAddress(sEE_I2C, sEE_HW_ADDRESS, I2C_Direction_Transmitter);
//
//  /*!< Test on EV6 and clear it */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//  {
//    if ((sEETimeout--) == 0)
//      return 0x400;
//  }
//
//
//  /*!< Send the EEPROM's internal address to read from: MSB of the address first */
//  I2C_SendData(sEE_I2C, (uint8_t)((ee_adr & 0xFF00) >> 8));
//
//  /*!< Test on EV8 and clear it */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
//  {
//    if ((sEETimeout--) == 0)
//      return 0x800;
//  }
//
//  /*!< Send the EEPROM's internal address to read from: LSB of the address */
//  I2C_SendData(sEE_I2C, (uint8_t)(ee_adr & 0x00FF));
//
//
//  /*!< Test on EV8 and clear it */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
//  //    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) == RESET)
//  {
//    if ((sEETimeout--) == 0)
//      return 0x1000;
//  }
//
//  /*!< Send STRAT condition a second time */
//  I2C_GenerateSTART(sEE_I2C, ENABLE);
//
//  /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//  {
//    if ((sEETimeout--) == 0)
//      return 0x2000;
//  }
//
//  /*!< Send EEPROM address for read */
//  I2C_Send7bitAddress(sEE_I2C, sEE_HW_ADDRESS, I2C_Direction_Receiver);
//
//  /* If number of data to be read is 1, then DMA couldn't be used */
//  /* One Byte Master Reception procedure (POLLING) ---------------------------*/
//  //  if ((uint16_t)(*NumByteToRead) < 2)
//  //  {
//  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
//  sEETimeout = sEE_FLAG_TIMEOUT;
////    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_ADDR) == RESET)
//  while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
//  {
//    if ((sEETimeout--) == 0)
//      return 0x4000;
//  }
//  do
//  {
//    if (ee_len==1)
//    {
//      /*!< Disable Acknowledgement */
//      I2C_AcknowledgeConfig(sEE_I2C, DISABLE);
//    }
//    else
//    {
//      /*!< Re-Enable Acknowledgement to be ready for another reception */
//      I2C_AcknowledgeConfig(sEE_I2C, ENABLE);
//    }
//
//    /* Call User callback for critical section start (should typically disable interrupts) */
//    //    sEE_EnterCriticalSection_UserCallback();
//    /* Wait for the byte to be received */
//    sEETimeout = sEE_FLAG_TIMEOUT;
//    //      while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_RXNE) == RESET)
//    while (SUCCESS!=I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
//    {
//      if ((sEETimeout--) == 0)
//        return 0x8000;
//    }
//
//    /*!< Read the byte received from the EEPROM */
//    *(to_mem_ptr++) = I2C_ReceiveData(sEE_I2C);
//
//    /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
//    (void)sEE_I2C->SR2;
//    ee_adr++;
//    ee_len--;
//  } while (ee_len !=0);
//
//  /*!< Send STOP Condition */
//  I2C_GenerateSTOP(sEE_I2C, ENABLE);
//
//  /* Call User callback for critical section end (should typically re-enable interrupts) */
//  //sEE_ExitCriticalSection_UserCallback();
//
//
//
//  /* Wait to make sure that STOP control bit has been cleared */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while (sEE_I2C->CR1 & I2C_CR1_STOP)
//  {
//    if ((sEETimeout--) == 0)
//      return 0x8001;
//  }
//
//  I2C_AcknowledgeConfig(sEE_I2C, ENABLE);
//
  if ( hstd == HAL_OK )
  {
    return true;
  }
  else
    return false;
}

/*
void I2C_EE_WaitEepromStandbyState(void)
{
  static __IO uint32_t SR1_Tmp = 0;
  do
  {
    I2C_ClearFlag(sEE_I2C,I2C_FLAG_ADDR|I2C_FLAG_AF);
    // Send START condition
    I2C_GenerateSTART(sEE_I2C, ENABLE);

    // Read I2C_EE SR1 register to clear pending flags
    //    SR1_Tmp = I2C_ReadRegister(sEE_I2C, I2C_Register_SR1);
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    // Send EEPROM address for write
    I2C_Send7bitAddress(sEE_I2C, sEE_HW_ADDRESS, I2C_Direction_Transmitter);
    //    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    do
    {
      SR1_Tmp = I2C_GetLastEvent(sEE_I2C);
    }while//(!(I2C_ReadRegister(sEE_I2C, I2C_Register_SR1) & 0x0002));
       ((SR1_Tmp & (I2C_FLAG_ADDR|I2C_FLAG_AF))==0);
    __no_operation();
  } while ((SR1_Tmp & I2C_FLAG_AF)!=0);
  // Clear AF flag
//  I2C_ClearFlag(sEE_I2C, I2C_FLAG_AF);

  // STOP condition
  I2C_GenerateSTOP(sEE_I2C, ENABLE);
}

*/
int affl;
uint16_t sEE_Write( uint8_t *from_mem_ptr, uint32_t ee_adr, uint32_t ee_len )
{
  //uint32_t sEETimeout;
  HAL_StatusTypeDef hstd;
  uint16_t          ee_curlen;
  if ( HAL_GPIO_ReadPin( WPEE_GPIO_Port, WPEE_Pin ) != GPIO_PIN_RESET )
    MX_GPIO_Init();
  if ( hi2c2.State != HAL_I2C_STATE_READY )
    MX_I2C2_Init();

 
  hstd = HAL_I2C_IsDeviceReady( &hi2c2, sEE_HW_ADDRESS, 30, 16 );
  if ( hstd != HAL_OK )
  {
    while(1)
    {
      hstd = HAL_I2C_IsDeviceReady( &hi2c2, sEE_HW_ADDRESS, 30, 16 );
      if(HAL_OK==hstd)
        break;
       else
         affl++;
    }
 //   return false;
  }
  do
  {
  ee_curlen = sEE_PAGELEN - (ee_adr % sEE_PAGELEN);
  if ( ee_curlen > ee_len )
  {
    ee_curlen = ee_len;
  }
    if(ee_curlen > 32)
      __no_operation();
    if(ee_adr > 256)
      __no_operation();
    hstd = HAL_I2C_Mem_Write( &hi2c2, sEE_HW_ADDRESS, ee_adr, I2C_MEMADD_SIZE_16BIT, from_mem_ptr, ee_curlen, 100 );
    if ( HAL_OK != hstd )
    {
      __no_operation();
      if ( hi2c2.ErrorCode == HAL_I2C_ERROR_AF )
      {
        affl++;
        continue;
      }
      else
        break;
    }
    if ( hstd == HAL_OK )
    {
      from_mem_ptr += ee_curlen;
      ee_adr += ee_curlen;
    if ( ee_curlen > ee_len )
    {
      __no_operation();//ee_curlen = ee_len;
    }
      ee_curlen=ee_len -= ee_curlen;
      //     HAL_Delay( 1 );
    }
    else
      return hstd;
  }
  while ( ee_len != 0 );
  if ( hstd == HAL_OK )
  {
    return true;
  }
  else
    return false;
}
