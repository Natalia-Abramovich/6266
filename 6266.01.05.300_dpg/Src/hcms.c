#include "hcms.h"

//#ifdef CONF6272120100005
//#if defined(CONF6272120100005) or defined(CONF6272120100005_RTT)

#define CONTROL_WORD0 (0x4f)
#define CONTROL_WORD1 (0x80)

/*
#define HCMS_CE (PORTB_PORTB6)     pg8
#define HCMS_BLANK (PORTB_PORTB7)  pc6
#define HCMS_DIN (PORTL_PORTL3)    pc7
#define HCMS_RST (PORTL_PORTL4)    pc8
#define HCMS_RS  (PORTL_PORTL5)    pc9 //control == 1, dot = 0
#define HCMS_CLK (PORTL_PORTL6)    pa8
*/

#define HCMS_CE_Pin   ICE_Pin
#define HCMS_CE_GPIO_Port  ICE_GPIO_Port
//
#define HCMS_BLANK_Pin  BLANK_Pin
#define HCMS_BLANK_GPIO_Port BLANK_GPIO_Port
//
#define HCMS_DIN_Pin    IDIN_Pin
#define HCMS_DIN_GPIO_Port   IDIN_GPIO_Port
//
#define HCMS_RS_Pin    IRS_Pin
#define HCMS_RS_GPIO_Port   IRS_GPIO_Port
//
#define HCMS_RST_Pin     RSTLED_Pin
#define HCMS_RST_GPIO_Port    RSTLED_GPIO_Port
//
#define HCMS_CLK_Pin    ICLK_Pin
#define HCMS_CLK_GPIO_Port   ICLK_GPIO_Port




uint8_t hcms_buf[40];

void putword(uint8_t wrd)
{
  uint8_t mask = 0x80;
//  HCMS_RS=0;
  //GPIO_ResetBits(HCMS_RS_Port,HCMS_RS_Pin);
  HAL_GPIO_WritePin( HCMS_RS_GPIO_Port, HCMS_RS_Pin, GPIO_PIN_RESET );
  //HCMS_CE=0;
//  GPIO_ResetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_RESET );

  while (mask != 0)
  {
    if((wrd & mask)!=0)
    {
      //HCMS_DIN=1;
//      GPIO_SetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
      HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_SET );

    }
    else
      //HCMS_DIN=0;
//      GPIO_ResetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
      HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_RESET );

    //HCMS_CLK=0;
//    GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
    HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );

    //HCMS_CLK=1;
//    GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
    HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );

    mask >>= 1;
  }
  //HCMS_CE=1;
//  GPIO_SetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_SET );

  //HCMS_CLK=0;
//  GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );

  //HCMS_CLK=1;
//  GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );
}

void put_cw(uint8_t cw)
{
  uint8_t mask = 0x80;
  //HCMS_RS=1;
//  GPIO_SetBits(HCMS_RS_Port,HCMS_RS_Pin);
  HAL_GPIO_WritePin( HCMS_RS_GPIO_Port, HCMS_RS_Pin, GPIO_PIN_SET );

  //HCMS_CE=0;
//  GPIO_ResetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_RESET );

  while (mask != 0)
  {
    if((cw & mask)!=0)
    {
      //HCMS_DIN=1;
//      GPIO_SetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
      HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_SET );

    }
    else
      //HCMS_DIN=0;
//      GPIO_ResetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
      HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_RESET );
    //HCMS_CLK=0;
//    GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
    HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );

    //HCMS_CLK=1;
//    GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
    HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );

    mask >>= 1;
  }
  //HCMS_CE=1;
  //GPIO_SetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_SET );
  //HCMS_CLK=0;
//  GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );
  //HCMS_CLK=1;
//  GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );

}


void hcms_init(void)
{
//  DDRB_DDB6=DDRB_DDB7=DDRL_DDL3= DDRL_DDL4 = DDRL_DDL5=DDRL_DDL6=1;
{
/*
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOG, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
*/
}

/*
   while(1)
   {
     HCMS_CE=~HCMS_CE;
     HCMS_RS=~HCMS_RS;
     HCMS_RST=~HCMS_RST;// reset\ active
     HCMS_BLANK=~HCMS_BLANK;//on
     HCMS_CLK=~HCMS_CLK;
     HCMS_DIN=~HCMS_DIN;
   }
*/
  //HCMS_CE=1;
  //GPIO_SetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_SET );
  //HCMS_RS=0;
  //GPIO_ResetBits(HCMS_RS_Port,HCMS_RS_Pin);
  HAL_GPIO_WritePin( HCMS_RS_GPIO_Port, HCMS_RS_Pin, GPIO_PIN_RESET );
  //HCMS_RST=0;// reset\ active
//  GPIO_ResetBits(HCMS_RST_Port,HCMS_RST_Pin);
  HAL_GPIO_WritePin( HCMS_RST_GPIO_Port, HCMS_RST_Pin, GPIO_PIN_RESET );
  //HCMS_RST=1;// reset\ off
//  GPIO_SetBits(HCMS_RST_Port,HCMS_RST_Pin);
  HAL_GPIO_WritePin( HCMS_RST_GPIO_Port, HCMS_RST_Pin, GPIO_PIN_SET );
  //HCMS_BLANK=1;//display off
//  GPIO_SetBits(HCMS_BLANK_Port,HCMS_BLANK_Pin);
  HAL_GPIO_WritePin( HCMS_BLANK_GPIO_Port, HCMS_BLANK_Pin, GPIO_PIN_SET );
  //HCMS_BLANK=0;//display on
 // GPIO_ResetBits(HCMS_BLANK_Port,HCMS_BLANK_Pin);
  HAL_GPIO_WritePin( HCMS_BLANK_GPIO_Port, HCMS_BLANK_Pin, GPIO_PIN_RESET );
  //HCMS_CLK=1;
 // GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );
  put_cw(CONTROL_WORD0);
  put_cw(CONTROL_WORD1);
  putword(1);
  putword(3);
  putword(7);
  putword(0xf);
  putword(0x1f);
  //HCMS_CLK=0;
//  GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );
  //HCMS_CLK=1;
//  GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );
}




void putwords(uint8_t *buf,uint8_t len)
{
  //HCMS_RS=0;
//  GPIO_ResetBits(HCMS_RS_Port,HCMS_RS_Pin);
  HAL_GPIO_WritePin( HCMS_RS_GPIO_Port, HCMS_RS_Pin, GPIO_PIN_RESET );
  //HCMS_CE=0;
//  GPIO_ResetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_RESET );
  while (len)
  {
    uint8_t mask=0x80;
    while (mask != 0)
    {
      if((*buf & mask)!=0)
      {
        //HCMS_DIN=1;
//        GPIO_SetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
        HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_SET );
      }
      else
//        HCMS_DIN=0;
//        GPIO_ResetBits(HCMS_DIN_Port,HCMS_DIN_Pin);
        HAL_GPIO_WritePin( HCMS_DIN_GPIO_Port, HCMS_DIN_Pin, GPIO_PIN_RESET );
      //HCMS_CLK=0;
//      GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
      HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );
      //HCMS_CLK=1;
//      GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
      HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );
      mask >>= 1;

    }
    buf++;
    len--;
  }
  //HCMS_CE=1;
//  GPIO_SetBits(HCMS_CE_Port,HCMS_CE_Pin);
  HAL_GPIO_WritePin( HCMS_CE_GPIO_Port, HCMS_CE_Pin, GPIO_PIN_SET );
  //HCMS_CLK=0;
//  GPIO_ResetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_RESET );
  //HCMS_CLK=1;
//  GPIO_SetBits(HCMS_CLK_Port,HCMS_CLK_Pin);
  HAL_GPIO_WritePin( HCMS_CLK_GPIO_Port, HCMS_CLK_Pin, GPIO_PIN_SET );
}




void hcms_putbuf(void)
{
  //HCMS_BLANK=1;//display off
//  GPIO_SetBits(HCMS_BLANK_Port,HCMS_BLANK_Pin);
  HAL_GPIO_WritePin( HCMS_BLANK_GPIO_Port, HCMS_BLANK_Pin, GPIO_PIN_SET );
  putwords(hcms_buf,sizeof(hcms_buf));
  //HCMS_BLANK=0;//display off
//  GPIO_ResetBits(HCMS_BLANK_Port,HCMS_BLANK_Pin);
  HAL_GPIO_WritePin( HCMS_BLANK_GPIO_Port, HCMS_BLANK_Pin, GPIO_PIN_RESET );
}

//void hcms_clear(void)
//{
//  memset(hcms_buf,0,sizeof(hcms_buf));
//  hcms_putbuf();
//}
void hcms_clear( void )
{
  memset( hcms_buf, 0, sizeof(hcms_buf) );
  hcms_putbuf();
}


uint8_t reverse_byte(uint8_t byt)
{
  uint8_t mask = 0x80,mask1=1,rslt=0;
  while(mask!=0)
  {
    if((mask & byt)!=0)
    {
      rslt |= mask1;
    }
    mask >>= 1;
    mask1 <<= 1;
  }
  return rslt;
}

void hcms_putchars(char *cptr,int8_t pos)
{
  while (*cptr)
  {
    if (pos < 8 )
    {
      uint8_t i;
      for (i=0;i<5;i++)
      {
        hcms_buf[(7-pos)*5+i]=reverse_byte( zng[((*cptr)*5)+(i)]);

      }
    }
    cptr++;
    if(pos > -1)pos--;
  }
}

//#endif
