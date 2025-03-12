#ifndef SERIAL_H
#define SERIAL_H

//#include "global.h"



#define   FRAMING_ERROR   (1 << FE0)
#define   PARITY_ERROR    (1 << UPE0)
#define   DATA_OVERRUN    (1 << DOR0)


#define   IB_STATE_IDLE   (0)
#define   IB_STATE_SECOND_SYNC (1)
#define   IB_STATE_FIRST_LEN (2)
#define   IB_STATE_SECOND_LEN (3)
#define   IB_STATE_CONTENT (4)
#define   IB_STATE_FIRST_CHK (5)
#define   IB_STATE_SECOND_CHK (6)
#define   IB_STATE_PKT_RDY (7)



extern volatile uint8_t iidx;
extern volatile bool ibuf_rdy;

//extern uint8_t obuf[17];
extern volatile uint8_t oidx;

extern void putpkt(uint32_t send_val);
extern void init_serial(void);
extern void init_obf(void);

extern void putpkt_debug(double MORmsb,
                         double PSmsb,           
                         double MORlsb,          
                         double PSlsb,           
                         double CORR,            
                         double SIGMA,           
                         double PSfull_momentary,
                         double PS0_momentary,   
                         double PSmomentary,     
                         double MORmomentary     
                        );


#define UCSR1A_UDRE1__ UCSR1A_Bit5
#define UCSR1A_TXC1__ UCSR1A_Bit6
#define UCSR1A_RXC1__ UCSR1A_Bit7
#define UCSR1B_TXCIE1__ UCSR1B_Bit6
#define UCSR1B_UDRIE1__ UCSR1B_Bit5
#define UCSR1B_RXCIE1__ UCSR1B_Bit7
#define UCSR1B_RXEN__   UCSR1B_Bit4

#define OBUF_STATE_FREE (0)
#define OBUF_STATE_PREP (1)
#define OBUF_STATE_IN_PROC (2)


//struct outbuf_descr___
//{
//  struct buf_state___
//  {
//    uint8_t curr_state :2;// 00 buffer is free, !=0 busy, 01 - prepare, 02 - in output process
//  }bs;
//  uint8_t * buf_ptr;
//  u16 infolen;
//  u16 infoidx;
//};

//struct outbuf___
//{
//  struct outbuf_descr___ obd;
//  uint8_t infobuf[192];
//};

#endif   //SERIAL_H
