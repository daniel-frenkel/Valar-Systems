#define MOVE 2
#define STOP 3

int MOVE_VELOCITY;
int MOVE_ACCEL;

//Inches to steps calculation
float gear_ratio = 1;
float pulley_diameter = 12.22; 
int motor_microsteps = 64; 

float inches_mm = 25.4;
float circumferance = 2 * PI * (pulley_diameter/2);
float inch_ratio = inches_mm/circumferance;

float one_inch = (motor_microsteps * 200) * inch_ratio * gear_ratio; //3.8mm radius

int stall_close;
int stall_open;
int current_close;
int current_open;

int XACTUAL;
float move_to_position;
float open_percent;

int command = 1;

int current_position;

int max_steps;

int CLOSE_POSITION;

bool motor_running = false;

bool reverse_flag = false;

int stalled_motor = false;
int stop_motor = false;

int TCOOLS;
volatile int trip;
int distance_cal = false;
bool blynk_update = false;
