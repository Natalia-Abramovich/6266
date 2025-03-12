#include "stm32f3xx.h"
#include "main.h"

#include "OneWire.h"
#include "i2c.h"
//extern double count_termo(double MORmomentary_koeff);

struct flag_error__
{
  bool temperature_inside=0;
  bool temperature_outside=0;
};
extern struct flag_error__ flag_error;

extern double select_temperature();