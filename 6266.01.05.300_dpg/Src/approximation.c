#include "approximation.h"
#include "main.h"

//double kx = 0.167;
bool flag_approximation = 1;
int amount_point = 16;
double delta = 0;
int flag_calibration = 0;

double dataMOR[16] = {5, 20, 110, 230, 600, 980, 1250, 3210, 5090, 5730, 6280, 9730, 22360, 24720, 38960, 75000};
double dataPS[16] = {162500.0 , 6500.0 , 1185.0 , 530.0 , 190.0 , 105.0 , 80.0 , 18.0 , 10.5 , 10.0 , 8.5, 5.5 , 2.25, 2.05, 1.0, 0.1};

int write_approximation_parameters(char write_name[], double write_value_array[], int memAddr)
{  
  uint8_t buf[16];
  if(!write( (uint8_t*)write_name, memAddr, 15))
     return 0;
  for(uint8_t i=0; i<100; i++){__no_operation();}
  
  for(int index = 0; index < amount_point; index++)
  {
    memAddr = memAddr+16; 
    write_value_in_buf(buf, write_value_array[index]);
    if(!write( (uint8_t*) buf, memAddr, 15 ))
       return 0;
    for(uint8_t i=0; i<100; i++){__no_operation();}
   }
   return 1;    
}

//void read_approximation_parameters()
//{
//  
//  flag_approximation = read_in_mem("fl_app",(1),4352);
//  amount_point = read_in_mem("amount_point",(14),0);
//  delta = read_in_mem("delta",(0),4288);
////  dataSigma[13];
//  if(flag_approximation && amount_point>0)
//  {  
//    uint16_t devAddr = (0x50 << 1);  
//    uint8_t read_value[16];
//    uint8_t read_name[16];
//    int memAddr_value = 5856;
//    
//    read( read_name, memAddr_value);
//    if(read_name[0]=='M' && read_name[1]=='O' && read_name[2]=='R')
//    {
//       for (int i = 0; i < amount_point; i++)
//      { 
//        memAddr_value +=16;         
//        dataMOR[i] = reading_value(read_value, memAddr_value, dataMOR[i]);                     
//      }
//    }
//    else
//      write_approximation_parameters("MOR_array", dataMOR, 5856); //4480);
////  dataPS[13];
//     memAddr_value = 6128;
//     read(read_name, memAddr_value);
//   
//    if(read_name[0]=='P' && read_name[1]=='S' && read_name[2]=='_' )
//    {
//        for (int i = 0; i < amount_point; i++)
//      {         
//        memAddr_value +=16;
//        dataPS[i] = reading_value(read_value, memAddr_value, dataPS[i]);                     
//      }
//    }
//     else
//      write_approximation_parameters("PS_array", dataPS, 6128);// 4736);
//  }
//
//}

double calculate_visibility( double PS )
{
  double MOR = 0;
    if (PS < dataPS[0])
        {
          if (PS < 0.1)  // maximum visibility          
          {
            MOR = 75000; 
            PS = 0.1;
          }
          else
          {          
            if (PS < dataPS[amount_point-1]) // out of array range 
              MOR = dataMOR[amount_point-1];        
            else //main algorithm approximation
            {
              int currentN = 0;
              for (int i = 0; i < amount_point; i++)	//find point
                if(PS < dataPS[i])
                  currentN = i;	
              double dSigma = 3.0/dataMOR[amount_point-1];
              double deltaPSMOM = dataPS[currentN] - PS;	        //differense PS from the nearest fixed point with a lower index
              double deltaRangePS = dataPS[currentN] - dataPS[currentN+1];          //length of the segment between points
              double deltaRangeSigma = (double)3.0/dataMOR[currentN] - (double)3.0/dataMOR[currentN+1]; //differense Sigma from the nearest fixed points
              double deltaPercent = deltaPSMOM/deltaRangePS;		                //?? ????? ????? ????????? ????????
              double sigmaMOM = (double)3.0/dataMOR[currentN] - deltaRangeSigma*deltaPercent;
              MOR = 3.0 / sigmaMOM;
            }
          }   
        }
        else
          MOR = dataMOR[0];  
    return MOR;
}