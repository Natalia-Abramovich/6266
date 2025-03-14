struct Intensity__
{
  uint8_t rate_change = 15;     //rate of change of the function for the derivative
  uint8_t filterwindow = 5;
  uint16_t limitderivative = 75;
  uint16_t limitfogduration = 50; 
  uint16_t limitprecipitation = 500; 
  uint16_t deltabeginend = 120;//300; 
  uint64_t sum_intensity = 0;
  double value_intensity;
  double light;
  double heave;
  bool flag_intensity=0;
  int countdrop = 0;
  double intensity_count = 0;
};
  
extern struct Intensity__ intensity;
extern void algolithm_intensity(int16_t* profile);

  
extern int16_t profile_back[];
