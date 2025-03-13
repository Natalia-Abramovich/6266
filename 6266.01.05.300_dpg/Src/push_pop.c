#include "main.h"
#include "push_pop.h"
#include "math.h"

double  PG_stack[10] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};

uint8_t stkidxPG   = 0;
uint8_t stk_lvlPG  = 0;

double  pop(uint8_t stk_lvl,  double* stack)
{
  uint8_t i;
  double  rslt = 0.0;
  for (i = 0; i < stk_lvl; i++)
  {
    rslt += stack[i];
  }
  return rslt /= (double)stk_lvl;
}


void push(uint8_t &stk_lvl, uint8_t max_stk_lvl, uint8_t &stk_idx, double value, double* stack)
{
 stack[stk_idx] = value;
   stk_idx = (stk_idx + 1) % max_stk_lvl;
  if (stk_lvl < max_stk_lvl)
  {
    stk_lvl++;
  }  
}