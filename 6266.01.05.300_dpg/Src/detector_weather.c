#include <vector>
#include <numeric>
#include <algorithm>

//
#include "data.h"
#include "push_pop.h"
#include "main.h"
#include "detector_weather.h"
#include "stack.h"
#include "intensity.h"


//template<class T>

uint16_t maxscattering;
uint16_t maxindexscattering;
uint16_t minscattering;
uint16_t minindexscattering;
uint16_t maxminamplitude;

uint32_t average; 
bool bithazeorfog = false;
bool bitdrizzle = false;
bool bitprecipitation = false;
bool bitfog1 = false;
bool bitfog2 = false;

uint16_t  pheno_coll [9] = {0,0,0,0,0,0,0,0,0};
uint8_t byteweather = 0;
MyStack <uint8_t, 8> clear;
MyStack <uint8_t, 8> haze;
MyStack <uint8_t, 8> fog;
MyStack <uint8_t, 8> precipitation;
MyStack <uint8_t, 8> drizzle;
MyStack <uint8_t, 8> rain;
MyStack <uint8_t, 8> snow;
MyStack <uint8_t, 8> rain_and_snow;
MyStack <uint8_t, 8> hail;

MyStack <uint8_t, 8>* minute2[] = {&clear, &haze, &fog, &precipitation, &drizzle, &hail, &rain_and_snow, &rain, &snow };

uint8_t count_minute=0;
uint8_t count_hour=0;

MyStack <uint8_t, 5> clear_m10;
MyStack <uint8_t, 5> haze_m10;
MyStack <uint8_t, 5> fog_m10;
MyStack <uint8_t, 5> precipitation_m10;
MyStack <uint8_t, 5> drizzle_m10;
MyStack <uint8_t, 5> rain_m10;
MyStack <uint8_t, 5> snow_m10;
MyStack <uint8_t, 5> rain_and_snow_m10;
MyStack <uint8_t, 5> hail_m10;

MyStack <uint8_t, 5>* minute10[] = {&clear_m10, &haze_m10, &fog_m10, &precipitation_m10, &drizzle_m10, &hail_m10, &rain_and_snow_m10, &rain_m10, &snow_m10};

MyStack <uint8_t, 6> clear_hour;
MyStack <uint8_t, 6> haze_hour;
MyStack <uint8_t, 6> fog_hour;
MyStack <uint8_t, 6> precipitation_hour;
MyStack <uint8_t, 6> drizzle_hour;
MyStack <uint8_t, 6> rain_hour;
MyStack <uint8_t, 6> snow_hour;
MyStack <uint8_t, 6> rain_and_snow_hour;
MyStack <uint8_t, 6> hail_hour;

MyStack <uint8_t, 6>* hour[] = {&clear_hour, &haze_hour, &fog_hour, &precipitation_hour, &drizzle_hour, &hail_hour, &rain_and_snow_hour, &rain_hour, &snow_hour};


uint8_t count_drizzle; 
uint8_t count_precipitation;

int X=2;

struct Thresholds__ thresholds;



#define STOPPER 0                                      /* Smaller than any datum */
//#define    MEDIAN_FILTER_SIZE    (10)                  //filter change 5
int const size_array = 9120;

const uint16_t size_window = 5;
int size_array_converted = size_array - size_window;
uint16_t ClassicMedianFilter(uint16_t* ptrArray)
{
    uint16_t returnValue;

    if (size_window > 1u)
    {
        //selection sort method
        for (int i = 0; i < size_window / 2 + 1; i++)
        {
            int index_min = i;
            for (int j = i; j < size_window; j++)
            {
                if (ptrArray[index_min] > ptrArray[j])
                    index_min = j;
            }
            int k = ptrArray[i];
            ptrArray[i] = ptrArray[index_min];
            ptrArray[index_min] = k;

        }
   
    //return result value
        if (0 == (size_window & 1u))
        {// Even number 
            returnValue = (ptrArray[(size_window / 2) - 1] + ptrArray[size_window / 2]) / 2;
        }
        else
        {// Odd number
            returnValue = ptrArray[(size_window - 1) / 2];
        }
    }
    else
    {// Array has only one element
        returnValue = ptrArray[0];
    }
    return returnValue;
}


void maxmin_scattering(uint16_t* fscattering)
{                                                                 // находим минимальное значение сигнала прямого рассеяния minscattering и находим среднее арифметическое значение
   maxscattering = 0;
   for ( int i = 0; i < size_array_converted; i++ )
   {
      if ( fscattering[ i ] > maxscattering )
      {
        maxscattering = fscattering[ i ];
        maxindexscattering = i;
      }
   }
   minscattering = 65535;
   average=0;
  
   for ( int i = 2; i < size_array_converted; i++ )
   {
     if ( fscattering[ i ] < minscattering )
     {
       minscattering = fscattering[ i ];
       minindexscattering = i;
     }
     average += fscattering[ i ];
   }
   average/= COUNT_MEASURES;
   maxminamplitude = maxscattering - minscattering;
}
void flag_weather( uint16_t* fscattering, float temperature)
{
 /* uint16_t thresholds.clear=150; //150
  uint16_t noisethreshold=75;
  uint16_t hazethreshold=50;
  uint16_t fogthreshold=400;
   uint16_t diffdrizzlethreshold=200;     //125
  uint16_t diffprecipitationthreshold=300;*/

  if (minscattering > (thresholds.clear - thresholds.noise + thresholds.haze ))     //����������� �������� > ������ ����� - ���� + ����� �����
  {
    bithazeorfog = true;      //���� �����
  }
  if (minscattering > (thresholds.clear - thresholds.noise + thresholds.fog) )    //����������� �������� > ������ ����� - ���� + ����� ������
  {
    bitfog1 = true;      //���� �����1
  }
  if (average > (thresholds.clear + thresholds.fog))     //������� �������� > ������ ����� + ����� ������
  {
    bitfog2 = true;        //���� ����� 2
  }

  uint16_t diffscattering=0;
 
  count_drizzle=0; count_precipitation=0;
  
  for ( int i = 10; i < size_array_converted; i++ )
  { 
    if(fscattering[i] > fscattering[i-4]) 
      diffscattering = fscattering[i] - fscattering[i-4];       //diffscattering[n] - ����� �����������������
    else 
      diffscattering=0;
    if ( thresholds.precipitation > diffscattering)
      if (diffscattering >  thresholds.drizzle )   //���� ���� ����� > ������   
      {
        bitdrizzle = true;
        count_drizzle++;
      }
    if(diffscattering > thresholds.precipitation )   
    {
      count_precipitation++;
      bitprecipitation = true;                                  //bitprecipitation = 1;  //���� ������ //���� "������"
    }
   }
   if(count_drizzle < 2) bitdrizzle = 0; 
   if(count_precipitation < 2) bitprecipitation = false;
}



void clear_vectors()
{
 for(int i = 0; i < 9; i++)
  {
    minute2[i]->push(0);
  } 
}

void clear_vectors_vaisala()
{
  for(int i = 0; i < 9; i++)
  {
    minute10[i]->push(0);
  } 
}
 

void clear_vectors_vaisala_hour()
{
for(int i = 0; i < 9; i++)
  {
    hour[i]->push(0);
  }  
}



//uint8_t const phenocodes[] = { 0x00, 0x04, 0x30, 0x40, 0x50, 0x60, 0x68, 0x70, 0x89};
uint8_t vote_pheno_vaisala_hour(uint8_t index)
{ 
 for(int i = 0; i < 9; i++)
  {
    pheno_coll[i] = hour[i]->sum();
  }
 
  int max_value= pheno_coll[0];
  int index_hour =0;
  
  for (int i=0;i<=8; i++)
  {
    if(pheno_coll[i] >= max_value)
    {
      index_hour=i;
      max_value = pheno_coll[i];
    }
   }  
     if (max_value==0)
    index_hour=index;
  return index_hour; 

}

uint8_t vote_pheno_vaisala(uint8_t index)
{  
  for(int i = 0; i < 9; i++)
  {
    pheno_coll[i] = minute10[i]->sum();
  }
    
  int max_value= pheno_coll[0];
  int index_minute =0;
  for (int i=0;i<=8; i++)
  {
    if(pheno_coll[i] >= max_value)
    {
      index_minute=i;
      max_value = pheno_coll[i];
    }
   }  
    if (max_value==0)
    index_minute=index;
  return index_minute;
}

uint8_t filling_pheno_vaisala_hour(uint8_t index)
{
  count_hour+=1;
  clear_vectors_vaisala_hour();
  hour[index] -> rewrite_last(1);
  count_minute=0;  
}

uint8_t filling_pheno_vaisala(uint8_t index)
{
  count_hour+=1;
  clear_vectors_vaisala();
  minute10[index] -> rewrite_last(1);
  count_minute=0;  
}

uint8_t vote_pheno(void)
{
   for(int k = 0; k < 9; k++)
  {
    pheno_coll[k] = minute2[k]->sum();
  }

  int max_value = pheno_coll[8];
  int index =8;
  int i=7;
  for ( ;i>=5; i--)
  {
    if(pheno_coll[i] > max_value)
    {
      index=i;
      max_value = pheno_coll[i];
    }
  }  

  if(max_value > thresholds.sumlimit) 
  { 
    return index;
  } 
  if(pheno_coll[4]> thresholds.sumlimit) 
  { 
    return 4;
  } 
  if(pheno_coll[3]> thresholds.sumlimit) 
  { 
    return 3;
  } 
  
  max_value=pheno_coll[2];
  index = 2;
  i=1;
  for ( ;i>=0; i--)
  {
    if(pheno_coll[i] > max_value)
    {
      index=i;
      max_value = pheno_coll[i];
    }
  }  
  if(max_value > thresholds.sumlimit) 
  { 
    return index;
  } 
  else return 0;
}   

void push_profile(uint8_t &stk_lvl, uint8_t max_stk_lvl, uint8_t &stk_idx, int16_t value, int16_t* stack)
{
 stack[stk_idx] = value;
   stk_idx = (stk_idx + 1) % max_stk_lvl;
  if (stk_lvl < max_stk_lvl)
  {
    stk_lvl++;
  }  
}

void filling_massiv(uint16_t* fscattering)     //медианный фильтр с окном 5//10       
{   
  int16_t  profile_stack[3]={0,0,0};//size_window / 2

  uint8_t stkidxprofile = 0;
  uint8_t stk_lvlprofile = 0;

  uint16_t median_filter_array[5] = {0};
 
    for (int index_profile = 0; index_profile < size_array-size_window; index_profile++)
    {
        push_profile(stk_lvlprofile, size_window/2, stkidxprofile, profile[index_profile], profile_stack);
        for (int index_median_ms = 0; index_median_ms < size_window; index_median_ms++)
        {
            if (index_median_ms < size_window  / 2+1)
            {
              /*  if ((index_profile + index_median_ms - size_window / 2) < 0 )
                    median_filter_array[index_median_ms] = 0;
                else*/
                    median_filter_array[index_median_ms] = profile_stack[index_median_ms];
            }
           else
            { 
                if ((index_profile + index_median_ms - size_window / 2 )>= size_array)
                    median_filter_array[index_median_ms] = 0;
                else
                    median_filter_array[index_median_ms] = profile[index_profile + index_median_ms - size_window / 2];
            }
        }
         profile[index_profile] = ClassicMedianFilter(median_filter_array);
         fscattering[index_profile] = profile[index_profile];
    }
  
}

//переделать, мне не нравится
//!!!!!!!!!!!!!!!!!!!!!!
int amount_peaks1=0;
int sum_peaks=0;
double sum_speeds = 0.0;
int amount_snow = 0;
int amount_rain=0;

//double array_speed[40] = { 0 };//size_array-2
 int k = 0;
void findpeaks(uint16_t* changed_profile,int begain, int increment, bool snowORhail)
{
  double speed = 0.0;
  
  for(int i=1; i< size_array - 2;  i++)
  {
    if(changed_profile[i-1]<changed_profile[i] && changed_profile[i]>changed_profile[i+1])
    {
    //  amount_peaks1+=1;
    //  sum_peaks+=array[i];           //сумма длительностей
     
      speed = thresholds.l * 1000/changed_profile[i];
  //    array_speed[k]=speed;
            k++;
   //   sum_speeds+=speed1;   //сумма скоростей    
      if(speed>0)
      {
        if((!snowORhail && speed > thresholds.limsnow) || ( snowORhail && speed > thresholds.limspeedhail))
       
        {
          amount_rain++;    //counting rainfall
        }
        else
          amount_snow++;
      }
    }
  } 
 
}

//рассчет длительности очистка меньше порога, гасим мусор
void clearp(uint16_t* changed_profile, bool* flag, int max, int begain, int increment)
{
  
  int count = 1;
  for(int index = begain; count < size_array - 2; count++, index+=increment)               
  {
    if(flag[index]==0) 
    {
      if(changed_profile[index] > max * thresholds.clearp)
      {
       while(flag[index]==0 && count < size_array - 2)
       { index+=increment;
       count++;}
      }
      else
        changed_profile[index]=0;
    }
   else
     changed_profile[index]=0;
  }
}

void culculate_derivative(int begain, int increment, bool snowORhail)  //рассчет производной 
{
  uint16_t changed_profile[size_array];//size_array-2
  bool flags[size_array];                        //айяйяй переделать
  int n = 0, nn = 0, nnn = 0;
  int max = 0;
  
  
  int count = 1;
  int index = begain;
   for (; count < size_array_converted -1; count++, index += increment)                //айяйяй переделать
  {
    changed_profile[index]=0;
    flags[index] = 0;
    n=profile[index+(increment*5)]-profile[index];
    if(n > intensity.rate_change)
    {
      nn = nn + n;
      nnn = nnn + 1;
      changed_profile[index] = 0;
      flags[index]=1;      
    }
    else
    {
       if((!snowORhail && nnn>thresholds.minduration) || ( snowORhail && nnn <  7 && nnn > 0))
      {
       if(nn>thresholds.minderivative)
        {
          changed_profile[index] = nnn;
          flags[index]=0; 
          if(max<nnn)
          {
            max=nnn;
          }
        }     
      }
      nn=0;
      nnn=0;
    }
  }

   increment*=(-1);
  begain=index+increment;            //меняем направление цикла
 
 clearp(changed_profile, flags, max, begain, increment);
 findpeaks(changed_profile, begain, increment, snowORhail);

}

void culculate_rain_snow( bool snowORhail)
{
   int count = size_array;//переделать

     //внести в функцию
    amount_peaks1 = 0;
  //  sum_peaks = 0;
   // sum_speeds = 0.0;
    amount_snow = 0;
    amount_rain = 0;
    culculate_derivative(0, 1, snowORhail);//переделать
    culculate_derivative(size_array_converted-3, -1, snowORhail);//переделать
    
   // percentrain = amount_rain * 100/(amount_rain + amount_snow);     // процент вероятности дождя
   // percentsnow = amount_snow * 100/(amount_rain + amount_snow);     // процент вероятности снега

}

//precipitation 
//rain && snow && drizzle && hail
void filling_vectors_precipitation(float temperature)
{
  //(-1; 6)
  if ( temperature > -1 && temperature<6 )
  {
    culculate_rain_snow(0); 
   
    //вычисление процента вероятности осадков дождя и снега
    uint16_t percentrain = 0;
    uint16_t percentsnow= 0;
    if(amount_rain>0 || amount_snow>0)
    {
      percentrain = amount_rain * 100/(amount_rain + amount_snow);
      percentsnow= amount_snow * 100/(amount_rain + amount_snow);     // процент вероятности снега
    }
    if(percentrain>thresholds.limitcipitation)
      rain.rewrite_last(percentrain);
    if(percentsnow>thresholds.limitcipitation)
      snow.rewrite_last(percentsnow);
  
    if (percentrain == 0 && percentsnow == 0)	// матлаб не определил
    {
      if (bitprecipitation || bitdrizzle )	// но осадки есть
      {
        precipitation.rewrite_last(100);		//осадки неопределенного типа
      }  
    else
      //если не осадки // морось ////есть дождь
      if (!bitprecipitation && bitdrizzle) //&& percentrain > 0)			
      {
        if (percentrain>0)
        {
            drizzle.rewrite_last(percentrain);
            rain.rewrite_last(0);    
        }
       }
    
    }
   
    //снег и дождь 
    double averaged_rain= rain.sum();
    double averaged_snow = snow.sum();
    if (averaged_rain <  thresholds.sumlimit && averaged_snow <  thresholds.sumlimit)
    {
      if((averaged_rain/8)>(100-thresholds.limitcipitation) && (averaged_snow/8)>(100-thresholds.limitcipitation)) 
        rain_and_snow.rewrite_last(100);    
    } 
     bitdrizzle=0;
     bitprecipitation=0;
  }
  else
  { 
    //(-∞; -1]
    if ( temperature <= -1 )
    {
      if (bitprecipitation || bitdrizzle) 
      {
        snow.rewrite_last(100);				/*snow 100%*/
      } 
      bitdrizzle=0;
      bitprecipitation=0;
    }
    //[6; +∞)
    if ( temperature >= 6)      // && temperature <=15)
    { 
      uint16_t percenthail = 0;  
      if(thresholds.flag_hail && temperature>=15)
      {
        culculate_rain_snow(1);  //hail
                    //вычисление процента вероятности 
            
        if(amount_rain>0 )
        {
          percenthail = amount_rain * 100/(amount_rain + amount_snow); // процент вероятности града          
        
        }
        if (percenthail > 0) 
        {
         hail.rewrite_last(100);
        }
      }
      if(bitprecipitation &&  percenthail<40)
      {
        rain.rewrite_last(100);    		/*Rain 100%*/
      }
      else
      {
        if(bitdrizzle) 
          drizzle.rewrite_last(100);           /*Drizzle 100%*/
      }
      bitdrizzle=0;
      bitprecipitation=0;
    }
    
  }
  
}

void fill_vectors_weather(uint32_t visibility, float temperature )
{  
 // double T_fl = search_for_extremes(fscattering);         //  Π񮤨젶 񪱲𥬳􈠠񠬠벨𪱶󺐠᭯쩲󤮩


  clear_vectors();
  if ( !bithazeorfog && !bitfog1 && ! bitfog2 && !bitdrizzle && !bitprecipitation )    /*Clear*/
  {
    clear.rewrite_last(100);
  }
  else
  {
    if (bithazeorfog || bitfog1 || bitfog2)
    {
      if ( visibility < 1000 )
      {
        fog.rewrite_last(100);     /*Fog*/
      }
      else
      {
        if (visibility < 10000)
        {      
          haze.rewrite_last(100); /*Haze*/
        }
        else
          if(!bitdrizzle && !bitprecipitation )
        { /*Clear*/
          clear.rewrite_last(100);         
        }
      }
      bithazeorfog = 0;
      bitfog1 = 0;
      bitfog2 = 0;
    }
    if 	(bitprecipitation || bitdrizzle)   			//precipitation
    {
       algolithm_intensity(profile);
      filling_vectors_precipitation(temperature);    
      bitprecipitation=0;
      bitdrizzle=0;
    }
  }
}

void culculate_precipitation(uint32_t visibility, float temperature)
{
  uint16_t fscattering[size_array];
  bithazeorfog = 0;
  bitdrizzle = 0;
  bitprecipitation = 0;

 // particle_scattering(PS0x, fscattering);    // рассеянее частиц
  filling_massiv(fscattering);
  maxmin_scattering(fscattering);          // максимальный и минимальный элемент рассеяния
  flag_weather(fscattering, temperature);                // устанавливаем флаги погоды
 
}
int index_minute=0;
int index_hour=0;

uint8_t weather(uint32_t visibility, double temperature)
{       
/*  for(int i=0;i<9120; i++)
  {
    profile[i] = data_prof[i];
  }
  visibility = 1000;*/

  
  culculate_precipitation(visibility, temperature);
  fill_vectors_weather(visibility, temperature);
  int index=vote_pheno();
  
   if(flag_vaisala )
  {
    count_minute+=1;
    if(count_minute>=7) 
      filling_pheno_vaisala(index);    
    index_minute=vote_pheno_vaisala(index);
    if(count_hour>=7)
      filling_pheno_vaisala_hour(index_minute);
    index_hour=vote_pheno_vaisala_hour(index);
  }
  return index;  
}

