
//extern double kx;
extern bool flag_approximation ;
extern int amount_point;
extern double delta;
extern int flag_calibration;

extern double dataMOR[16];
extern double dataPS[16];

extern void read_approximation_parameters();
extern int write_approximation_parameters(char write_name[], double write_value_array[], int memAddr);

extern double calculate_visibility(double PS);