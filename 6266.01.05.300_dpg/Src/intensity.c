//#include <numeric>
#include <algorithm>

#include "intensity.h"
#include "main.h"


struct Intensity__ intensity;
#define size_array  (9120)


uint64_t calculate_intensity_sum(int current_index_begin, int current_index_end, int16_t* profile)
{ 
  uint64_t sum = 0;
  int rain;
  int delta = profile[current_index_begin]; 
 
  for(int i = current_index_begin + 1; i <= current_index_end; i++)
  {
    rain = profile[i] - delta;
    if(rain < 0) rain = 0; 
    sum += rain;

  }
  if ( sum > 0) intensity.countdrop++;
    
  return sum;
}

// Вспомогательная функция для проверки бита в массиве флагов
static inline bool check_bit(uint8_t* flags, int index) {
    return (flags[index / 8] >> (7 - index % 8)) & 1;
}

// Вспомогательная функция для сброса состояния капли
static inline void reset_drop_state(int* amplitude_begin, int* index_begin, int* amplitude_end, int* index_end, int* duration) {
    *amplitude_begin = 0;
    *index_begin = 0;
    *amplitude_end = 0;
    *index_end = 0;
    *duration = 0;
}

uint64_t find_one_drop(uint8_t* begin_flags, uint8_t* end_flags, int limitduration, int16_t* profile)
{
  uint64_t one_sum = 0;
  int duration = 0;
  
  intensity.countdrop = 0;
  
  int current_amplitude_begin = 0;
  int current_index_begin = 0;
  int current_amplitude_end = 0;
  int current_index_end = 0;

  bool begin_value;
  bool end_value;

  for (int index = 1; index < size_array - 1; index++ )//определение капли на профиле
  {
    begin_value = check_bit(begin_flags, index);
    end_value = check_bit(begin_flags, index);
    
    if (current_index_begin != 0)
      duration++; 
    
    if (begin_value && profile[index] != 0 && current_index_begin == 0) // обнаружили начало капли
    {
      current_amplitude_begin = profile[index];
      current_index_begin = index;
      duration = 0;
    }
   //если было начало пропускаем все до обнаружения окончания
    else if (end_value && profile[index - 1] != 0 && current_index_begin != 0) 
    {
      // окончание капли и есть начало все промежуточные склеиваются
      current_amplitude_end = profile[index]; // сохраняем амплитуду окончания капли
      current_index_end = index - 1;
      //
      if (current_amplitude_begin > (current_amplitude_end - intensity.deltabeginend))
      { 
        //тут обработать каплю 
        one_sum += calculate_intensity_sum(current_index_begin, current_index_end, profile);        
      }      
      // Сброс состояния для поиска следующей капли
      reset_drop_state(&current_amplitude_begin, &current_index_begin, &current_amplitude_end, &current_index_end, &duration);      
      
    }  
    //abort processing if duration exceeds limit
    if ( duration > limitduration )
    {
      if (end_value && profile[index - 1] != 0 && current_index_begin != 0) 
      {
        current_index_end = index - 1;
        one_sum += calculate_intensity_sum (current_index_begin, current_index_end,  profile);       
      }
      // Сброс состояния для поиска следующей капли
      reset_drop_state(&current_amplitude_begin, &current_index_begin, &current_amplitude_end, &current_index_end, &duration);      
    }
  }
  
  return one_sum;
}


void set_drops_flags(uint16_t* changed_profile, int begain, int increment, uint8_t* flags)
{
  int count = 1;
  uint8_t mask = 0b10000000;
  for(int index = begain; count < size_array - 3; count++, index+=increment)  
  {
    if(changed_profile[index + increment] == 0 && changed_profile[index]>1)
    {
     // flags[index] = 1;
       flags[index/8] += 0b10000000 >> (index % 8); 
    }
  }
}

void clear(uint16_t* changed_profile, int begain, int increment)
{  
  bool flag=0;
  int count = 1;
  for(int index = begain; count < size_array - 3; count++, index+=increment)               
  {   
   if( changed_profile[index + increment]>= intensity.limitderivative && changed_profile[index]==0) 
     flag = 1;
   if(flag)
   {
     if(changed_profile[index + increment] == 0)
       flag=0;
   }
   else
     changed_profile[index]=0;
  }
}

void intensity_derivative(int begain, int increment, uint8_t* flags, int16_t* profile)  //рассчет производной 
{
  uint16_t changed_profile[size_array];//size_array-2
  memset(changed_profile, 0, size_array*2);
  int n = 0, nn = 0;
 
  int count = 1;
  int index = begain;
  for (; count < size_array -1; count++, index += increment)                //айяйяй переделать
  {
    //changed_profile[index]=0;  
    n=profile[index+(increment)]- profile[index];
    if(n > intensity.rate_change)
    {
      nn = nn + n;
      changed_profile[index] = nn;    
    }
    else
    {
      nn=0; 
      changed_profile[index] = 0;
    }
  }
  int sign = increment *(-1);
 // begain=index+increment;            //меняем направление цикла
 
  clear(changed_profile, index - increment, sign);
  //findpeaks(changed_profile, begain, increment, 0);
 // increment*=(-1);          //again меняем направление цикла
  set_drops_flags(changed_profile, begain + increment, increment, flags);
  
}
//uint64_t sum_intensity = 0;
void algolithm_intensity(int16_t* profile)
{
  intensity.value_intensity = 0;

  uint16_t limitduration = 50;
   
  uint8_t begin_flags[size_array/8];  
  memset(begin_flags, 0, size_array/8);
  intensity_derivative(0, 1, begin_flags, profile);
  uint8_t end_flags[size_array/8]; 
  memset(end_flags, 0, size_array/8);
  intensity_derivative(size_array-1, -1, end_flags, profile);
 
 
  intensity.sum_intensity = find_one_drop(begin_flags, end_flags, limitduration, profile);
 // intensity.value_intensity =  intensity.sum_intensity * koeff * 10000 / size_array;
    
}