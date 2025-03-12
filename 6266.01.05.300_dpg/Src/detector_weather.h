extern uint16_t maxminamplitude;

extern uint8_t weather( uint32_t visibility, double termo_val);
extern uint16_t ClassicMedianFilter(uint16_t* ptrArray);
struct Thresholds__
{
  bool flag=0;
  uint16_t clear;
  uint16_t noise;
  uint16_t haze;
  uint16_t fog;

  uint16_t drizzle;     
  uint16_t precipitation;
  double t_corr;
  double out_t_corr;
  uint16_t limitcipitation;
  
  uint16_t sumlimit;
  uint16_t slp;
  //double scattering_coefficient;
  
  double limsnow;
  double minduration;
  double minderivative ;
  double clearp  ;
  double l;
  
  bool flag_hail;
  uint16_t limspeedhail;     
  uint16_t mindurationhail;
  
   bool termo;
 
};

extern struct Thresholds__ thresholds; 


struct Intensity__
{
  uint8_t rate_change = 15;     //rate of change of the function for the derivative
  uint8_t filterwindow = 10;
  uint8_t limitderivative = 100;
  uint8_t limitfogduration = 50; 
  uint8_t limitpretipitation = 500; 
  uint8_t deltabeginend = 300; 
};
extern struct Intensity__ intensity;

extern int X;