/* Includes */
#include "main.h"
#define US_OK 1;
#define US_ERROR  0;
/*
In automatic end mode (AUTOEND=1), a NACK and a STOP are automatically sent after the last received byte.

In software end mode (AUTOEND=0), a NACK is automatically sent after the last received byte,
the TC flag is set and the SCL line is stretched low in order to allow software actions: RESTART or STOP condition.

WR and RD command bit in start address byte set by commands (I2C1_WR_TRANSFER and I2C1_RD_TRANSFER).
*/

/* External variables */




/*************************************************************************************************/

/* Function I2C1 36MHz EEPROM and TEMPERATURE IC config */
void I2C1_Config(void)
{
	RCC->APB1ENR |= (1<<21);         // I2C1 clock enable

//-----------------------------------------------------------------------------------------------//

	I2C1->CR1 &= ~(1<<23);           // I2C1 PEC calculation disabled
	I2C1->CR1 &= ~(1<<19);           // I2C1 general call disabled, address 0b00000000 is NACKed
	I2C1->CR1 &= ~(1<<18);           // I2C1 wakeup from stop mode disable
	I2C1->CR1 |= (1<<17);            // I2C1 clock stretching disabled
	I2C1->CR1 &= ~(1<<16);           // I2C1 slave byte control disabled
	I2C1->CR1 &= ~(1<<15);           // I2C1 DMA mode disabled for reception
	I2C1->CR1 &= ~(1<<14);           // I2C1 DMA mode disabled for transmission
	I2C1->CR1 &= ~(1<<12);           // I2C1 analog noise filter enabled
	I2C1->CR1 &= ~(0xF<<8);          // I2C1 digital filter disabled
	I2C1->CR1 &= ~(1<<7);            // I2C1 error detection interrupts disabled
	I2C1->CR1 &= ~(1<<6);            // I2C1 transfer complete interrupt disabled
	I2C1->CR1 &= ~(1<<5);            // I2C1 stop detection (STOPF) interrupt disabled
	I2C1->CR1 &= ~(1<<4);            // I2C1 not acknowledge (NACKF) received interrupts disabled
	I2C1->CR1 &= ~(1<<3);            // I2C1 address match (ADDR) interrupts disabled
	I2C1->CR1 &= ~(1<<2);            // I2C1 receive (RXNE) interrupt disabled
	I2C1->CR1 &= ~(1<<1);            // I2C1 transmit (TXIS) interrupt disabled

//-----------------------------------------------------------------------------------------------//

	I2C1->CR2 &= ~(1<<25);           // I2C1 automatic end mode (master mode) disabled

	I2C1->CR2 &= ~(1<<24);           // I2C1 transfer is completed after the NBYTES data transfer
	//I2C1->CR2 |= (1<<24);            // I2C1 transfer is not completed after the NBYTES data transfer

	I2C1->CR2 &= ~(0xFF<<16);        // I2C1 number of bytes to be transmitted/received clear
	//I2C1->CR2 |= (1<<16);            // I2C1 number of bytes to be transmitted/received set

	I2C1->CR2 |= (1<<12);            // I2C1 master sends the complete 10 bit slave address read sequence
	I2C1->CR2 &= ~(1<<11);           // I2C1 master operates in 7-bit addressing mode

	I2C1->CR2 &= ~(1<<10);           // I2C1 master requests a write transfer
	//I2C1->CR2 |= (1<<10);            // I2C1 master requests a read transfer

	I2C1->CR2 &= ~(3<<8);            // I2C1 slave address bit 9:8 in 10-bit addressing mode

	I2C1->CR2 &= ~(0x7F<<1);         // I2C1 slave address bit 7:1 in 7-bit addressing mode clear
	//I2C1->CR2 |= (0x49<<1);          // I2C1 slave address DS1621 set

	I2C1->CR2 &= ~(1<<0);            // I2C1 slave address bit 0 in 10-bit addressing mode

//-----------------------------------------------------------------------------------------------//

	I2C1->OAR1 &= ~(1<<15);          // I2C1 own address 1 disabled
	I2C1->OAR1 &= ~(1<<10);          // I2C1 own address 1 is a 7-bit address
	I2C1->OAR1 &= ~(3<<8);           // I2C1 10-bit addressing mode: bits 9:8 of address
	I2C1->OAR1 &= ~(0x7F<<1);        // I2C1 7-bit addressing mode: 7-bit address
	I2C1->OAR1 &= ~(1<<0);           // I2C1 10-bit addressing mode: bit 0 of address

//-----------------------------------------------------------------------------------------------//

	I2C1->OAR2 &= ~(1<<15);          // I2C1 own address 2 disabled
	I2C1->OAR2 &= ~(7<<8);           // I2C1 own address 2 masks, no mask
	I2C1->OAR2 &= ~(0x7F<<1);        // I2C1 7-bit addressing mode: 7-bit address

//-----------------------------------------------------------------------------------------------//

	I2C1->TIMINGR = 0x2000090E;      // 100kHz

//-----------------------------------------------------------------------------------------------//

	I2C1->TIMEOUTR &= ~(1u<<31);     // extended clock timeout detection is disabled
	I2C1->TIMEOUTR &= ~(0xFFF<<16);  // bus timeout B, tLOW:EXT= (TIMEOUTB+1) x 2048 x tI2CCLK
	I2C1->TIMEOUTR &= ~(1<<15);      // SCL timeout detection is disabled
	I2C1->TIMEOUTR &= ~(1<<12);      // TIMEOUTA is used to detect SCL low timeout
	I2C1->TIMEOUTR &= ~(0xFFF<<0);   // bus timeout A, tTIMEOUT= (TIMEOUTA+1) x 2048 x tI2CCLK, tIDLE= (TIMEOUTA+1) x 4 x tI2CCLK

//-----------------------------------------------------------------------------------------------//

	I2C1->CR1 |= (1<<0);             // I2C1 On

//-----------------------------------------------------------------------------------------------//

	//HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
	//HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	//HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
	//HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

/*************************************************************************************************/



/********************* US 12.09.2022 ************************ END OF FILE ************************/
