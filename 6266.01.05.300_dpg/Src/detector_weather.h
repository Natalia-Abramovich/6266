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

