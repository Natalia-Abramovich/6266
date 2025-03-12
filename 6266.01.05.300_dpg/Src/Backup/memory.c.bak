#include "main.h"

#include "i2c.h"
#include <math.h>
#include "approximation.h"
//#include "stm32f3xx_hal.h"

//#include <stdio.h>

//#define sEE_HW_ADDRESS         0xA0   /* E0 = E1 = E2 = 0 */
uint8_t devAddr = (0x50 << 1);  

//template <typename T>  int write_block(char const *, T*, int, int );

uint16_t write( uint8_t *from_mem_ptr, uint32_t memAddr_value, uint32_t ee_len )
{
  HAL_StatusTypeDef hstd;
   
  int countsend=0;
   
  hstd= HAL_I2C_IsDeviceReady( &hi2c2, devAddr, 30, 16 );
  if ( hstd != HAL_OK )
  {
    while(countsend<100)
    {
       countsend++;
      hstd = HAL_I2C_IsDeviceReady( &hi2c2, devAddr, 30, 16 );
      if(HAL_OK==hstd)
        break;   
    }
  }
  hstd = HAL_I2C_Mem_Write(&hi2c2, devAddr, memAddr_value, I2C_MEMADD_SIZE_16BIT,
        from_mem_ptr,16,100);
   countsend=0;
   if(HAL_OK != hstd)
   while (countsend<100)
   {  
      countsend++;
       hstd = HAL_I2C_Mem_Write(&hi2c2, devAddr, memAddr_value, I2C_MEMADD_SIZE_16BIT,
        from_mem_ptr, sizeof(from_mem_ptr),100);
       if(HAL_OK == hstd)
         break; 
    }
 
  if ( hstd == HAL_OK )
  {
    return 1;
  }
  else
    return 0;
}

void write_value_in_buf(uint8_t* buf, double write_value)
{
   int i=0;
   if(write_value<0)
   {
     buf[i]=0x2D;
     write_value*=(-1);
     i++;
   }
   for(int j=pow(10,6-i); i<7; j/=10,i++)
   {      
    buf[i]= (int)(write_value / j) % 10+'0'; 
   }

   buf[i++]=0x2E;
   
   int rd=(int)((write_value - (int)write_value)*100000000);
   for(int j=10000000; i<16; j/=10,i++)
   {      
    buf[i]= (int)(rd / j) % 10+'0'; 
   }
     
}

int write123(const char write_name[], double write_value, uint16_t memAddr) 
{   
uint8_t buf[16];
  uint16_t memAddr_value = memAddr+32;
  
  write_value_in_buf(buf,write_value);
     if(!write( (uint8_t*)write_name, memAddr,  sizeof(write_name)))
       return 0;
    for(uint8_t i=0; i<100; i++){__no_operation();}
     if(!write( (uint8_t*) buf,memAddr_value, 15 ))
       return 0;
    for(uint8_t i=0; i<100; i++){__no_operation();}
    

   /*  hstd = HAL_I2C_Mem_Write(&hi2c1, devAddr, memAddr, I2C_MEMADD_SIZE_16BIT,
        (uint8_t*)write_name, sizeof(write_name), 500);
   
    if ( HAL_OK != hstd )
    {     
      hstd = HAL_I2C_Mem_Write(&hi2c1, devAddr, memAddr, I2C_MEMADD_SIZE_16BIT,
        (uint8_t*)write_name, sizeof(write_name),500);
       
    }*/
    return 1;
    
}

uint8_t read( uint8_t *from_mem_ptr, uint32_t memAddr_value )
{
  HAL_StatusTypeDef hstd;
   
  int countsend=0;
   
  hstd= HAL_I2C_IsDeviceReady( &hi2c1, devAddr, 30, 16 );
  if ( hstd != HAL_OK )
  {
    while(countsend<100)
    {
       countsend++;
      hstd = HAL_I2C_IsDeviceReady( &hi2c2, devAddr, 30, 16 );
      if(HAL_OK==hstd)
        break;   
    }
  } 
  hstd = HAL_I2C_Mem_Read(&hi2c2, devAddr, memAddr_value, I2C_MEMADD_SIZE_16BIT,
       from_mem_ptr,16, HAL_MAX_DELAY);
   countsend=0;
   if(HAL_OK != hstd)
   while (countsend<100)
   {  
      countsend++;
      hstd = HAL_I2C_Mem_Read(&hi2c2, devAddr, memAddr_value, I2C_MEMADD_SIZE_16BIT,
       from_mem_ptr,16, HAL_MAX_DELAY);
       if(HAL_OK == hstd)
         break; 
    }
   
   if ( hstd == HAL_OK )
  {
    return 1;
  }
  else
    return 0;
}

double reading_value( uint32_t memAddr_value, double value)
{
  uint8_t read_value[16];
  double rezalt = 0;
  if ( !read( read_value, memAddr_value)) 
   {
      return (value);
   } 
   else       
   {    
      for(int index =0; index<14; index++)
      if(read_value[index]>0x40)
        return (value);      
   
      int i=0;
      if(read_value[i]==0x2D)      
          i++;
     
      for(; i<7; i++)
      {
        rezalt = rezalt*10+(read_value[i]-'0') ; 
      }
      i++;

      int x=0;
      for(; i<14; i++)
      {
        x = x*10+(read_value[i]-'0') ; 
      }
      double y=(double)(x);
      rezalt += y* 0.000001; 

      if(read_value[0]==0x2D)
      {       
        rezalt*=(-1);
      }      
    
    } 
  return rezalt;
}

double read_in_mem(const char name[], double  value, uint16_t memAddr) 
{   
 
   char read_name[sizeof(name)];
   
    double rezalt=0;
    // HAL expects address to be shifted one bit to the left
    //uint16_t devAddr = (0x50 << 1);   
    uint16_t memAddr_value = memAddr+32;    
      
    read( (uint8_t*)read_name, memAddr);
   //  sEE_Read((uint8_t*)read_name, memAddr,  sizeof(read_name));
    HAL_Delay(25);
     for(uint8_t index=0; index<50; index++){__no_operation();}   
    if(read_name[0]==name[0] && read_name[1]==name[1] && read_name[2]==name[2] ) 
    {  
       rezalt = reading_value(memAddr_value, value);
    }
    else 
    {
      // write123(name, value, memAddr);
        rezalt=(value);     
    }
    return rezalt;//Math.Round(rezalt, 8);
}

template <typename T> int write_block (char const* write_name, T* write_value_array, int size_block, int memAddr)
{  
  uint8_t buf[16];
  if(!write( (uint8_t*)write_name, memAddr, 15))
     return 0;
  for(uint8_t i=0; i<100; i++){__no_operation();}
  
  for(int index = 0; index < size_block; index++)
  {
    memAddr = memAddr+16; 
    write_value_in_buf(buf, write_value_array[index]);
    if(!write( (uint8_t*) buf, memAddr, 15 ))
       return 0;
    for(uint8_t i=0; i<100; i++){__no_operation();}
   }
   return 1;    
}

int read_block( const char * name, int memAddr_value, int size_block, uint16_t* data)
{
  uint8_t read_name[16];
  double test[16];
  read( read_name, memAddr_value);
  if(read_name[0]==name[0] && read_name[1]==name[1] && read_name[2]== name[2])
  {
     for (int i = 0; i < size_block; i++)
    { 
      memAddr_value +=16;         
      data[i] = reading_value( memAddr_value, data[i]);                     
    }
  }
  else  
    write_block(name, data, size_block, memAddr_value );
 // write_block(name, test, size_block, memAddr_value);
  
}


void read_approximation_parameters()
{  
//  flag_approximation = read_in_mem("fl_app",(1),4352);
  amount_point = read_in_mem("amount_point",(16),4352);
  delta = read_in_mem("delta",(0),4288);
  flag_calibration = read_in_mem("calib",(0),128);
//  dataSigma[13];
  if( amount_point>0)
  {     
    uint8_t read_name[16];
    int memAddr_value = 5856;
    
    read( read_name, memAddr_value);
    if(read_name[0]=='M' && read_name[1]=='O' && read_name[2]=='R')
    {
       for (int i = 0; i < amount_point; i++)
      { 
        memAddr_value +=16;         
        dataMOR[i] = reading_value( memAddr_value, dataMOR[i]);                     
      }
    }
    else{
     write_block("MOR_array", dataMOR, amount_point, 5856); //4480);
    }

     memAddr_value = 6128;
     read(read_name, memAddr_value);
   
    if(read_name[0]=='P' && read_name[1]=='S' && read_name[2]=='_' )
    {
        for (int i = 0; i < amount_point; i++)
      {         
        memAddr_value +=16;
        dataPS[i] = reading_value( memAddr_value, dataPS[i]);                     
      }
    }
     else
     {  
       write_block("PS_array",  dataPS, amount_point, 6128);// 4736);
     }
  }

}