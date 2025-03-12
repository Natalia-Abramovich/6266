#include "main.h"
#include "usart.h"
#include "tim.h"
#include "i2c.h"
//#include "eeprom.h"
#include "detector_weather.h"
#include "OneWire.h"
#include "lib.h"
#include "i2c.h"
#include "algorithm_contamination.h"
#include "approximation.h"
#include "push_pop.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

uint32_t rs_adr = 1;   //address 
uint32_t version = 0;   //version 
uint32_t ID = 22626; //number board
uint32_t number = 6039; //number board
bool flag_realization = 1;
int sending_period = 15000;
bool flag_delayed_recording = 0;
int number_of_iteration = 0;

double  *adr_temporary_stack1;
double  *adr_temporary_stack2;

double * adr_temporary_value;

char * temporary_name1;
char * temporary_name2;
 
int how_many_adr;

HAL_StatusTypeDef hs;

int index_memory_history=0;
bool flag_history=0;

void history_write(uint8_t *b)
{
  //count size command
   int memAddr=6400+16*index_memory_history;
   int i=1;
   while(i<17 && b [i-1]!=0x0d){i++;}  
   for(;i<17; i++)b[i]=0;
    write( (uint8_t*)b, memAddr, 16);   
   index_memory_history++;
   if(index_memory_history>10)index_memory_history=0;
}

void history_read()
{
  uint16_t devAddr = (0x50 << 1);  
  uint8_t read_value[16];

  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
  int memAddr_value;
  int count;
  if (flag_history) count = 10;
  else count = index_memory_history;
  int i=index_memory_history-1;
  
  while (count>0)
  {     
     if(i<0)
       i = flag_history ? 9 : (index_memory_history-1);
     memAddr_value = 6400+i*16;
     if ( read( (uint8_t*)read_value, memAddr_value)) 
     {
        HAL_UART_Transmit(&huart4, read_value, 16, 100);
        HAL_UART_Transmit(&huart5, read_value, 16, 100);
     }     
     i--;
     count--;
  }
  
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
}


void correct_command(uint8_t *console_buffer)
{
for(int i=0;i<256;i++)

if (console_buffer[i]>0x60 && console_buffer[i]<0x7b)
  console_buffer[i]-=0x20;
}

void communicator(char text[], uint8_t size)
{   
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
  
  HAL_UART_Transmit( &huart4, (uint8_t*)text ,size, size*10);
 
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
}

int bit_depth(int num)
{
  int n=1;
  while((abs(num))>= (int)pow(10,n)) n++;
  return n;
}


void change_speed(uint8_t *console_buffer)
{
  //BAUDx //change speed //0-1200 1-2400 2-4800 3-9600 4-14400 5-19200 6-38400 7-57600
  flag_realization=1;
  HAL_UART_DeInit( &huart4 );
  HAL_UART_DeInit( &huart5 );
   switch(console_buffer[4])//корректировка скорости передачи 
   {
     //speed
     case 0x30: speed = 1200; //B0 
     break;
     case 0x31: speed = 2400; //B1
     break;
     case 0x32: speed = 4800;
     break;
     case 0x33: speed = 9600;
     break;
     case 0x34: speed = 14400;
     break;
     case 0x35: speed = 19200;
     break;
     case 0x36: speed = 38400;
     break;
     case 0x37: speed = 57600;
     break;
     default: flag_realization=0;
     break;
  }
   
    huart4.Init.BaudRate = speed;
    huart5.Init.BaudRate = speed;
    HAL_UART_Init( &huart4 );
    HAL_UART_Init( &huart5 );
    u4rlen=0xffff;
    u4_rcv_pkt_IT();
    if(!write123("speed_in_memory",(speed),576))
      flag_realization=0;
    HAL_Delay(100);  
}

//flags
void setting_flag(uint8_t *console_buffer)                                //отключить/включить флаги 
{
  bool flag=0;
  if(console_buffer[0]==0x53) flag=1;
  
  if(find_subarray(console_buffer," APPROX\r", 8))
  {    
    if(write123("fl_app", (flag),4352))
    {
      flag_realization=1;    
      flag_approximation = flag; //SET approx       
    }
    return;
  }
  if ( find_subarray(console_buffer," AGING\r", 7))
  {
    if(write123("aging_LED", (flag),4032))
    {
      aging_LED = flag;  //SET Aging
      flag_realization=1;
    }
    return;
  }
  if(find_subarray(console_buffer, " CS\r", 4))
   { 
     if(write123("CS",(flag),4672))
     {
       flag_cs = flag; //SET CS /CLR CS
       flag_realization = 1;
     }   
     return;
   }
  if(find_subarray(console_buffer," DEBUG\r", 7))
  {    
     flag_realization = 1;
     flag_out = flag; //Mode Debug //SET Debug 
     if(flag) stop_tim();
     return;
  }
  
  if(find_subarray(console_buffer,"ALL ECO\r", 8))
  {   
    flag_energy = flag;        
    write123("energy",(flag),1216);
    flag_realization = 1; 
    if(!flag_heating)
    {
       
       if(flag_energy) 
         HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_RESET);
       else
         HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET); 
    }
    else
       communicator("HOOD HEATER ACT\n\r", 17);
    
    return;
  }
  if(find_subarray(console_buffer,"MOR ECO\r", 8))
  {   
    if(flag_heating)
   {
     communicator("HOOD HEATER ACT\n\r", 17);
     return;
   }
     if(flag_energy)
   {
     flag_realization = 0;
     return;
   }
   flag_mor_eco = flag; 
   flag_realization = 1;
   write123("mor_eco",(flag), 4864); 
  
  return;
  }
  
   if(find_subarray(console_buffer, " GLASS\r", 7))
   { 
     if(write123("glass_poll", flag, 3904))
     {
       flag_glass_poll = flag; //SET Glass
       flag_realization = 1;
     }   
     return;
   }
  
  if(find_subarray(console_buffer, " OUTT\r",6))
  {
    if(write123("Termo", (flag), 3712))
    {
      thresholds.termo =  flag;
      flag_realization = 1;
    }  
    return;
  }
  if(find_subarray(console_buffer, " REQUEST\r", 9))
  {
    //SET Request //Mode Request //CLR Request
    if( write123("flag_request",(flag),2944))
    {
      flag_request = flag;
      flag_realization = 1;
    }
    return;
  }
  if(find_subarray(console_buffer,"T COMP\r", 7))
  {
    if(write123("compensation", flag, 3840))
    {
      flag_termo_compensation = flag;
      flag_realization =1;
    }
    return;
  }
  // will be removed, old algorithm
  if(find_subarray(console_buffer," UNI\r", 5))
  {
     //Mode UNI //CLR UNI //SET UNI    
     if((flag_unification && flag )|| (flag && !flag_cs)) 
     {    
       flag_realization = 0;
       communicator("Unification not available",25);      
     }
     else
     {
       flag_realization = 1;
       flag_unification = flag; 
       write123("unification",(flag),384);
       unification();  
     }
     return;
   }
   if(find_subarray(console_buffer," W\r", 3))
    {
      //Mode W //CLR W //SET W
      if(write123("weather", flag, 2816))
      {
        flag_realization = 1;
        set_weather = flag;      
      }
      return;
    }
 
}



void read_32(int num)
{
    
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
 
  flag_realization=1;
  uint8_t n=bit_depth(num);
  uint8_t *buffer=new uint8_t[n+3];
  if (num>=0)buffer[0]='+';
  else
  {
    buffer[0]='-';
    num=abs(num);
  }
  
  for(int i=1; i<n+1; i++)
  {
    int p=pow(10,n-i);
    buffer[i]=num/p%10+ '0';
  }
  buffer[n+1]=0x0A;
  buffer[n+2]=0x0D;
  HAL_UART_Transmit( &huart4, buffer,n+3,100);
 // HAL_UART_Transmit( &huart5, buffer,n+1,100);
  delete[]buffer;
  
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
}

void read_d(double num)
{
   HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
 
  flag_realization=1;
   uint8_t n=bit_depth(num);
   uint8_t *buffer=new uint8_t[n+6];

   if(num<0)
   {
     buffer[0] = 0x2D;
     num*=(-1);
   }
    else buffer[0] = 0x2B;
    uint32_t s = num*100;
    int i=1;
    for( ;i<=n;i++)
    {
      int p=pow(10,n+2-i);
      buffer[i] = s/p%10+ '0';
    }
    buffer[i++]=0x2E;
    buffer[i++] = s/10%10+ '0';
    buffer[i++] = s%10+ '0';

    buffer[i++]=0x0A;
    buffer[i++]=0x0D;
    HAL_UART_Transmit( &huart4, buffer,n+6,100);
   // HAL_UART_Transmit( &huart5, buffer,n+4,100);
    delete[]buffer;
    
    HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );

}

void light_reset()
{
  sending_period=15000;
  write123("period",(sending_period),2880);
  
  flag_glass_poll = 0;
  aging_LED = 0;
  flag_termo_compensation = 0;
  
  write123("aging_LED",(aging_LED),4032);
  write123("glass_poll", flag_glass_poll, 3904);
  write123("compensation", flag_termo_compensation, 3840);

  flag_energy = 0;      
  write123("energy",(0),1216) ; 
}

void read(uint8_t *b)
{ 
//  HAL_GPIO_WritePin( M0_GPIO_Port, M0_Pin, GPIO_PIN_SET );
//  HAL_GPIO_WritePin( M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
   if(b[3]==0x58&&  b[4]==0x0D)
  {
   read_32(X); 
  }
  if(b[3]==0x4D &&  b[4]==0x4F && b[5]==0x52 && b[6]==0x0D) //RD MOR
  {
   read_d(pprs.MORmomentary);
 //  read_d(pprs.MORstacked);
  }
  
///////Mode UNI //CLR UNI //SET UNI
// calibration   
  //
  if(b[3]==0x48 &&  b[4]==0x49 && b[5]==0x47 && b[6]==0x48 && b[7]==0x0D)//RD HIGH.
  {//старая калибровка верхней видимости 
   read_d(calibration_value.MORmsb);  //old calibration value hight visibility
  } 
  if(b[3]==0x55 &&  b[4]==0x20 && b[5]==0x48 &&  b[6]==0x49 && b[7]==0x47 && b[8]==0x48 && b[9]==0x0D) //RD U HIGH.
  {
   //унифицированное занчение верхней видимости  
   read_d(uni_calibration_value.MORmsb);  //unification hight val
  }
  if(b[3]==0x50 &&  b[4]==0x53 && b[5]==0x20 && b[6]==0x4D && b[7]==0x0D) //RD PS m.
  {
   //old calibration value photodetector 
   read_d(calibration_value.PSmsb );  // PS 0.1
  }
  if(b[3]==0x55 &&  b[4]==0x50&& b[5]==0x53 && b[6]==0x4D && b[7]==0x0D)//RD UPSm.
  {
   read_d(uni_calibration_value.PSmsb);  
  }
 
  if(b[3]==0x4C &&  b[4]==0x4F && b[5]==0x57  && b[6]==0x0D)//RD LOW.
  {
   read_d(calibration_value.MORlsb);  
  }
  if( b[3]==0x55 &&  b[4]==0x20 && b[5]==0x4C &&  b[6]==0x4F && b[7]==0x57  && b[8]==0x0D)//RD U LOW.
  {
   read_d(uni_calibration_value.MORlsb);  
  }
  if(b[3]==0x50 &&  b[4]==0x53 && b[5]==0x20 && b[6]==0x4C && b[7]==0x0D)//RD PS l.
  {
   read_d(calibration_value.PSlsb);   // PS 1400
  }
  if(b[3]==0x55 &&  b[4]==0x50 && b[5]==0x53 && b[6]==0x4C && b[7]==0x0D)//RD UPSl.
  {
   read_d(uni_calibration_value.PSlsb);  
  }  
  if(b[3]==0x53 &&  b[4]==0x49 && b[5]==0x47 && b[6]==0x4D && b[7]==0x41 && b[8]==0x0D)//RD SIGMA.
  {
   read_d(pprs.SIGMA);
  }
  if(b[3]==0x43 &&  b[4]==0x4F && b[5]==0x52 && b[6]==0x52 && b[7]==0x0D) //RD corr.
  {
   read_d(pprs.CORR);
  }
   if( b[3]==0x44 && b[4]==0x45 && b[5]==0x4C && b[6]==0x54 && b[7]==0x41 && b[8]==0x0D)  //RD Delta
  {
   read_d(pprs.PSoffset);
   // read_d(0);
  }
 /////////////// calibration  
  /////////
  if(b[3]==0x50 &&  b[4]==0x4C && b[5]==0x0D)
  {
    flag_realization = 1;
    char *str = (char*)malloc(60);//new char[51]; 
    sprintf(str, "PLFULL = %-9ld\r\nPL0 = %-9ld\r\nPLMOMENT = %-9ld\r\n", (int)pprs.PLFull, (int)pprs.PL0, (int)pprs.PL);
    communicator(str, 59);      
    free(str);   
   
  }
  
  if(b[3]==0x50 &&  b[4]==0x47 && b[5]==0x0D)
  {
  flag_realization = 1;
    char *str = (char*)malloc(60);//new char[51]; 
    sprintf(str, "PGFULL = %-9ld\r\nPG0 = %-9ld\r\nPGMOMENT = %-9ld\r\n", (int)pprs.PGFull, (int)pprs.PG0, (int)pprs.PG);
    communicator(str, 59);      
    free(str);   
  }
  
  if(b[3]==0x50 &&  b[4]==0x53 && b[5]==0x0D) //RD PS
  {
     flag_realization = 1;
     HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET ); 
     HAL_UART_Transmit( &huart4, (uint8_t*)"Wait! The device measures the data for 1 minute!\r\n" ,50, 500);

     number_of_iteration = 0;
     flag_delayed_recording = 1;
     adr_temporary_value = &pprs.PS;

  /*  communicator("PSfull = ", 9);
    read_d(pprs.PS);
    HAL_Delay(10);
    communicator("PS0 = ", 6);
    read_d(pprs.PS0);
    HAL_Delay(10);
    communicator("PSmom = ", 8);
    read_d(pprs.PSmomentary);*/

  }

  if(b[3]==0x4B &&  b[4]==0x50 && b[5]==0x0D)
  {
    read_d(Kp);//in developing
  }
  if(b[3]==0x4B &&  b[4]==0x44 && b[5]==0x49 && b[6]==0x52 && b[7]==0x54 && b[8]==0x59 && b[9]==0x0D)
  {
   // read_d( pprs.MORmomentary);//Kdirty //in developing
  }
  if(find_subarray(b, "TREF\r", 5))
  {   
    flag_realization = 1;
    char *str = new char[13];//in developing
    sprintf(str, "Tref = %.1f\r\n", Tref);
    communicator(str, 13);      
    free(str); 
    //in developing
  }
  if(find_subarray(b, "PLREF\r", 5))
  {
    flag_realization = 1;
    char *str = new char[16]; //in developing
    sprintf(str, "PLref = %6d\r\n", (int)PLref);
    communicator(str, 16);      
    free(str);    
   
  }
  if(b[3]==0x47 &&  b[4]==0x6C && b[5]==0x61 && b[6]==0x73 && b[7]==0x73 && b[8]==0x0D)
  {
   // read_32(75000);//in developing
  }
///////////  
//temperature
  
  if(b[3]==0x54 &&  b[4]==0x45 && b[5]==0x4D && b[6]==0x50 && b[7]==0x0D)//RD Temp
  {
    flag_realization=1;
    double value=get_termoval();
    if(!flag_error.temperature_inside)
      read_32(value/10); //temp inside
    else
      communicator("No sensor", 9);
  }
   if(b[3]==0x54 &&  b[4]==0x4F && b[5]==0x55 && b[6]==0x54 && b[7]==0x0D)//RD TOut
  {
    flag_realization=1;
    get_ROMid();
    get_Temperature();
    if(devices!=0x00)
    {      
      if(Temp>84) get_Temperature();
      read_d(Temp); //temperature outside
    }
    else
      communicator("No sensor", 9);   
  }
   if(b[3]==0x54 && b[4]==0x43 && b[5]==0x4F && b[6]==0x52 && b[7]==0x52)  // RD tcorr
  {
    read_d(thresholds.t_corr); //read inside correction
  }
  // 4F 55 54 20 54 43 4F 52 52 0D
   if(b[3]==0x4F && b[4]==0x55 && b[5]==0x54 && b[6]==0x20 && b[7]==0x54 && b[8]==0x43 && b[9]==0x4F && b[10]==0x52 && b[11]==0x52) // RD out tcorr
  {
     read_d(thresholds.out_t_corr);//read outside correction
  }
    if(b[3]==0x54 &&  b[4]==0x2B && b[5]==0x43 && b[6]==0x0D )//RD t+c
  {
    flag_realization=1;
    double value=get_termoval();
    if(!flag_error.temperature_inside)
      read_d(value/10.0 + thresholds.t_corr);//read inside T + corr
    else
      communicator("No sensor", 9);
  }
    if(b[3]==0x4F && b[4]==0x54 &&  b[5]==0x2B && b[6]==0x43 && b[7]==0x0D )//RD ot+c
  {
    flag_realization=1;
    get_ROMid();
    get_Temperature();
    if(devices!=0x00)
    {      
      if(Temp>84) get_Temperature();
      read_d(Temp + thresholds.out_t_corr);//read outside T + corr 
    }
    else
      communicator("No sensor", 9);   
  
  }
  if(find_subarray(b, "RD T\r", 5))
  {
    flag_realization=1;
    get_ROMid();
    get_Temperature();
    
    float value = get_termoval()/10;
    char * str = new char[81]; 
    sprintf(str, "Tin = %+2.1f\rTin+d = %+2.1f\rTout = %+2.1f\rTout+d = %+2.1f\rThigh = %+2.1f\rTlow = %+2.1f\r\n",value, value + thresholds.t_corr, Temp, Temp + thresholds.out_t_corr, Thigh, Tlow);
    
    communicator(str, 81);
  }
  if(find_subarray(b, "RD HYSTERESIS", 13))
  { 
    read_32(hysteresis);
  }
    
///////temperature
/////
  if(b[3]==0x50 &&  b[4]==0x45 && b[5]==0x52 && b[6]==0x49 && b[7]==0x4F && b[8]==0x44 && b[9]==0x0D )
  {
    read_32(sending_period/1000);
  }
  if(b[3]==0x43 &&  b[4]==0x4C && b[5]==0x45 && b[6]==0x41 && b[7]==0x52 && b[8]==0x0D )
  {
    read_32(thresholds.clear);
  }
  if(b[3]==0x4E && b[4]==0x4F  && b[5]==0x49 &&  b[6]==0x53 &&  b[7]==0x45)
  {
     read_32(thresholds.noise);
  }
  if(b[3]==0x48 && b[4]==0x41 && b[5]==0x5A && b[6]==0x45)
  {
     read_32(thresholds.haze);
  }
  if(b[3]==0x46 && b[4]==0x4F && b[5]==0x47)
  {
     read_32(thresholds.fog);
  }
  if(b[3]==0x44 && b[4]==0x44 && b[5]==0x52 && b[6]==0x49 && b[7]==0x5A && b[8]==0x5A)
  { 
    read_32(thresholds.drizzle);
  }
  if(b[3]==0x44 && b[4]==0x50 && b[5]==0x52)
  {
     read_32(thresholds.precipitation);
  }
  if(b[3]==0x4C && b[4]==0x49 && b[5]==0x4D && b[6]==0x49 && b[7]==0x54)
  {
     read_32(thresholds.limitcipitation);
  }
   if(b[3]==0x43 && b[4]==0x53)  {
   
    flag_realization = 1;
    char *str = (char*)malloc(13);//new char[51]; 
    sprintf(str, "CS = %+.3f\r\n", (float)pprs.scattering_coefficient);
    communicator(str, 13);      
    free(str); 
  
  }
  if(b[3]==0x53 && b[4]==0x4C && b[5]==0x49)
  {
     read_32(thresholds.sumlimit);
  }
    if(find_subarray(b, "RD DELAY\r", 9))
  {
    read_32(del);  
  }
  
     if(find_subarray(b, "RD OUT DELAY\r", 13))
  {
    read_32(out_del);  
  }

  if(find_subarray(b, "RD N\r", 5))
  {
    read_32(amount_point);
  }
  if(find_subarray(b, "RD MAS\r", 7))
  {
    flag_realization=1;
    char *str = new char[45];
    for(int i = 0; i < amount_point; i++)
    {
      int mor = (int)dataMOR[i];
      float ps = (float)(round(dataPS[i]*1000))/1000;
      int size = 23 + bit_depth(ps);
      if(i == 0)
      {
        int end = (int)round(dataMOR[i] * 10) % 10;
        if (end > 0)
          sprintf(str, "mor%-2d %4ld.%1d\tps%-2d %.3f\r", i+1, mor, end, i+1, ps);    
        else
          sprintf(str, "mor%-2d %6ld\tps%-2d %.3f\r", i+1, mor, i+1, ps);
      }
      else
        sprintf(str, "mor%-2d %6ld\tps%-2d %.3f\r", i+1, mor, i+1, ps);
      communicator(str, size);    
      HAL_Delay(300);
    } 
 //delete[]str;
  free(str);
  }
  if(find_subarray(b, "RD DPS\r", 7))
  {
    read_d(delta);  
  }
   if(b[3]==0x53 && b[4]==0x4C && b[5]==0x50)
  {
     read_32(thresholds.slp);
  }
  
  if(b[3]==0x4C && b[4]==0x49  && b[5]==0x4D &&  b[6]==0x53 &&  b[7]==0x4E && b[8]==0x4F && b[9]==0x57) 
  {
     read_d(thresholds.limsnow);   
  }
  if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x55 && b[8]==0x52 && b[9]==0x0D) 
  {
    read_d(thresholds.minduration);
  }

  if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x45 && b[8]==0x52 && b[9]==0x0D )
  {
    read_d(thresholds.minderivative);    
  }
  if(b[3]==0x43 && b[4]==0x4C  && b[5]==0x45 &&  b[6]==0x41 &&  b[7]==0x52 && b[8]==0x50) 
  {
    read_d(thresholds.clearp);
  }                                                                                              
   if(b[3]==0x42 && b[4]==0x4F  && b[5]==0x4F &&  b[6]==0x53 &&  b[7]==0x54) 
  {
    read_d(thresholds.l);
  }
 
  if(b[3]==0x4C && b[4]==0x53 && b[5]==0x48)////RD LSH
  {
    read_32(thresholds.limspeedhail);
  }
    if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x55 && b[8]==0x52 && b[9]==0x48 && b[10]==0x41 && b[11]==0x49 && b[12]==0x4C && b[13]==0x0D) //RD mindurhail
  {
    read_32(thresholds.mindurationhail);
  }
  
    if(b[3]==0x46 && b[4]==0x4C  && b[5]==0x41 &&  b[6]==0x47) //RD FLAGS //flags
  { 
    int cell=0;
    uint8_t *buffer = new uint8_t[12]{0x30, 0x30, 0x30, 0x30,0x30, 0x30, 0x30, 0x30, 0x30, 0x30, };
    buffer[cell++] += flag_energy;
    buffer[cell++] += thresholds.flag_hail;
    buffer[cell++] += flag_request;
    buffer[cell++] += thresholds.termo;
    buffer[cell++] += flag_unification;
    buffer[cell++] += set_weather;
    
    // new
    buffer[cell++] += flag_approximation;
    buffer[cell++] += aging_LED;
    buffer[cell++] += flag_glass_poll;
    buffer[cell++] += flag_termo_compensation;
    
    buffer[cell++]=0x0A;
    buffer[cell++]=0x0D;
    HAL_UART_Transmit( &huart4, buffer,12,100);

    delete[]buffer;
    flag_realization=1;
  }
  if( b[3]==0x4D &&  b[4]==0x41 && b[5]==0x58 &&  b[6]==0x4D && b[7]==0x4F  && b[8]==0x52 && b[9]==0x0D)//RD maxMOR.
  {
   read_32(limit_visibility);  
  }  
 // HAL_GPIO_WritePin( M0_GPIO_Port, M0_Pin, GPIO_PIN_RESET );
 // HAL_GPIO_WritePin( M1_GPIO_Port, M1_Pin, GPIO_PIN_SET );
  HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
}
int read_value_from_package(uint8_t *b, uint8_t index) 
{
  int i=index;
  int input=0;
  flag_realization=1;
  
  while(b[i+1]!=0x0D)
    i++;
  if(i<=22)
  for (double k = 1; i > index; k *= 10, i--)
    { 
      if((b[i]>=0x30 && b[i]<0x40) || b[i]==0x2D)
     {
        if(b[i]!=0x2D)
         input += (b[i] - '0') * k;
        else
         input*=(-1);
      }
      else k = (double)(k / 10.0);
  }
  else flag_realization=0;
  return input;
}

double read_value_from_package_d(uint8_t *b, uint8_t index) 
{
  flag_realization=1;
  int i=index;
  double input=0;
  while(b[i+1]!=0x0D)
    i++;
  if(i<=22)
  for (double k = 1; i > index; k *= 10, i--)
  {
    if(b[i] > 0x20 && b[i]<0x40) 
    {
      if(b[i]!=0x2D && b[i]!=0x2B)
      {
        if(b[i]>=0x30 )
        {
          input += (b[i] - '0') * k;       
        }
        else
        {
          input/=k;
          k=0.1;
        }
      }
      else
        if(b[i]==0x2D)
          input*=(-1);
    }
    else k/=10;
  }
  else flag_realization=0;
  return input;
}

void write(uint8_t *b)
{
  if(find_subarray(b, "WR HYSTERESIS", 13))
  {
    int value  = read_value_from_package(b, 13); 
    if(value >= 0 && value < 100)
    {
      hysteresis = value;
      write123 ("hysteresis",(value), 4928);
      flag_realization=1;  
    }
  }
  
  if( b[3]==0x41 && b[4]==0x44 && b[5]==0x52 && b[6]==0x20)
  {
    rs_adr = read_value_from_package(b, 6); 
    write123("address",(rs_adr),2752); 
    flag_realization=1;      
  }

  if( b[3]==0x49 && b[4]==0x44  && b[5]==0x20 &&  b[15]==0x0D)
  {
        ID=0; number=0;
        for(int i=0;i<5; i++)
          ID = ID*10+(b[i+6]-0x30);
         for(int i=5;i<9; i++)
          number = number *10+(b[i+6]-0x30);
        write123("id",(ID),3008);
        write123("num_board",(number),3072);
         flag_realization=1;       
   }
    
      if(b[3]==0x48 && b[4]==0x49  && b[5]==0x47 &&  b[6]==0x48 &&  b[7]==0x20)
      {
        calibration_with_console(b, 12, 7);        
        flag_realization=1;
      } 
  
      if(b[3]==0x50 && b[4]==0x53  && b[5]==0x20 &&  b[6]==0x4D &&  b[7]==0x20)
      {
         calibration_value.PSmsb  = read_value_from_package_d(b, 7) ;
         write123("PSmsb", (calibration_value.PSmsb),64);
         for(uint8_t i=0; i<100; i++){__no_operation();} 
      } 
      
      if(b[3]==0x4C && b[4]==0x4F  && b[5]==0x57 &&  b[6]==0x20)
      {
         calibration_with_console(b, 12, 6);               
         flag_realization=1;
      }
     if(b[3]==0x50 && b[4]==0x53  && b[5]==0x20 &&  b[6]==0x4C &&  b[7]==0x20)
     {
        calibration_value.PSlsb = read_value_from_package_d(b, 7) ;
      
        write123("PSlsb", (calibration_value.PSlsb),192);
        for(uint8_t i=0; i<100; i++){__no_operation();} 
   
       /* pprs.CORR = (pprs.PSlsb * (double)pprs.MORlsb - pprs.PSmsb * (double)pprs.MORmsb) /
        ((double)pprs.MORlsb - (double)pprs.MORmsb);

        pprs.SIGMA = pprs.MORlsb * (pprs.PSlsb - pprs.CORR);
        calibration_successful=1;*/
     } 
     if(b[3]==0x55 && b[4]==0x20  && b[5]==0x48 &&  b[6]==0x49  && b[7]==0x47 && b[8]==0x48 && b[9]==0x20) //WR U HIGH 26000
      {
         uni_calibration_value.MORmsb = read_value_from_package_d(b,9);
         write123("MORmsb", (uni_calibration_value.MORmsb),3328);
         for(uint8_t i=0; i<100; i++){__no_operation();}           
     
      }
     if(b[3]==0x55 && b[4]==0x50 && b[5]==0x53 &&  b[6]==0x4D  && b[7]==0x20) //WR UPSm 13
     {
       uni_calibration_value.PSmsb = read_value_from_package_d(b,7);
       write123("PSmsb", (uni_calibration_value.PSmsb),3392);
       for(uint8_t i=0; i<100; i++){__no_operation();}         
      
      }
    
     if(b[3]==0x55 && b[4]==0x20  && b[5]==0x4C &&  b[6]==0x4F  && b[7]==0x57 && b[8]==0x20 ) //WR U LOW 2850
     {
       uni_calibration_value.MORlsb = read_value_from_package_d(b,8);
       write123("MORlsb", (uni_calibration_value.MORlsb),3456);
       for(uint8_t i=0; i<100; i++){__no_operation();}          
      
      }
     if(b[3]==0x55 && b[4]==0x50 && b[5]==0x53 &&  b[6]==0x4C  && b[7]==0x20) //WR UPSl 1400 
     {
       uni_calibration_value.PSlsb =  read_value_from_package_d(b,7);
       write123("PSlsb", (uni_calibration_value.PSlsb),3520);
       for(uint8_t i=0; i<100; i++){__no_operation();}         
      
      }
  //пороги погоды    
  if(b[3]==0x43 && b[4]==0x4C  && b[5]==0x45 &&  b[6]==0x41 &&  b[7]==0x52 && b[8]==0x20) //RD Clear
  {
    thresholds.clear=read_value_from_package(b, 8) ;
    write123("Clear",(thresholds.clear),640);
    
  }
  if(b[3]==0x4E && b[4]==0x4F  && b[5]==0x49 &&  b[6]==0x53 &&  b[7]==0x45 && b[8]==0x20) // RD Noise
  {
    thresholds.noise=read_value_from_package(b, 8) ;
    write123("Noise",(thresholds.noise),704);
  
  }
  if(b[3]==0x48 && b[4]==0x41 && b[5]==0x5A && b[6]==0x45 && b[7]==0x20) //RD Haze
  {
    thresholds.haze=read_value_from_package(b, 7) ;
    write123("Haze",(thresholds.haze),768);
    
  }
  if(b[3]==0x46 && b[4]==0x4F && b[5]==0x47 && b[6]==0x20) //RD Fog
  {
    thresholds.fog=read_value_from_package(b, 6) ;
      write123("Fog",(thresholds.fog),832);
     
  }

  if(b[3]==0x44 && b[4]==0x44 && b[5]==0x52 && b[6]==0x49 && b[7]==0x5A && b[8]==0x5A && b[9]==0x20) //RD Ddrizz
  { 
    thresholds.drizzle=read_value_from_package(b, 9) ;
    write123("Drizzle",(thresholds.drizzle),896);
   
  }

  if(b[3]==0x44 && b[4]==0x50 && b[5]==0x52 && b[6]==0x20) //RD Dpr
  {
    thresholds.precipitation=read_value_from_package(b, 6) ;
    write123("Precipitation",(thresholds.precipitation),960);
   
  } 
  
 // WR tcorr 0.
  if(b[3]==0x54 && b[4]==0x43 && b[5]==0x4F && b[6]==0x52 && b[7]==0x52 && b[8]==0x20)
  {
    //write inside correction
    thresholds.t_corr=read_value_from_package(b, 8) ;
    write123("t_corr",(thresholds.t_corr),1408);
 
  }
  
 //  WR OUT TCORR 0.
    if(b[3]==0x4F && b[4]==0x55 && b[5]==0x54 && b[6]==0x20 &&  b[7]==0x54 && b[8]==0x43 && b[9]==0x4F && b[10]==0x52 && b[11]==0x52 && b[12]==0x20)
  {
    //write outside correction
    thresholds.out_t_corr=read_value_from_package_d(b, 12) ;
    write123("out_t_corr",(thresholds.out_t_corr),3776);
  
  }
  
  if(b[3]==0x4C && b[4]==0x49  && b[5]==0x4D &&  b[6]==0x49 &&  b[7]==0x54 && b[8]==0x20)//RD limit
  { 
    thresholds.limitcipitation=read_value_from_package(b, 8) ;
    write123("Limit",(thresholds.limitcipitation),1024);
 
  }
  
  
  if(b[3]==0x53 && b[4]==0x4C  && b[5]==0x49 &&  b[6]==0x4D &&  b[7]==0x20)//RD SLim
  {
    thresholds.sumlimit=read_value_from_package(b, 7) ;
    write123("Sumlim",(thresholds.sumlimit),1088);
   
  }
  if(b[3]==0x53 && b[4]==0x4C  && b[5]==0x50 &&  b[6]==0x20) //RD SLP
  {
    thresholds.slp=read_value_from_package(b, 6) ;
    write123("SLP",(thresholds.slp),1152);
  
  }
 
    if(b[3]==0x4C && b[4]==0x49  && b[5]==0x4D &&  b[6]==0x53 &&  b[7]==0x4E && b[8]==0x4F && b[9]==0x57 && b[10]==0x20) //RD Limsnow
  {
    thresholds.limsnow=read_value_from_package_d(b, 10) ;
    write123("limsnow",(thresholds.limsnow),1280);  
  
  }
    if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x55 && b[8]==0x52 && b[9]==0x20) //RD mindur
  {
    thresholds.minduration=read_value_from_package_d(b, 9) ;
    write123("minduration",(thresholds.minduration),1600);
 
  }

   if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x45 && b[8]==0x52 && b[9]==0x49 && b[10]==0x56 && b[11]==0x20) //RD minder
  {
    thresholds.minderivative=read_value_from_package_d(b, 11) ;
    write123("minderivative",(thresholds.minderivative),1664) ;
 
  }
  if(b[3]==0x43 && b[4]==0x4C  && b[5]==0x45 &&  b[6]==0x41 &&  b[7]==0x52 && b[8]==0x50 && b[9]==0x20) //WR clearp
  {
    thresholds.clearp=read_value_from_package_d(b, 9) ;
    write123("clearp",(thresholds.clearp),1728);

  }
                                                                                              
   if(b[3]==0x42 && b[4]==0x4F  && b[5]==0x4F &&  b[6]==0x53 &&  b[7]==0x54 && b[8]==0x20 ) 
  {
    thresholds.l=read_value_from_package_d(b, 8) ;
    write123("L",(thresholds.l),1792);
 
  }
   if(b[3]==0x4D && b[4]==0x41  && b[5]==0x58 &&  b[6]==0x4D &&  b[7]==0x4F && b[8]==0x52 && b[9]==0x20 ) //WR maxMOR
   {
     limit_visibility = read_value_from_package(b, 9);
     if(limit_visibility > 2000)
     write123("limit_visibility", (limit_visibility),4096);
     else flag_realization = 0;
   }
     
   if(b[3]==0x4C && b[4]==0x53 && b[5]==0x48)////RD LSH
  {
    thresholds.limspeedhail=read_value_from_package_d(b, 6) ;
    write123("speed_hail",(15),3200);
  
   
  }
    if(b[3]==0x4D && b[4]==0x49  && b[5]==0x4E &&  b[6]==0x44 &&  b[7]==0x55 && b[8]==0x52 && b[9]==0x48 && b[10]==0x41) //RD mindurhail
  {
    thresholds.mindurationhail=read_value_from_package_d(b, 13) ;
    write123("duration_hail",(2),3264);
  }
  if(b[3]==0x43 && b[4]==0x53  && b[5]==0x20) //RD CS
  {
    pprs.scattering_coefficient=read_value_from_package_d(b, 5) ;
    write123("Scattering",(pprs.scattering_coefficient),2560);
    flag_realization=1;
  }
  if(find_subarray(b, "WR DELAY", 8))
  {
    del = read_value_from_package(b, 8) ; 
    write123("delay",(del), 4160);
  }
   if(find_subarray(b, "WR OUT DELAY", 12))
  {
    out_del = read_value_from_package(b, 12) ; 
    write123("delay",(out_del), 4224);
  }
  
  if(find_subarray(b, "WR N", 4))
  {
    amount_point = read_value_from_package(b, 4) ; 
    if (amount_point<0) amount_point = 0;
    if (amount_point>16) amount_point = 16;
    write123("amount_point",(amount_point), 4352);
  }
  
  if(find_subarray(b, "WR F", 4))
  {
    int in_v = 6;
    int index_point = b[4] - 0x30;   
    if(b[5]!=0x20)
      index_point = index_point *10 + b[5]- 0x30 - 1; 
    else 
    {
      index_point--;
      in_v --;
    }
    if(index_point>=0 && index_point < amount_point)
    {
        int MOR = read_value_from_package(b, in_v) ;       
        if(MOR >= 0.1 && MOR < 99000)
        {
          uint8_t buf[16];
          int memAddr = 5872 + 16 * index_point; 
          dataMOR[index_point] = MOR;
          write_value_in_buf(buf, dataMOR[index_point]);
          write( (uint8_t*) buf, memAddr, 15 );
          for(uint8_t i=0; i<100; i++){__no_operation();}
        } 
    }
  }
 
  if(find_subarray(b, "WR TH", 5))
  {
    Thigh = read_value_from_package_d(b, 5);
    write123("Thigh",( Thigh),4736); 
  }
   if(find_subarray(b, "WR TL", 5))
  {
    Tlow = read_value_from_package_d(b, 5);
    write123("Tlow",(Tlow),4800); 
  }
   
  if( flag_realization==1) 
     communicator("CMD OK!\n\r", 9);    
 
}
int console(uint8_t *console_buffer)
{ 
  flag_realization=0;
  history_write(console_buffer);
  switch(console_buffer[0])
  {
  case 0x41:
  {
    if(find_subarray(console_buffer,"ARRAY PS CLEAR\r",15))
    {
      /* double array[14];
      for(int i=0; i<amount_point; i++)
        array[i] = dataPS[i]-delta;*/
      double array[16] = {162500.0 , 6500.0 , 1185.0 , 530.0 , 190.0 , 105.0 , 80.0 , 18.0 , 10.5 , 10.0 , 8.5, 5.5 , 2.25, 2.05, 1.0, 0.1};
      memcpy( dataPS, &array, amount_point* sizeof( double));
      write_approximation_parameters("PS_array", array, 6128);
      flag_realization = 1;
        communicator("CMD OK!\n\r", 9);  
     
    }
   if(find_subarray(console_buffer,"ARRAY MOR CLEAR\r",16))
    {
      // double array[16] = {5, 20, 110, 230, 600, 980, 1250, 3210, 5700, 5730, 6280, 9730, 22360, 24720, 38960, 75000};
      double array[16] = {5, 20, 110, 230, 600, 980, 1250, 3210, 5090, 5730, 6280, 9730, 22360, 24720, 38960, 75000};
      memcpy( dataMOR, &array, amount_point * sizeof( double));
     //  write_block("MOR_array", dataMOR, amount_point, 5856);
       write_approximation_parameters("MOR_array", array, 5856);
       flag_realization = 1;
         communicator("CMD OK!\n\r", 9);    
    }
  }
  break;
  case 0x42:
    { //BAUDx //change speed //0-1200 1-2400 2-4800 3-9600 4-14400 5-19200 6-38400 7-57600
      if(find_subarray(console_buffer,"BAUD", 4)) 
      {
        change_speed(console_buffer); 
        if( flag_realization==1) 
           communicator("CMD OK!\n\r", 9);    
      }
    }
    break;

  case 0x43: //C
    { 
      if(console_buffer[2]==0x52)
      {
        if(find_subarray(console_buffer,"DPS\r", 4))
        {
          delta = 0;
          write123("delta",(delta), 4288);
          flag_realization=1;
          communicator("CMD OK!\rDO ReCAL!\r\n", 19);  
        }
        else        
        {
          setting_flag(console_buffer);    //type command CLR
          if( flag_realization==1) 
           communicator("CMD OK!\n\r", 9);  
        }
     
      }
      //else stop timer8 
      else
        //command close
        if(console_buffer[2]==0x4F &&  console_buffer[3]==0x53 && console_buffer[4]==0x45 && console_buffer[5]==0x0D)
        {
          stop_tim();
          flag_realization=1;
          communicator("LINE CLOSED\n\r", 13);
        }
    }
    break;
  case 0x44: //D
    {
      if(find_subarray(console_buffer, "DEFROST\r", 8))
      {
         flag_realization=1;
         if(flag_heating)
         {
           defrosting = 1;          
           communicator("CMD OK!\n\r", 9);
           HAL_TIM_Base_Start_IT(&htim15);    
           HAL_GPIO_WritePin(T_GPIO_Port, T_Pin, GPIO_PIN_SET);
         }  
         else
           communicator("HOOD HEATER DEACT\n\r", 19);
      }
    }
    break;
    case 0x48: //History
    { 
      if(console_buffer[1]==0x49 && console_buffer[2]==0x53 && console_buffer[3]==0x54 && console_buffer[4]==0x4F && console_buffer[5]==0x52 && console_buffer[6]==0x59)
      {
        flag_realization=1;
        history_read();
      }      
     
      if(find_subarray(console_buffer, "HEAT", 4))
      {
        flag_realization = run_of_heating(console_buffer);
      }   
    }
    break;
  case 0x4C:
    {     
       //light reset
       if(find_subarray(console_buffer, "LIGHT RESET\r",12))
      {       
        light_reset();
          communicator("CMD OK!\n\r", 9);   
        flag_realization=1;   
      }
     
    }
    break;
  case 0x4D: 
    {
        if(console_buffer[1]==0x45 && console_buffer[2]==0x53 && console_buffer[3]==0x53 && console_buffer[4]==0x41 && console_buffer[5]==0x47 && console_buffer[6]==0x45)
        {
          if(console_buffer[8]>=0x30 &&console_buffer[8]<=0x39)
          {
            message=console_buffer[8]-0x30;
            if(PWandFD)
            {
              if(message=0)
                limit_visibility=19900;
              else 
                limit_visibility=50000;              
            }
             int type=10+10*PWandFD+message;
             write123("type",(type),1984); 
             write123("limit_visibility", (limit_visibility), 4096);
               communicator("CMD OK!\n\r", 9);     
            flag_realization=1;   
          }     
        }      
    }
    break;
  case 0x50:
    {      
      //change period
       if(console_buffer[3]==0x0D)
       {
         //Period //P15
         sending_period = 1000*((console_buffer[1]-'0')*10+console_buffer[2]-'0');//Изменение периода выдачи информации, от 15 до 60 сек
         if(sending_period < 15000)sending_period=15000;
         if(sending_period > 60000)sending_period=60000;
         write123("period",(sending_period),2880);
           communicator("CMD OK!\n\r", 9);     
         flag_realization=1;
       }
      //settings port 
       if(console_buffer[1]==0x4F && console_buffer[2]==0x52 && console_buffer[3]==0x54) //port
       {
          HAL_UART_DeInit( &huart4 );          
          word_length=console_buffer[5]-0x30;
          if(console_buffer[6]==0x45)  
            huart4.Init.Parity = UART_PARITY_EVEN; 
          if(console_buffer[6]==0x4E)  
           huart4.Init.Parity = UART_PARITY_NONE;
     
          HAL_UART_Init( &huart4 );
          u4rlen=0xffff;
          u4_rcv_pkt_IT();
          write123("Parity",(word_length),2688);
            communicator("CMD OK!\n\r", 9);        
          flag_realization=1;
         
        break;          
       }
       
    }
    break;
 case 0x51:
    { //quick c //reset calibration
      if (console_buffer[1]==0x55 && console_buffer[2]==0x49 && console_buffer[3]==0x43 && console_buffer[4]==0x4B && console_buffer[5]==0x20 && console_buffer[6]==0x43 && console_buffer[7]==0x0D)
      {
          communicator("CMD OK!\n\r", 9);    
         flag_realization=1;
         calibration(0, 1);
      }    
    }
    break;
  case 0x52:
    {
      //чтение rs485
      //RD ADR
      if(console_buffer[1]==0x44 && console_buffer[2]==0x20 && console_buffer[3]==0x41 && console_buffer[4]==0x44 && console_buffer[5]==0x52 && console_buffer[6]==0x0D)
      { 
         flag_realization=1;
        char *str = new char[8];
        sprintf(str, "ADR:%-2d\r\n", rs_adr);
        communicator(str, 8);      
        free(str); 
        
        break;
      }
      //чтение ID //RD ID
      if(console_buffer[1]==0x44 && console_buffer[2]==0x20 && console_buffer[3]==0x49 && console_buffer[4]==0x44  && console_buffer[5]==0x0D)
      {
        HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET );
        
        ID= read_in_mem("id",(22626),3008);
        number=read_in_mem("num_board",(6040),3072);
        uint8_t ms_ID[11];
        int i=0;
        for(int dev=10000; i<5; i++, dev/=10)
          ms_ID[i] =((ID /dev) %10)+'0';
        for(int dev=1000; i<9; i++, dev/=10)
          ms_ID[i] = ((number /dev) %10)+'0';
        ms_ID[9] = 0x0A; 
        ms_ID[10] = 0x0D;
        HAL_UART_Transmit( &huart4, ms_ID, 11, 100);        

        HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_RESET );
        flag_realization=1;
        break;
      }
   if(find_subarray(console_buffer, "READ HEAT\r", 10))
   {
     char str[16] = {"HH:A \rHEAT:T  \r"};
     if(!flag_heating)
     {
       str[3]='D';
       str[3]='A';
     }
     if(heatingpresent == 1)
     {
       str[11]='O';
       str[12]='N';
     }
     if(heatingpresent == 0)
     {
       str[11] = 'O';
       str[12] = str[13] = 'F';     
     }
      communicator(str, 15);      
    // free(str);      
 
      flag_realization=1;
      break;
   }
      if(console_buffer[1]==0x44 && console_buffer[2]==0x20)
        read(console_buffer);
      if(console_buffer[1]==0x45 && console_buffer[2]==0x42 && console_buffer[3]==0x4F && console_buffer[4]==0x4F && console_buffer[5]==0x54 && console_buffer[6]==0x0D)
      {
         communicator("CMD OK!\n\r", 9);
         NVIC_SystemReset();
      }
       
       //reset //RESET
      if(console_buffer[1]==0x45 && console_buffer[2]==0x53 && console_buffer[3]==0x45 && console_buffer[4]==0x54 && console_buffer[5]==0x0D)
      {
        set_weather = 1;
        limit_visibility = 75000;       
        write123("weather", (set_weather), 2816);        
        write123("limit_visibility", (limit_visibility), 4096);
        
        light_reset();
       
        calibration(0, 1);
      //
        HAL_UART_DeInit( &huart4 );
        HAL_UART_DeInit( &huart5 );
     
        flag_peleng=1;             
        flag_vaisala=0;     
        flag_vaisala_fs=0;   
        flag_biral=0;
        
        huart4.Init.BaudRate = speed = 1200;
        huart5.Init.BaudRate = speed;
        word_length=8;       
        huart4.Init.Parity = UART_PARITY_NONE;
        huart5.Init.Parity = UART_PARITY_NONE;
        flag_request  = 0;  
        
        HAL_UART_Init( &huart4 );
        HAL_UART_Init( &huart5);
   
        u4rlen=0xffff;
        u4_rcv_pkt_IT();
        write123("flag_request",(flag_request),2944);
        write123("speed_in_memory",(speed),576);
        write123("Parity",(8),2688);
        write123("type",(0),1984);
       
        //
          communicator("CMD OK!\n\r", 9);      
        flag_realization=1;
      }
         
          
    }
    break;
 case 0x53: 
    { 
      //Пеленг //SET Peleng //Mode Peleng
      if(console_buffer[1]==0x45 && console_buffer[2]==0x54 && console_buffer[3]==0x20 && console_buffer[4]==0x50 && console_buffer[5]==0x45 && console_buffer[6]==0x4C)
     {  
        set_weather=1;
        limit_visibility=75000;
        flag_request  = 0;  
        write123("weather", (set_weather), 2816);
        write123("flag_request",(flag_request),2944);
        write123("limit_visibility", (limit_visibility), 4096);
        
        HAL_UART_DeInit( &huart4 );
        HAL_UART_DeInit( &huart5 );
     
        flag_peleng=1;             
        flag_vaisala=0;
        flag_vaisala_fs=0;  
        flag_biral=0;
       
       
        huart4.Init.BaudRate = speed = 1200;
        huart5.Init.BaudRate = speed;
        word_length=8;       
        huart4.Init.Parity = UART_PARITY_NONE;
        huart5.Init.Parity = UART_PARITY_NONE;
      
        
        HAL_UART_Init( &huart4 );
        HAL_UART_Init( &huart5 );
        u4rlen=0xffff;
        u4_rcv_pkt_IT();   
      
       
        write123("speed_in_memory",(speed),576);
        write123("Parity",(8),2688);
        write123("type",(0),1984);
        
        flag_realization=1;
          communicator("CMD OK!\n\r", 9);    
       
        break;
      }
      //Вайсала //SET Vaisala //Mode Vaisala
      if(console_buffer[1]==0x45 && console_buffer[2]==0x54 && console_buffer[3]==0x20 && console_buffer[4]==0x56 && console_buffer[5]==0x41 && console_buffer[6]==0x49 && console_buffer[7]==0x53 && console_buffer[8]==0x41 && console_buffer[9]==0x4C && console_buffer[10]==0x41 ) 
      {  
        flag_request = 1;
        write123("flag_request",(flag_request),2944);  
        communicator("CMD OK!\n\r", 9);     
        
        flag_peleng=0;       
        flag_biral=0;
        
        flag_vaisala=1;
        flag_vaisala_fs=0;  
        
        HAL_UART_DeInit( &huart4 );
        
        huart4.Init.BaudRate = speed = 9600;
        word_length=7;
        huart4.Init.Parity = UART_PARITY_EVEN; 
        message=2;
       
        if(console_buffer[12]==0x46 && console_buffer[13]==0x53)
        {
           set_weather=0;
           flag_vaisala=0;
           flag_vaisala_fs=1; 
            limit_visibility=50000;
        }
        
        
        if(console_buffer[12]==0x50 && console_buffer[13]==0x57)
        {
          PWandFD=0;        
          switch(console_buffer[15])
          {
             case 31:
               limit_visibility=2000;
               break;
             case 32:
               limit_visibility=20000;
               break;
             case 35:
               limit_visibility=50000;
               break;
          }
          if(console_buffer[16]==0x32)
            set_weather=1;
          if(console_buffer[16]==0x30)
            set_weather=0;
        }
         if(console_buffer[12]==0x46 && console_buffer[13]==0x44)
        {
          PWandFD=1;        
         
          if(console_buffer[16]==0x50)
            set_weather=1;
          limit_visibility=50000;
        }
           
        int type=10 + 10*PWandFD + message + 20*flag_vaisala_fs;
        HAL_UART_Init( &huart4 );

        write123("type",(type),1984);      
        write123("speed_in_memory",(speed),576);
        write123("Parity",(word_length),2688);
        write123("limit_visibility", (limit_visibility), 4096);
        write123("weather", (set_weather), 2816);   
        u4rlen=0xffff;
        u4_rcv_pkt_IT(); 
        flag_realization=1;
          communicator("CMD OK!\n\r", 9);    
        break;
      }
       //Бирал //SET Biral //Mode Biral
      if(console_buffer[1]==0x45 && console_buffer[2]==0x54 && console_buffer[3]==0x20 && console_buffer[4]==0x42 && console_buffer[5]==0x49 && console_buffer[6]==0x52) 
      { 
        HAL_UART_DeInit( &huart4 );
        HAL_UART_DeInit( &huart5 );
        flag_vaisala_fs=0;  
        flag_peleng=0;
        flag_vaisala=0;
        flag_biral=1;
        
        huart4.Init.BaudRate = speed = 1200;
        huart5.Init.BaudRate = speed;
        word_length=8;     
        huart4.Init.Parity = UART_PARITY_NONE;
        huart5.Init.Parity = UART_PARITY_NONE;
        flag_request  = 0; 
        
        HAL_UART_Init( &huart4 );
        HAL_UART_Init( &huart5 );
        u4rlen=0xffff;
        u4_rcv_pkt_IT();  
        write123("flag_request",(flag_request),2944);  
        write123("speed_in_memory",(speed),576);
        write123("Parity",(8),2688);
        write123("type",(5),1984); 
        
          communicator("CMD OK!\n\r", 9);    
        flag_realization=1;
        break;
      }
      
      setting_flag(console_buffer);
      if( flag_realization==1) 
          communicator("CMD OK!\n\r", 9);         
    }

    break;
  case 0x56: //letter V
    {
      if(find_subarray(console_buffer, "VRF MAS\r",8))
      {
        bool error = 0;
        if(dataPS[amount_point - 1] > 0.1) 
          error=1;
        else
          for(int i = 1; i < amount_point; i++)
            if(dataMOR[i-1] > dataMOR[i] || dataPS[i-1]<dataPS[i])
              error=1;
        
        if(error)        
          communicator("Mass ERR!\n\r",11);
        else
          communicator("Mass OK!\n\r",10);
        flag_realization =1;
      }
       if( find_subarray(console_buffer, "VERSION\r", 8)|| find_subarray(console_buffer, "VERS\r", 5))
      {  
        flag_realization=1;
        char *str = new char[14];
        sprintf(str, "SL-03P v%.2f\r\n", (float)version/100);
        communicator(str, 14);      
        free(str);   
      }
    }
    break;
  case 0x57: 
    {
      if(console_buffer[1]==0x52 && console_buffer[2]==0x20)
      {
         if(find_subarray(console_buffer, "WR DPS", 6))
        {
        /*  delta = pprs.PS - pprs.PS0;
          write123("delta",(delta), 4288);*/
          flag_realization=1; 
        
          HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET ); 
          HAL_UART_Transmit( &huart4, (uint8_t*)"Wait! The device measures the data for 1 minute!\r\n" ,50, 500);

           
          number_of_iteration = 0;
          flag_delayed_recording = 1;
          adr_temporary_value = &pprs.PS;
          break;
          
        }
        if(find_subarray(console_buffer, "WR PS", 5))
        {
          int index_point = read_value_from_package(console_buffer, 4) - 1; 
          if(index_point>=0 && index_point<amount_point)
          {
            if(index_point==0)
            {
              if(flag_calibration != 123)                
                communicator("Error! Do WR PS2.\r\n", 19);
              else
                communicator("PS1 has already been recorded.\r\n", 32);
              flag_realization = 1;
            }
            else
            {
               flag_realization = 1;
               HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET ); 
               HAL_UART_Transmit( &huart4, (uint8_t*)"Wait! The device measures the data for 1 minute!\r\n" ,50, 500);

               number_of_iteration = 0;
               flag_delayed_recording = 1;
               adr_temporary_value = &pprs.PS;
            }
           /*   uint8_t buf[16];
              int memAddr = 6144 + 16 * index_point; 
              if(index_point < amount_point - 1)
              {
                if(index_point == 0)
                {
                  Tref = termo_val;
                  write123("Tref",(Tref),3840); 
                  PLref = pprs.PL;
                  write123("PLref",(PLref),4480); 
                }
                dataPS[index_point] =  pop(stk_lvlPS, PSstack);
              }
              else 
                dataPS[index_point] = 0.1;
              
              write_value_in_buf(buf, dataPS[index_point]);
              write( (uint8_t*) buf, memAddr, 15 );
               
              for(uint8_t i=0; i<100; i++){__no_operation();}*/
          }
          else 
          {
            communicator("Incorrected indexing. ", 22);
            flag_realization = 0;
          }
          break;
        }
          if(find_subarray(ibuf, "WR CS\r", 6)) //WR CS
          {
            flag_realization = 1;    
             HAL_GPIO_WritePin( RDE_GPIO_Port, RDE_Pin, GPIO_PIN_SET ); 
             HAL_UART_Transmit( &huart4, (uint8_t*)"Wait! The device measures the data for 1 minute!\r\n" ,50, 500);

             number_of_iteration = 0;
             flag_delayed_recording = 1;
             adr_temporary_value = &pprs.PSmomentary;
           /* double value = 1400 / pop(stk_lvlPS, PSstack);
            if(value >= 0.3 && value <= 2)
            {
              pprs.scattering_coefficient = value;
              write123("Scattering",(pprs.scattering_coefficient),2560);  
            }
            else 
              communicator("CS BAD\n\r", 8);*/
             break;
          }
        write(console_buffer);
      }
    }
    break;
  default:    
      flag_realization=0;      
   
    break;
  
  }
  if(flag_realization==0) 
  {
    for( int i=0; i<21; i++)
      console_buffer[i]=0;
    communicator("CMD ERR?\n\r", 10);
  }
  else
    c_t_15 = 0;
    
 // history_write(console_buffer);
  
  huart4.RxXferCount=256;
  HAL_UART_AbortReceive(&huart4);
 
  return 0;
}

HAL_StatusTypeDef hstd1;
int wait_command(uint8_t *console_buffer)
{
  if(!flag_delayed_recording)
  {
    flag_realization=0;  
    huart4.RxXferCount = 256;
    return console(console_buffer); 
  }
}

double  PSx_stack[4] = {NAN, NAN, NAN, NAN};
uint8_t stkidxPSx   = 0;
uint8_t stk_lvlPSx  = 0;

void delayed_recording()
{  
  int size;
   if(flag_console && flag_delayed_recording)
   {
     push(stk_lvlPSx, 4, stkidxPSx, *adr_temporary_value, PSx_stack);
     if(number_of_iteration==5)
     {
       flag_delayed_recording = 0;
       
       char *str = new char[60]; 
       double value1 = pop(4, PSx_stack); 
      
       if(find_subarray(ibuf, "RD PS", 5))
       { 
         double value2 = pop(4, PS_0_stack);       
         double value3 = (double) 400/xcoeff *(value1 - value2);         
         value3 -= delta;
         if(flag_cs) 
            value3 *= pprs.scattering_coefficient;
         
         size = 41 + bit_depth(value1)+bit_depth(value2)+bit_depth(value3);
         sprintf(str, "PSFull = %.3f\rPS0 = %.3f\rPSMoment = %.3f\r",  value1,  value2,  value3);
         //  sprintf(str, "mor%-2d %6ld\tps%-2d %.3f\r", i+1, mor, i+1, ps);
         communicator(str, size);    
         HAL_Delay(300);       
       } 
       
        if(find_subarray(ibuf, "WR DPS", 5))
       {             
         double value2 = pop(4, PS_0_stack);       
         double value3 = value1 - value2;   
         size = 49 + bit_depth(value1)+bit_depth(value2)+bit_depth(value3);
         delta = value3;
         write123("delta",(value3), 4288); 
         sprintf(str, "PSFull = %.3f\rPS0 = %.3f\rdelta PS = %.3f\rCMD OK!\r", value1,  value2, value3);       
         communicator(str, size);    
         HAL_Delay(300);       
       }   
       
       if(find_subarray(ibuf, "WR PS", 5))
       {
          int index_point = read_value_from_package(ibuf, 4) - 1; 
          double value2 = pop(4, PS_0_stack); 
          
          uint8_t buf[16];
          int memAddr = 6144 + 16 * (index_point ); 
          if(index_point < amount_point - 1)
          {
            if(index_point == 1)
            {
              Tref = termo_val;
              write123("Tref",(Tref),3968); 
              PLref = pprs.PL;
              write123("PLref",(PLref),4480); 
              dataPS[index_point] = (double) 400/xcoeff *(value1 - value2);
              dataPS[index_point]-= delta;
              
              if(flag_cs) 
                dataPS[index_point] *= pprs.scattering_coefficient;
              
              dataPS[0] = 25.0 * dataPS[index_point];
              flag_calibration = 123;
              write123("calib",(flag_calibration),128);
             
              write_value_in_buf(buf, dataPS[0]);
              write( (uint8_t*) buf, memAddr - 16, 15 );
              write_value_in_buf(buf, dataPS[1]);
              write( (uint8_t*) buf, memAddr, 15 );
              
            }
            else
            {
              dataPS[index_point] = (double) 400/xcoeff *(value1 - value2);
              dataPS[index_point]-= delta;
              if(flag_cs) 
                dataPS[index_point] *= pprs.scattering_coefficient;
              write_value_in_buf(buf, dataPS[index_point]);
              write( (uint8_t*) buf, memAddr, 15 );
            }
            
            
          }
          else 
          {
            dataPS[index_point] = 0.1;
            write_value_in_buf(buf, dataPS[index_point]);
            write( (uint8_t*) buf, memAddr, 15 );
          }
          communicator("CMD OK!\n\r", 9);   
         
          for(uint8_t i=0; i<100; i++){__no_operation();}
       }
       if(find_subarray(ibuf, "WR CS", 5))
       {
          double value = 1180 / value1;
          if(value >= 0.3 && value <= 2)
          {
            pprs.scattering_coefficient = value;
            write123("Scattering",(pprs.scattering_coefficient),2560);
            size = 17 + bit_depth(value);
            sprintf(str, "CS OK!CS = %.3f\n\r", value);
            communicator(str, size); 
            if(write123("CS",(1),4672))
            {
              flag_cs = 1; //SET CS /CLR CS
            }   
          }
          else 
            communicator("CS BAD\n\r", 8);
       }
       
       
       
       free(str);
     }
     else
       number_of_iteration++;
        
   }
}