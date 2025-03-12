#include "main.h"
#include <math.h>
#include <stdio.h>
#include "usart.h"
#include "push_pop.h"
#include "lib.h"
#include "algorithm_contamination.h"

bool set_weather = 1;
uint8_t message = 2;
int limit_visibility = 75000;
bool flag_peleng = 1;
bool flag_vaisala = 0;
bool flag_vaisala_fs = 0;
bool PWandFD = 0;
bool flag_request = 0; //is work whith request?
bool request = 0;//is come request?
bool flag_biral = 0;
uint8_t ms_adr[4];

bool error_glass=0;
bool warning_glass=0;
bool error_led = 0;

void pkt_peleng(uint32_t send_val, uint16_t pogoda )
{
  uint8_t obuf [17];
  uint8_t  i;
  uint16_t chk = 0;
  init_obf(obuf);
  if(flag_error.temperature_outside==1 && flag_error.temperature_inside==1)
  {   
    obuf[2]= 0x31;
  }
  if(pprs.PLFull<40000 && aging_LED)obuf[2]+=2;
  if(pprs.PG > 2000 && flag_glass_poll)obuf[2]+=8;
  /*if(aging_LED)
  {
    if(pprs.PL<5)
      obuf[2]+=2;
  }*/
  if(flag_glass_poll)
  {
    obuf[3]+=5;//pollution_status;
  }
  if(flag_termo_compensation)
    obuf[3]+=3;
  
  obuf[4] = (( send_val/ 10000ul) % 10ul) + '0';
  obuf[5] = (( send_val/ 1000ul) % 10ul) + '0';
  obuf[6] = (( send_val/ 100ul) % 10ul) + '0';
  obuf[7] = (( send_val/ 10ul) % 10ul) + '0';
  obuf[8] = (send_val % 10ul) + '0';
  
  /*////////////////////*/
/*  send_val = (uint32_t)pprs.PSmomentary;
  obuf[9] = (( send_val/ 10000ul) % 10ul) + '0';
  obuf[10] = (( send_val/ 1000ul) % 10ul) + '0';
  obuf[11] = (( send_val/ 100ul) % 10ul) + '0';
  obuf[12] = (( send_val/ 10ul) % 10ul) + '0';
  obuf[13] = (send_val % 10ul) + '0';*/
 /*////////////////////*/
  //////
  if(set_weather)
  {
    obuf[9] = ((pogoda >> 4) & 0xf) + 0x30;
    obuf[10] = (pogoda & 0xf) + 0x30;

  }
  else
  {
    obuf[9] =  '0';
    obuf[10] = '0';
  } /**/

  for (i = 1; i < 14; i++)
  {
    chk += (uint16_t)obuf[i];
  }
  chk = (chk & 0xff) + ((chk >> 8) & 0xff);
  obuf[14] = ((chk >> 4) & 0xf) + 0x40;
  obuf[15] = (chk & 0xf) + 0x40;
  oidx = 0;

  HAL_UART_Transmit(&huart4, obuf, 17, 200);
  HAL_UART_Transmit(&huart5, obuf, 17, 150);

  HAL_Delay(10);

}

void  first_pkt_vaisala()
{
  HAL_UART_DeInit( &huart4 );
  HAL_UART_DeInit( &huart5 );
  huart4.Init.BaudRate = speed ;
  huart5.Init.BaudRate = speed ;
  if(word_length==7)
  {
    huart4.Init.Parity = UART_PARITY_EVEN;
    huart5.Init.Parity = UART_PARITY_EVEN;
  }
  if(word_length==8)
  {
    huart4.Init.Parity = UART_PARITY_NONE;
    huart5.Init.Parity = UART_PARITY_NONE;
  }
  HAL_UART_Init( &huart4 );
  HAL_UART_Init( &huart5 );
  
  u4rlen=0xffff;
  
  //VAISALA PWD22 V 1.15 2023-01-15 SN:  9600 ID STRING: 9 
  //56 41 49 53 41 4C 41 20 50 57 44 35 32 20 56 20 31 2E 31 35  ////VAISALA PWD52 V 1.19
  //20 32 30 32 33 2D 30 31 2D 31 35 20 53 4E 3A 20 20 39 36 30   ////_2023-01-15 SN:  960
  //30 20 49 44 20 53 54 52 49 4E 47 3A 20 39                   ////0 ID STRING: 9 
  
  uint8_t obuf [55]={0x56, 0x41, 0x49, 0x53, 0x41, 0x4C, 0x41, 0x20, 0x50, 0x57, 0x44, 0x35, 0x32, 0x20, 0x56, 0x20, 0x31, 0x2E, 0x31, 0x39,
  0x20, 0x32, 0x30, 0x32, 0x34, 0x2D, 0x30, 0x38, 0x2D, 0x30, 0x35, 0x20, 0x53, 0x4E, 0x3A, 0x20, 0x20, 0x39, 0x36, 0x30, 
  0x30, 0x20, 0x49, 0x44, 0x20, 0x53, 0x54, 0x52, 0x49, 0x4E, 0x47, 0x3A, 0x20, 0x39};
  if(flag_vaisala==1)
  {
    if(!PWandFD )
    {
      switch(limit_visibility)
      {
      case 2000:
        obuf[11] = 0x31;
        break;
      case 20000:
        obuf[11] = 0x32;
        break;
      case 50000:
        obuf[11] = 0x35;
        break;
      }
      
      if(set_weather)
        obuf[12] = 0x32;
      else
        obuf[12] = 0x30;
    }
    if(PWandFD)
    {
      obuf[8]  = 0x46;
      obuf[9]  = 0x44;
      obuf[10] = 0x31;
      obuf[11] = 0x32;
      if(set_weather)
        obuf[12] = 0x50;
      else
        obuf[12] = 0x20;
    }
    
  }
  if(flag_vaisala_fs==1)
  {
    obuf[8]  = 0x46;
    obuf[9]  = 0x53;
    obuf[10] = 0x31;
    obuf[11] = 0x31;    
    obuf[12] = 0x20;
  }
  
  //10 11
  /* if(message==7)
  {
  obuf[11] = 0x32;
  if(set_weather)
  obuf[12] = 0x32;
  else
  obuf[12] = 0x30;
}*/
  obuf[16]  = (version/100%10+'0');
  obuf[18] = (version/10%10+'0');
  obuf[19] = (version%10+'0');
  
  obuf[35] =(speed>=100000)?(speed/100000%10+'0'):(' ');
  obuf[36]= (speed>=10000)?(speed/10000%10+'0'):(' ');
  obuf[37]= (speed>=1000)?(speed/1000 %10+'0'):(' ');
  obuf[38]= (speed>=100)?(speed/100%10+'0'):(' ');
  obuf[39]= (speed>=10)?(speed/10%10+'0'):(' ');
  obuf[40]= speed%10+'0';
  
  
  obuf[53] = ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
  obuf[54] = rs_adr % 10 + '0';
  HAL_UART_Transmit(&huart4, obuf, 55, 500);
  HAL_UART_Transmit(&huart5, obuf, 55, 500);
  HAL_Delay(25);
}

void vaisala_header( uint8_t* obuf, uint8_t ms_answer[] )
{
  int index=0;
   // 01 50 57 20 20 39 02 30 30 20
  obuf[index++]=0x01;
  obuf[index++]= ms_answer[0];           //0x50;
  obuf[index++]= ms_answer[1];           //0x57;
  obuf[index++]=0x20;
  obuf[index++]= ms_answer[2];           //0x20;
  obuf[index++]= ms_answer[3];           //0x39;
  obuf[index++]=0x02;
 
  obuf[index++]= '0'; //(aging_LED)?(1):((aging_LED)?(2):(0))+'0';
  obuf[index++]= (error_glass)?(3):((warning_glass)?(4):(0))+'0';
  
  uint32_t out = (uint32_t)pprs.MORstacked;
  if(out>limit_visibility)
    out=limit_visibility;
  if(out<=0) out=1;
  obuf[index++]=  (out>=1000000)?(out/1000000%10+'0'):(' ');
  obuf[index++]=  (out>=100000)?(out/100000%10+'0'):(' ');
  obuf[index++]=  (out>=10000)?(out/10000%10+'0'):(' ');
  obuf[index++]=  (out>=1000)?(out/1000%10+'0'):(' ');
  obuf[index++]=  (out>=100)?(out/100%10+'0'):(' ');
  obuf[index++]=  (out>=10)?(out/10%10+'0'):(' ');
  obuf[index++]=  out%10 +'0';
}

void pkt_vaisala_0(uint8_t ms_answer[])
{
  uint8_t obuf [26];
  vaisala_header(obuf, ms_answer);
 
  int index=16;

  if(!PWandFD)
  {
    uint32_t out = (uint32_t)pop(vaisala_stk_lvlMOR,vaisala_MORstack);
    if(out>limit_visibility)
      out=limit_visibility;
    obuf[index++]= (out>=100000)?(out/100000%10+'0'):(' ');
    obuf[index++]= (out>=10000)?(out/10000%10+'0'):(' ');
    obuf[index++]= (out>=1000)?(out/1000%10+'0'):(' ');
    obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
    obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
    obuf[index++]= out%10 +'0';
  }
  else 
  {
    obuf[index++]= ' ';
    obuf[index++]= '/';
    obuf[index++]= '/';
    obuf[index++]= '/';
    obuf[index++]= '/';
    obuf[index++]= '/';
    obuf[index++]= '/';
  }
  
  obuf[index++]=0x03;
   
  obuf[index++]=0x0D;
  obuf[index++]=0x0A;
  if(!PWandFD)
    HAL_UART_Transmit(&huart4, obuf, 25, 500);
  else
    HAL_UART_Transmit(&huart4, obuf, 26, 500);
 // HAL_UART_Transmit(&huart5, obuf, 25, 500);
  HAL_Delay(25);
}

void pkt_vaisala_1(uint8_t ms_answer[], uint8_t pogoda)
{
  uint8_t obuf [28];
  vaisala_header(obuf, ms_answer);
  int index=16;
 
    
  obuf[index++]= ' ';
/*  if(PWandFD)                //on if sending manual right 
  {
    obuf[index++]= '/';
    obuf[index++]= '/';
  }
  else*/
  if(set_weather)
  {
    obuf[index++] = ((pogoda >> 4) & 0xf) + 0x30;
    obuf[index++] = (pogoda & 0xf) + 0x30;
  }
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
   
  obuf[index++]=0x03;
   
  obuf[index++]=0x0D;
  obuf[index++]=0x0A;
  
  HAL_UART_Transmit(&huart4, obuf, 28, 500);
  HAL_Delay(25);
}

/*void pkt_vaisala_2(uint8_t ms_answer[])
{
  uint8_t obuf [40];
  vaisala_header(obuf, ms_answer);
  int index=16;
  
  obuf[index++]= ' ';
  out = pprs.MORstacked;
  if(out>50000)
    out = 50000;
   
  obuf[index++]= (out>=100000)?(out/100000%10+'0'):(' ');
  obuf[index++]= (out>=10000)?(out/10000%10+'0'):(' ');
  obuf[index++]= (out>=1000)?(out/1000%10+'0'):(' ');
  obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
  obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
  obuf[index++]= out%10 +'0';

  obuf[index++]= ' ';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
   
  obuf[index++]=0x03;
   
  obuf[index++]=0x0D;
  obuf[index++]=0x0A;
  
  HAL_UART_Transmit(&huart4, obuf, 28, 500);
  HAL_Delay(25);
}*/



/*void  first_pkt_vaisala_2()
{
  HAL_UART_DeInit( &huart4 );
  huart4.Init.BaudRate = speed ;
  if(word_length==7)
    huart4.Init.Parity = UART_PARITY_EVEN;
  HAL_UART_Init( &huart4 );

  //VAISALA PWD52 V 1.15 2023-01-15 SN:  9600 ID STRING: 9 
 //56 41 49 53 41 4C 41 20 50 57 44 35 32 20 56 20 31 2E 31 35  ////VAISALA PWD52 V 1.15 
//20 32 30 32 33 2D 30 31 2D 31 35 20 53 4E 3A 20 20 39 36 30   ////_2023-01-15 SN:  960
  //30 20 49 44 20 53 54 52 49 4E 47 3A 20 39                   ////0 ID STRING: 9 

  uint8_t obuf [55]={0x56, 0x41, 0x49, 0x53, 0x41, 0x4C, 0x41, 0x20, 0x50, 0x57, 0x44, 0x35, 0x32, 0x20, 0x56, 0x20, 0x31, 0x2E, 0x31, 0x35,
  0x20, 0x32, 0x30, 0x32, 0x33, 0x2D, 0x30, 0x31, 0x2D, 0x31, 0x35, 0x20, 0x53, 0x4E, 0x3A, 0x20, 0x20, 0x39, 0x36, 0x30, 
  0x30, 0x20, 0x49, 0x44, 0x20, 0x53, 0x54, 0x52, 0x49, 0x4E, 0x47, 0x3A, 0x20, 0x39};

  obuf[16]  = (version/100%10+'0');
  obuf[18] = (version/10%10+'0');
  obuf[19] = (version%10+'0');
  
  obuf[35] =(speed>=100000)?(speed/100000%10+'0'):(' ');
  obuf[36]= (speed>=10000)?(speed/10000%10+'0'):(' ');
  obuf[37]= (speed>=1000)?(speed/1000 %10+'0'):(' ');
  obuf[38]= (speed>=100)?(speed/100%10+'0'):(' ');
  obuf[39]= (speed>=10)?(speed/10%10+'0'):(' ');
  obuf[40]= speed%10+'0';
  
  
  obuf[53]= (((rs_adr >> 4) & 0xf)>0)?(((rs_adr >> 4) & 0xf)+'0'):(' ');
  obuf[54]=((rs_adr & 0xf)>0)?((rs_adr & 0xf)+'0'):(' ');
  HAL_UART_Transmit(&huart4, obuf, 55, 500);
  HAL_Delay(25);
}*/

void  first_pkt_peleng()
{
 //SL-03 V 1.15 2023-01-11 ADR 01
 // 53 4C 2D 30 33 20 56 20 31 2E 31 35 20 32 30 32 33 2D 30 31 2D 31 31 20 41 44 52 20 30 31
  if(speed != 1200 || word_length != 8)
  {
    HAL_UART_DeInit( &huart4 );
    HAL_UART_DeInit( &huart5 );

    huart4.Init.BaudRate = speed;
    huart5.Init.BaudRate = speed;
    
    if(word_length==7)
    {
      huart4.Init.Parity = UART_PARITY_EVEN;
      huart5.Init.Parity = UART_PARITY_EVEN;
    }
     if(word_length==8)
    {
      huart4.Init.Parity = UART_PARITY_NONE;
      huart5.Init.Parity = UART_PARITY_NONE;
    }
    
    HAL_UART_Init( &huart4 ); 
    HAL_UART_Init( &huart5 );
    u4rlen=0xffff;
  }
   //SL-03 V 1.18 2023-07-12 ADR  1..
 /* uint8_t obuf [32]={0x53, 0x4C, 0x2D, 0x30, 0x33, 0x20, 0x56, 0x20, 0x31, 0x2E, 0x31, 0x39, 0x20, 0x32, 0x30, 0x32, 0x34, 0x2D, 0x30, 0x38, 0x2D, 0x30, 0x35, 0x20, 0x41, 0x44, 0x52, 0x20, 0x30, 0x31, 0x0A, 0x0D};
  //version 
 
  //address
  obuf[28] = ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
  obuf[29] = rs_adr % 10 + '0';*/
  char *obuf = (char*)malloc(49);//new char[51]; 
  sprintf(obuf, "SL-03 v1.20, 16/08/2024, ADR % 2d, HH:  A, H:  T \n\r", rs_adr);
  obuf[7]  = (version/100%10+'0');
  obuf[9] = (version/10%10+'0');
  obuf[10] = (version%10+'0');
   if(!flag_heating)     
       obuf[37]='D';    
   if(heatingpresent == 1)
   {
     obuf[44]='O';
     obuf[45]='N';
   }
   if(heatingpresent == 0)
   {
     obuf[44] = 'O';
     obuf[45] = obuf[46] = 'F';     
   }
  HAL_UART_Transmit(&huart4, (uint8_t*)obuf, 49, 500);
  HAL_UART_Transmit(&huart5, (uint8_t*)obuf, 49, 500);
  
  HAL_Delay(25);
  
}

void pkt_vaisala_2(uint8_t* NWS_code,uint8_t pogoda, uint8_t pogoda_minute, uint8_t pogoda_hour, uint8_t ms_answer[])
{
  uint8_t obuf [56];
 // 01 50 57 20 20 39 02 30 30 20
  int index=0;
  obuf[index++]=0x01;
  obuf[index++]= ms_answer[0];           //0x50;
  obuf[index++]= ms_answer[1];           //0x57;
  obuf[index++]=0x20;
  obuf[index++]= ms_answer[2];           //0x20;
  obuf[index++]= ms_answer[3];           //0x39;
  obuf[index++]=0x02;
  
 
/**/ obuf[index++]=(aging_LED)?(1):((error_led)?(2):(0))+'0';
  obuf[index++]=(error_glass)?(3):((warning_glass)?(4):(0))+'0';
  obuf[index++]=0x20;
  int out = (int)pprs.MORstacked;
 // uint32_t out=3972;
  if(out>limit_visibility)
      out=limit_visibility;
  obuf[index++]= (out>=10000)?(out/10000 %10+'0'):(' ');
  obuf[index++]= (out>=1000)?(out/1000 %10+'0'):(' ');
  obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
  obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
  obuf[index++]=  out%10+'0';
 
 
  out = (int)pop(vaisala_stk_lvlMOR,vaisala_MORstack);
  if(out>limit_visibility)
      out=limit_visibility;
  obuf[index++]= (out>=100000)?(out/100000%10+'0'):(' '); 
  obuf[index++]= (out>=10000)?(out/10000%10+'0'):(' ');
  obuf[index++]= (out>=1000)?(out/1000 %10+'0'):(' ');
  obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
  obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
  obuf[index++]= out%10+'0';
  
  obuf[index++]=' '; 
  obuf[index++]= (set_weather)?(NWS_code[0]):('/');
  obuf[index++]= (set_weather)? (NWS_code[1]):('/');
  obuf[index++]=' ';//lj,fdkztncz + bkb -
  
  obuf[index++]=' ';
  obuf[index++] = (set_weather)?(((pogoda >> 4) & 0xf) + 0x30):('/');
  obuf[index++] = (set_weather)?((pogoda & 0xf) + 0x30):('/');
  
  obuf[index++]=' ';
  obuf[index++] = (set_weather)?(((pogoda_minute >> 4) & 0xf) + 0x30):('/');
  obuf[index++] = (set_weather)?((pogoda_minute & 0xf) + 0x30):('/');
  
  obuf[index++]=' ';
  obuf[index++] = (set_weather)?(((pogoda_hour >> 4) & 0xf) + 0x30):('/');
  obuf[index++] = (set_weather)?((pogoda_hour & 0xf) + 0x30):('/');

 
   
  for(int i=0; i<19;i++)
     obuf[index++]=0x2F;
   
   obuf[34]=0x20;
    obuf[35]=0x20;
   obuf[41]=0x20;
    obuf[42]=0x20;
   obuf[48]=0x20;
    obuf[49]=0x20;
   
   // obuf[38]=0x2E;
  // obuf[45]=0x2E;
   
   obuf[53]=0x03;
   
   obuf[54]=0x0D;
   obuf[55]=0x0A;
  
   HAL_UART_Transmit(&huart4, obuf, 56, 500);
  HAL_UART_Transmit(&huart5, obuf, 56, 500);
  //HAL_Delay(25);
}
void pkt_vaisala_fs_2( uint8_t ms_answer[])
{
 uint8_t obuf [40];
  vaisala_header(obuf, ms_answer);
  int index=16;
  
 // obuf[index++]= ' ';
  uint32_t out= (uint32_t)pop(vaisala_stk_lvlMOR,vaisala_MORstack);
  if(out<=0) out = (uint32_t)pprs.MORstacked;
  
  if(out>limit_visibility)
        out=limit_visibility; 
  if(out<=0)
    out=1;
  
  obuf[index++]= (out>=1000000)?(out/1000000%10+'0'):(' '); 
  obuf[index++]= (out>=100000)?(out/100000%10+'0'):(' ');
  obuf[index++]= (out>=10000)?(out/10000%10+'0'):(' ');
  obuf[index++]= (out>=1000)?(out/1000%10+'0'):(' ');
  obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
  obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
  obuf[index++]= out%10 +'0';

 // obuf[index++]= ' ';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= ' ';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
  obuf[index++]= '/';
   
  obuf[index++]=0x03;
   
  obuf[index++]=0x0D;
  obuf[index++]=0x0A;
  
  HAL_UART_Transmit(&huart4, obuf, 40, 500);
  HAL_Delay(out_del);
}


void pkt_vaisala_7(uint8_t* NWS_code,uint8_t pogoda, uint8_t pogoda_minute, uint8_t pogoda_hour, uint8_t ms_answer[])
{
  uint8_t obuf [77]={0x01, 0x50, 0x57, 0x20, 0x20, 0x37, 0x02, 0x30, 0x30, 0x20, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x2F, 0x2F,
                     0x2F , 0x20, 0x2F, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F,
                     0x2F, 0x20, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x2F, 0x2F, 0x20, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x20,
                     0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x0D, 0x0A, 0x2F, 0x2F, 0x20, 0x0D, 0x0A, 0x0D, 0x0A, 0x03, 0x0D, 0x0A
                    };
  
  // 01 50 57 20 20 39 02 30 30 20
  int index=1;
  
  obuf[index++]= ms_answer[0];           //0x50;
  obuf[index++]= ms_answer[1];           //0x57;
  index++;
  obuf[index++]= ms_answer[2];           //0x20;
  obuf[index++]= ms_answer[3];           //0x39;
  index++;
 
  obuf[index++]= (aging_LED)?(1):((error_led)?(2):(0))+'0';
  obuf[index++]= (error_glass)?(3):((warning_glass)?(4):(0))+'0';
  
  uint32_t out = (uint32_t)pprs.MORstacked; 
  if(out>0)
  {    if(out>limit_visibility)
      out=limit_visibility;
      obuf[index++]=  (out>=100000)?(out/100000%10+'0'):(' ');
      obuf[index++]=  (out>=10000)?(out/10000%10+'0'):(' ');
      obuf[index++]=  (out>=1000)?(out/1000%10+'0'):(' ');
      obuf[index++]=  (out>=100)?(out/100%10+'0'):(' ');
      obuf[index++]=  (out>=10)?(out/10%10+'0'):(' ');
      obuf[index++]=  out%10 +'0';
  }
  out = (uint32_t)pop(vaisala_stk_lvlMOR,vaisala_MORstack);
  if(out <= 0)out = (uint32_t)pprs.MORstacked;
  
  if(out>0)
  {
      if(out>limit_visibility)
      out=limit_visibility;
      obuf[index++]= (out>=100000)?(out/100000%10+'0'):(' ');
      obuf[index++]= (out>=10000)?(out/10000%10+'0'):(' ');
      obuf[index++]= (out>=1000)?(out/1000%10+'0'):(' ');
      obuf[index++]= (out>=100)?(out/100%10+'0'):(' ');
      obuf[index++]= (out>=10)?(out/10%10+'0'):(' ');
      obuf[index++]= out%10 +'0';
  }
  
 
   if(set_weather)
  {
    index=21;
     obuf[index++]=' '; 
  obuf[index++]= NWS_code[0];
  obuf[index++]= NWS_code[1];
  obuf[index++]=' ';//lj,fdkztncz + bkb -
  
  obuf[index++]=' ';
  obuf[index++] = ((pogoda >> 4) & 0xf) + 0x30;
  obuf[index++] = (pogoda & 0xf) + 0x30;
  
  obuf[index++]=' ';
  obuf[index++] = ((pogoda_minute >> 4) & 0xf) + 0x30;
  obuf[index++] = (pogoda_minute & 0xf) + 0x30;
  
  obuf[index++]=' ';
  obuf[index++] = ((pogoda_hour >> 4) & 0xf) + 0x30;
  obuf[index++] = (pogoda_hour & 0xf) + 0x30;
  }
  
  HAL_Delay(5);
  HAL_UART_Transmit(&huart4, obuf, 77, 500);
  HAL_UART_Transmit(&huart5, obuf, 77, 500);
 // 
}

void pkt_biral(uint16_t pogoda)
{
   uint8_t obuf [51];
   obuf[0]='S';   //'SWS100'
   obuf[1]='W';
   obuf[2]='S';
   obuf[3]='1';
   obuf[4]='0';
   obuf[5]='0';
   obuf[6]=',';
   
   obuf[7]='0';    //ID
   obuf[8]= ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
   obuf[9]=  rs_adr % 10 + '0';
   obuf[10]=',';
   
   obuf[11]='0';  //র鯤
   obuf[12]=sending_period/10000+'0';
   obuf[13]=sending_period/1000%10+'0';
   obuf[14]=',';
   
   uint32_t out=(uint32_t)pprs.MORstacked/100;
   obuf[15]= (out/1000%10) + '0';    //񼯰אַ䩷沪ῠﰲ鸥񪠿 填𭮱򼠨󱰥宥󩍊   obuf[16]=(out/100%10) + '0';
   obuf[17]='.';
   obuf[18]=(out/10%10) + '0';
   obuf[19]=(out%10) + '0';
  
   obuf[20]='K';
   obuf[21]='M';
   obuf[22]=',';
   
   obuf[23]='9';      //99.999
   obuf[24]='9';
   obuf[25]='.';
   obuf[26]='9';
   obuf[27]='9';
   obuf[28]='9';
   obuf[29]=',';
                      //௣怒
   if (pogoda != 0xffff)
  {
    if(pogoda==0x68)pogoda=0x60; 
    if(pogoda==0x89)pogoda=0x70;
    obuf[30] = ((pogoda >> 4) & 0xf) + 0x30;
    obuf[31] = (pogoda & 0xf) + 0x30;
   
  }
   obuf[32]=',';
   
   obuf[33]='+';
   obuf[34]='9';      //+99.9
   obuf[35]='9';
   obuf[36]='.';
   obuf[37]='9';
   obuf[38]='9';
   out=(uint32_t)pprs.MORmomentary/100;
   obuf[39]=(out/1000%10) + '0';    //񼯰אַ䩷沪ῠﰲ鸥񪠿 填𭮱򼠨񛓥󩍊   obuf[40]=(out/100%10) + '0';
   obuf[41]='.';
   obuf[42]=(out/10%10) + '0';
   obuf[43]=(out%10) + '0';
   obuf[44]=',';
   
   obuf[45]='K';
   obuf[46]='M';
   obuf[47]=',';
   
   obuf[48]='O';
   obuf[49]='O';
   obuf[50]='O';
  
   HAL_UART_Transmit(&huart4, obuf, 51, 200);
   HAL_UART_Transmit(&huart5, obuf, 51, 200);
  //HAL_Delay(25);
}

uint8_t const phenocodes[] = { 0x00,/*Clear*/ /*C*/
                               0x04,/*Haze*/ /*C*/
                               0x30,/*Fog*//*C*/
                               0x40,/* Precipitation *//*P*/
                               0x50,/*drizzle*//*L*/
                               0x89,/*hail*//*IC*/
                               0x68,/*Rain and snow*//*SP*/
                               0x60,/*rain*//*R*/
                               0x70};/*Snow*/   /*S*/ 
uint8_t const NWS_codes[9][2] = {{0x20, 0x43},/*Clear*/ /*C*/
                                 {0x20, 0x43},/*Haze*/ /*C*/
                                 {0x20, 0x43},/*Fog*//*C*/
                                 {0x20, 0x50},/* Precipitation *//*P*/
                                 {0x20, 0x4C},/*drizzle*//*L*/
                                 {0x49, 0x43},/*hail*//*IC*/
                                 {0x53, 0x50},/*Rain and snow*//*SP*/
                                 {0x20, 0x52},/*rain*//*R*/
                                 {0x20, 0x53}};/*Snow*/   /*S*/                                                               
                                  
bool flag_first_package=0;
void read_memory()
{
  //id  
  //period

  word_length = (int)read_in_mem("Parity",(8), 2688);
  rs_adr = (uint32_t)read_in_mem("address", (1), 2752);
  //version = read123("version", (118), 3648);
  speed = (int)read_in_mem("speed_in_memory", (1200), 576);
  set_weather = read_in_mem("weather", (1), 2816);
  sending_period = (int)read_in_mem("period", (15000), 2880);
  flag_request = read_in_mem("flag_request", (0), 2944);
  limit_visibility = (int)read_in_mem("limit_visibility", (75000), 4096);
  del = (int)read_in_mem("delay", (0), 4160);
  out_del = (int)read_in_mem("delay", (5), 4224);
}
void first_putpkt()
{
  //peleng package
  //50 4C 4E 47 30 33 2C 20 20 31 32 30 30 20 38 4E 31
  uint8_t obuf [19]={0x50, 0x4C, 0x4E, 0x47, 0x30, 0x33, 0x2C, 0x20, 0x20, 0x31, 0x32, 0x30, 0x30, 0x20, 0x38, 0x4E, 0x31, 0x0A, 0x0D};
//  if(flag_peleng==1)
 // { }
  if(flag_vaisala==1)
  {
     if(!PWandFD )
     {
     obuf[0]='P';
     obuf[1]='W';
     obuf[2]='D';
     obuf[3]=' ';
     switch(limit_visibility)
     {
        case 2000:
          obuf[4] = 0x31;
          break;
        case 20000:
          obuf[4] = 0x32;
          break;
        case 50000:
          obuf[4] = 0x35;
          break;
      }
    
     if(set_weather)
         obuf[5] = 0x32;
       else
         obuf[5] = 0x30;
      }
  if(PWandFD)
  {
     obuf[0]  = 0x46;
     obuf[1]  = 0x44;
     obuf[2]  = 0x20;
     obuf[3] = 0x31;
     obuf[4] = 0x32;
    
     if(set_weather)
       obuf[5] = 0x50;
     else
       obuf[5] = 0x20;
  }
  }
  if(flag_vaisala_fs==1)
  {
     obuf[0]  = 0x46;
     obuf[1]  = 0x53;
      obuf[2] = 0x20;
     obuf[3] = 0x31;
     obuf[4] = 0x31;    
     obuf[5] = 0x20;
  }
 
   if(flag_biral==1)
  {
    obuf[0]= 0x53;
    obuf[1]= 0x57;
    obuf[2]= 0x53;
    obuf[3]= 0x31; 
    obuf[4]= 0x30; 
    obuf[5]= 0x30; 
  }
  read_memory();
  obuf[8]= (speed>=10000)?(speed/10000%10+'0'):(' ');
  obuf[9]= (speed>=1000)?(speed/1000 %10+'0'):(' ');
  obuf[10]= (speed>=100)?(speed/100%10+'0'):(' ');
  obuf[11]= (speed>=10)?(speed/10%10+'0'):(' ');
  obuf[12]= speed%10+'0';
 
  if(word_length==8)
  {
    obuf[14]=0x38;
    obuf[15]=0x4E;
    obuf[16]=0x31;
  
  }
  if(word_length==7)  
    {
    obuf[14]=0x37;
    obuf[15]=0x45;
    obuf[16]=0x31;
  
  }

  HAL_UART_Transmit(&huart4, obuf, 19, 150);
  HAL_UART_Transmit(&huart5, obuf, 19, 150);
  HAL_Delay(50);
    
  
  if(flag_peleng==1)
  {
    first_pkt_peleng();
  }
  if(flag_vaisala==1 || flag_vaisala_fs==1)
  {
   first_pkt_vaisala();
  }
  
  if(flag_biral==1)
  {
  }
  u4_rcv_pkt_IT();
}
void putpkt(uint32_t send_val, uint8_t ms[], uint8_t index )
{
 /* if((pprs.PG >= 3000UL) && (pprs.PG<8000UL))
    warning_glass=1;
  if(pprs.PG>=8000UL) 
    error_glass=1;*/
 
  //first weather package
  //greeting

    uint16_t pogoda = 0x00;
    pogoda=phenocodes[index];
 if (request ||!flag_request)
 { 
   HAL_Delay(del);
   HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET);        
 
   if(flag_peleng==1)
      pkt_peleng(send_val, pogoda);
   if(flag_vaisala==1)
   { 
      if(message==0)
        pkt_vaisala_0(ms);  
      if(message==1)
        pkt_vaisala_1(ms, pogoda);
      if(message==2)
      {
        uint16_t pogoda_minute = phenocodes[index_minute]; //weather per minute
        uint16_t pogoda_hour = phenocodes[index_hour]; //wether per hour
        uint8_t NWS_code[2]= { NWS_codes[index][0],  NWS_codes[index][1]}; //NWS weather
        pkt_vaisala_2(NWS_code, pogoda, pogoda_minute,pogoda_hour, ms); 
      }
      if(message==7)
      {
         uint16_t pogoda_minute = phenocodes[index_minute]; //weather per minute
         uint16_t pogoda_hour = phenocodes[index_hour]; //wether per hour
         uint8_t NWS_code[2]= { NWS_codes[index][0],  NWS_codes[index][1]}; //NWS weather
         pkt_vaisala_7(NWS_code, pogoda, pogoda_minute,pogoda_hour, ms);  
      }
    }
    if(flag_vaisala_fs==1)
    {
      pkt_vaisala_fs_2(ms);
    }

    if(flag_biral==1)
    pkt_biral(pogoda);
    HAL_GPIO_WritePin(RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET);
  }
   HAL_UART_Receive_IT(&huart4, ibuf, 256);
   HAL_UART_Receive_IT(&huart5, ibuf, 256);
    
}
