#include "main.h"
#include "usart.h"

extern double PLref;
extern double Kp;
extern bool flag_termo_compensation;
extern bool flag_glass_poll;
extern int pollution_status;

extern void read_pollution_parameters();
extern void LED_deterioration();
extern void glass_contaminate();
extern void termo_compensation();