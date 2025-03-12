#include "main.h"
#include "usart.h"
#include <cstring>

int find_subarray( uint8_t *buffer, char *command, int size_command)
{
  int size_buffer = 22;// sizeof(buffer)/sizeof(uint8_t);
 // int size_command = sizeof(command)/sizeof(char);
  if(size_buffer >= size_command)
  for (int i = 0; i <= size_buffer - size_command; i++) 
  {
    if (memcmp(buffer + i, command, size_command ) == 0)
      return 1;
  }
  return 0;
}


void request_vaisala(uint8_t* ibuf, uint8_t* ms_adr, uint8_t size)
{
  
  //  HAL_Delay(40);

    request = 0;
    int index = 0;
    for(int scan=0; scan < size; scan++)
      if(ibuf[scan]==0x50 || ibuf[scan]==0x70 || ibuf[scan]==0x46 || ibuf[scan]==0x66)
      {       
        ms_adr[0]=ibuf[scan];
             
        index = (scan+1) < size ? (scan+1):0;
        if(ibuf[index]==0x57 || ibuf[index]==0x77 || ibuf[index]==0x44 || ibuf[index]==0x64)
        {        
          ms_adr[1] = ibuf[index];
          if(rs_adr==0)
            if(ibuf[index+1]==0x0D)
            {
               request=1;
               break;
            }
          index=(index+3)<size ? (index+3):index+3-size;
          {                  
             ms_adr[2] = ((rs_adr / 10) > 0) ? (rs_adr / 10 + '0') : (' ');
             ms_adr[3]= rs_adr % 10 + '0';
             if((ibuf[index-1]==ms_adr[2] && ibuf[index]==ms_adr[3])
                ||(ibuf[index-2]== ms_adr[2] && ibuf[index-1]==ms_adr[3]))
            // if( ibuf[scan]== 0x39)
              index=(index+2)<size ? (index+2):index+2-size;
            if(ibuf[index]==(0x30 + message)|| ibuf[index-1]==(0x30 + message))
            {
              request=1;
              break;
            }   
          }
        }     
      }
}

void request_peleng(uint8_t* ibuf)
{
  HAL_Delay(60);


  uint32_t sum = 0ul;
  if(ibuf[0]==0x20 && ibuf[14]==0x0D )
  {
    for(int i=7;i<12;i++)
      sum=sum*10+ibuf[i]-0x30;
    if(sum==42405)
      request=1;
   }
}
void handler()
{
}
