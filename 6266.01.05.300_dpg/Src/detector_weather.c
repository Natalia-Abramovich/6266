#include <vector>
#include <numeric>
#include <algorithm>

//
#include "data.h"
#include "push_pop.h"
#include "main.h"
#include "detector_weather.h"



//template<class T>

uint16_t maxscattering;
uint16_t maxindexscattering;
uint16_t minscattering;
uint16_t minindexscattering;
uint16_t maxminamplitude;
/*uint16_t limitcipitation = 75;
uint16_t sumlimit= 450;
uint16_t sumlimitprecipitation = 400;*/
uint32_t average; 
bool bithazeorfog = false;
bool bitdrizzle = false;
bool bitprecipitation = false;
bool bitfog1 = false;
bool bitfog2 = false;

uint8_t byteweather = 0;

using namespace std;
vector <bool> clear=  {0,0,0,0,0,0,0,0};
vector <bool> haze=   {0,0,0,0,0,0,0,0};
vector <bool> fog=    {0,0,0,0,0,0,0,0};
vector <bool> precipitation = {0,0,0,0,0,0,0,0};
vector <uint8_t> drizzle={0,0,0,0,0,0,0,0};
vector <uint8_t> rain=   {0,0,0,0,0,0,0,0};
vector <uint8_t> snow=   {0,0,0,0,0,0,0,0};
vector <bool> rain_and_snow={0,0,0,0,0,0,0,0};
vector <bool> hail=  {0,0,0,0,0,0,0,0};

uint8_t count_minute=0;
uint8_t count_hour=0;
vector <bool> clear_minute =  {0,0,0,0,0,0,0,0};
vector <bool> haze_minute =   {0,0,0,0,0,0,0,0};
vector <bool> fog_minute  =    {0,0,0,0,0,0,0,0};
vector <bool> precipitation_minute = {0,0,0,0,0,0,0,0};
vector <bool> drizzle_minute ={0,0,0,0,0,0,0,0};
vector <bool> rain_minute ={0,0,0,0,0,0,0,0};
vector <bool> snow_minute =   {0,0,0,0,0,0,0,0};
vector <bool> rain_and_snow_minute={0,0,0,0,0,0,0,0};
vector <bool> hail_minute =  {0,0,0,0,0,0,0,0};

vector <bool> clear_hour =  {0,0,0,0};
vector <bool> haze_hour =   {0,0,0,0};
vector <bool> fog_hour =    {0,0,0,0};
vector <bool> precipitation_hour = {0,0,0,0};
vector <bool> drizzle_hour ={0,0,0,0};
vector <bool> rain_hour =   {0,0,0,0};
vector <bool> snow_hour =   {0,0,0,0};
vector <bool> rain_and_snow_hour ={0,0,0,0};
vector <bool> hail_hour =  {0,0,0,0};

uint8_t count_drizzle; 
uint8_t count_precipitation;

int X=2;

struct Thresholds__ thresholds;
struct Intensity__ intensity;


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

/*void particle_scattering(uint16_t PS0x, uint16_t* fscattering)             //     �������� ������ ������ ����������� fscattering[10240] ����������� ������� �������� ���� �� �������� �������
{
  uint16_t median_filter_array[10] = {0};
    for (int index_profile = 0; index_profile < size_array; index_profile++)
    {
        for (int index_median_ms = 0; index_median_ms < size_window; index_median_ms++)
        {
            if ((index_profile + index_median_ms - size_window / 2) < 0 || (index_profile + index_median_ms - size_window / 2 )>= size_array)
                median_filter_array[index_median_ms] = 0;
            else
                median_filter_array[index_median_ms] = profile[index_profile + index_median_ms - size_window / 2];
        }
         fscattering[index_profile] = ClassicMedianFilter(median_filter_array, size_window);
    }
}*/

/*void particle_scattering( uint16_t* fscattering)             //     ௫󷠥졬Ჱ颠󠱲鶠𠱱橢ᮨ󠦳cattering[10240] ௽즬殲㼷鳠󠧭ḥ 𮭠 駠㶮宮䮠𬕢𸀍
{
 
    for (int index_profile = 0; index_profile < size_array; index_profile++)
    {
       
         fscattering[index_profile] = ClassicMedianFilter(median_filter_array, size_window);
    }
}*/
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
 clear.erase(clear.begin());
 clear.emplace_back(0);
 haze.erase(haze.begin());
 haze.emplace_back(0);
 fog.erase(fog.begin());
 fog.emplace_back(0);
 precipitation.erase(precipitation.begin());
 precipitation.emplace_back(0);
 drizzle.erase(drizzle.begin());
 drizzle.emplace_back(0);
 rain.erase(rain.begin());
 rain.emplace_back(0);
 snow.erase(snow.begin());
 snow.emplace_back(0);
 rain_and_snow.erase(rain_and_snow.begin());
 rain_and_snow.emplace_back(0);
 hail.erase(hail.begin());
 hail.emplace_back(0);  
}

void clear_vectors_vaisala()
{
 clear_minute.erase(clear_minute.begin());
 clear_minute.emplace_back(0);
 haze_minute.erase(haze_minute.begin());
 haze_minute.emplace_back(0);
 fog_minute.erase(fog_minute.begin());
 fog_minute.emplace_back(0);
 precipitation_minute.erase(precipitation_minute.begin());
 precipitation_minute.emplace_back(0);
 drizzle_minute.erase(drizzle_minute.begin());
 drizzle_minute.emplace_back(0);
 rain_minute.erase(rain_minute.begin());
 rain_minute.emplace_back(0);
 snow_minute.erase(snow_minute.begin());
 snow_minute.emplace_back(0);
 rain_and_snow_minute.erase(rain_and_snow_minute.begin());
 rain_and_snow_minute.emplace_back(0);
 hail_minute.erase(hail_minute.begin());
 hail_minute.emplace_back(0);  
}
void clear_vectors_vaisala_hour()
{
 clear_hour.erase(clear_hour.begin());
 clear_hour.emplace_back(0);
 haze_hour.erase(haze_hour.begin());
 haze_hour.emplace_back(0);
 fog_hour.erase(fog_hour.begin());
 fog_hour.emplace_back(0);
 precipitation_hour.erase(precipitation_hour.begin());
 precipitation_hour.emplace_back(0);
 drizzle_hour.erase(drizzle_hour.begin());
 drizzle_hour.emplace_back(0);
 rain_hour.erase(rain_hour.begin());
 rain_hour.emplace_back(0);
 snow_hour.erase(snow_hour.begin());
 snow_hour.emplace_back(0);
 rain_and_snow_hour.erase(rain_and_snow_hour.begin());
 rain_and_snow_hour.emplace_back(0);
 hail_hour.erase(hail_hour.begin());
 hail_hour.emplace_back(0);  
}


vector < uint16_t > pheno_coll(9, 0);
//uint8_t const phenocodes[] = { 0x00, 0x04, 0x30, 0x40, 0x50, 0x60, 0x68, 0x70, 0x89};
uint8_t vote_pheno_vaisala_hour(uint8_t index)
{ 
  pheno_coll[0] = std::accumulate(clear_hour.begin(), clear_hour.end(), 0);
  pheno_coll[1]= std::accumulate(haze_hour.begin(), haze_hour.end(), 0);
  pheno_coll[2]= std::accumulate(fog_hour.begin(), fog_hour.end(), 0);
  pheno_coll[3]= std::accumulate(precipitation_hour.begin(), precipitation_hour.end(), 0);
  pheno_coll[4]= std::accumulate(drizzle_hour.begin(), drizzle_hour.end(), 0);
  pheno_coll[5]= std::accumulate(hail_hour.begin(), hail_hour.end(), 0);
  pheno_coll[6]= std::accumulate(rain_and_snow_hour.begin(), rain_and_snow_hour.end(), 0);
  pheno_coll[7]= std::accumulate(rain_hour.begin(), rain_hour.end(), 0);
  pheno_coll[8]= std::accumulate(snow_hour.begin(), snow_hour.end(), 0);  
 
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
  pheno_coll[0] = std::accumulate(clear_minute.begin(), clear_minute.end(), 0);
  pheno_coll[1]= std::accumulate(haze_minute.begin(), haze_minute.end(), 0);
  pheno_coll[2]= std::accumulate(fog_minute.begin(), fog_minute.end(), 0);
  pheno_coll[3]= std::accumulate(precipitation_minute.begin(), precipitation_minute.end(), 0);
  pheno_coll[4]= std::accumulate(drizzle_minute.begin(), drizzle_minute.end(), 0);
  pheno_coll[5]= std::accumulate(hail_minute.begin(), hail_minute.end(), 0);
  pheno_coll[6]= std::accumulate(rain_and_snow_minute.begin(), rain_and_snow_minute.end(), 0);
  pheno_coll[7]= std::accumulate(rain_minute.begin(), rain_minute.end(), 0);
  pheno_coll[8]= std::accumulate(snow_minute.begin(), snow_minute.end(), 0);
    
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
  switch(index)
  {
    case 0: clear_hour[7]=1;
    break;
    case 1: haze_hour[7]=1;
    break;
    case 2: fog_hour[7]=1;
    break;
    case 3: precipitation_hour[7]=1;
    break;
    case 4: drizzle_hour[7]=1;
    break;
    case 5: hail_hour[7]=1;
    break;
    case 6: rain_and_snow_hour[7]=1;
    break;
    case 7: rain_hour[7]=1;
    break;
    case 8: snow_hour[7]=1;
    break;  
  }
  count_minute=0;  
}

uint8_t filling_pheno_vaisala(uint8_t index)
{
  count_hour+=1;
  clear_vectors_vaisala();
  switch(index)
  {
    case 0: clear_minute[7]=1;
    break;
    case 1: haze_minute[7]=1;
    break;
    case 2: fog_minute[7]=1;
    break;
    case 3: precipitation_minute[7]=1;
    break;
    case 4: drizzle_minute[7]=1;
    break;
    case 5: hail_minute[7]=1;
    break;
    case 6: rain_and_snow_minute[7]=1;
    break;
    case 7: rain_minute[7]=1;
    break;
    case 8: snow_minute[7]=1;
    break;
   
    
  }
  count_minute=0;  
}

uint8_t vote_pheno(void)
{
 /* 
  pheno_coll[3]= std::accumulate(precipitation.begin(), precipitation.end(), 0)*100;
  pheno_coll[4]= std::accumulate(drizzle.begin(), drizzle.end(), 0)*100;
  pheno_coll[5]= std::accumulate(rain.begin(), rain.end(), 0);
  pheno_coll[6]= std::accumulate(rain_and_snow.begin(), rain_and_snow.end(), 0)*100;
  pheno_coll[7]= std::accumulate(snow.begin(), snow.end(), 0);
  pheno_coll[8]= std::accumulate(hail.begin(), hail.end(), 0)*100;*/
  
  pheno_coll[0] = std::accumulate(clear.begin(), clear.end(), 0)*100;
  pheno_coll[1]= std::accumulate(haze.begin(), haze.end(), 0)*100;
  pheno_coll[2]= std::accumulate(fog.begin(), fog.end(), 0)*100;
  pheno_coll[3]= std::accumulate(precipitation.begin(), precipitation.end(), 0)*100;
  pheno_coll[4]= std::accumulate(drizzle.begin(), drizzle.end(), 0);
  pheno_coll[5]= std::accumulate(hail.begin(), hail.end(), 0)*100;
  pheno_coll[6]= std::accumulate(rain_and_snow.begin(), rain_and_snow.end(), 0)*100;
  pheno_coll[7]= std::accumulate(rain.begin(), rain.end(), 0);
  pheno_coll[8]= std::accumulate(snow.begin(), snow.end(), 0);

  int max_value=pheno_coll[8];
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
 //////
  /*int max_value=0;// pheno_coll[0];
  int index =0;
  int i=1;
  for ( ;i<=8; i++)
  {
    if(pheno_coll[i] >= max_value)
    {
      index=i;
      max_value = pheno_coll[i];
    }
  }  
  
 if(max_value> thresholds.sumlimit) 
  { 
    return index;
  } 
  else return 0;*/
///////
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
      rain[7]=percentrain;
    if(percentsnow>thresholds.limitcipitation)
      snow[7]=percentsnow;
  
    if (percentrain == 0 && percentsnow == 0)	// матлаб не определил
    {
      if (bitprecipitation || bitdrizzle )	// но осадки есть
      {
        precipitation[7] = 1;		//осадки неопределенного типа
      }  
    else
      //если не осадки // морось ////есть дождь
      if (!bitprecipitation && bitdrizzle) //&& percentrain > 0)			
      {
        if (percentrain>0)
        {
            drizzle[7] = percentrain;
            rain[7] = 0;    
        }
       }
    
    }
   
    //снег и дождь 
    double averaged_rain= std::accumulate(rain.begin(), rain.end(), 0);
    double averaged_snow = std::accumulate(snow.begin(), snow.end(), 0);  
    if (averaged_rain <  thresholds.sumlimit && averaged_snow <  thresholds.sumlimit)
    {
      if((averaged_rain/8)>(100-thresholds.limitcipitation) && (averaged_snow/8)>(100-thresholds.limitcipitation)) 
        rain_and_snow [7]= 1;    
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
        snow [7] = 100;				/*snow 100%*/
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
         hail[7] = 1;
        }
      }
      if(bitprecipitation &&  percenthail<40)
      {
        rain[7] = 100;    		/*Rain 100%*/
      }
      else
      {
        if(bitdrizzle) 
          drizzle [7] = 100;           /*Drizzle 100%*/
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
    clear[7]=1;
  }
  else
  {
    if (bithazeorfog || bitfog1 || bitfog2)
    {
      if ( visibility < 1000 )
      {
        fog[7]=1;     /*Fog*/
      }
      else
      {
        if (visibility < 10000)
        {      
          haze[7]=1; /*Haze*/
        }
        else
          if(!bitdrizzle && !bitprecipitation )
        { /*Clear*/
          clear[7]=1;         
        }
      }
      bithazeorfog = 0;
      bitfog1 = 0;
      bitfog2 = 0;
    }
    if 	(bitprecipitation || bitdrizzle)   			//precipitation
    {
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
 /* for(int i=0;i<9652; i++)
  {
    profile[i]=data_prof[i];
  }
  visibility=3800;
*/
  
  // //заполнение профиля + медианный фильтр 
 
  
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

