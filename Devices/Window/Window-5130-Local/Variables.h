#define DIR_OPEN 1
#define DIR_CLOSE 2
#define HOLD_OPEN 3
#define HOLD_CLOSE 4
#define MOVE_OPEN 5
#define STOP 6
#define MOVE_CLOSE 7
#define AUTO_CALIBRATE 8
#define SET_MAX_STEPS 9
#define CUSTOM_MOVE 10
#define POSITION_CLOSE 11
#define POSITION_OPEN 12
#define POSITION_ADJUST 13
#define STEP_1 14
#define STEP_3 16
#define STEP_4 17
#define STEP_5 18
#define STEP_6 19
#define OPEN_CLOSE 20
#define AUTO_TUNE 21

Preferences preferences;

String device_status;

int waitTime;
int lowTime;
int maxCloseTime;

int MOVE_OPEN_VELOCITY = 400000;
int MOVE_CLOSE_VELOCITY = 400000;
int MOVE_ACCEL = 3000;
int MOVE_DECEL = 80000;
int MOVE_DECEL_CLOSED = 100000;
int CALIBRATION_VELOCITY = 20000;
int setup_current = 3072;

//High Current
int current_high_open_initial_value = 4;
int current_high_close_initial_value = 4;

int stall_high_current_setup = 15;

int stall_high_open_initial_value = 0;
int stall_high_close_initial_value = 0;

int current_high_open_passes = 3;
int current_high_close_passes = 3;

int stall_high_open_passes = 3;
int stall_high_close_passes = 3;

//Low Current
int current_low_open_initial_value = 3;
int current_low_close_initial_value = 3;

int stall_low_current_setup = 15;

int stall_low_open_initial_value = 0;
int stall_low_close_initial_value = 0;

int current_low_open_passes = 1;
int current_low_close_passes = 1;

int stall_low_open_passes = 1;
int stall_low_close_passes = 1;


float gear_ratio = 5.18;
float inches_mm = 25.4;
int motor_microsteps = 64; //
int full_steps_mm = 100;

float one_inch = gear_ratio * inches_mm * motor_microsteps * full_steps_mm;

int stall_close_high;
int stall_open_high;
int stall_close_low;
int stall_open_low;

//int close_current;
//int open_current;
int current_setup = 3840;
int close_current_high;
int open_current_high;
int close_current_low;
int open_current_low;

int move_to_position;

bool move_close_stall;
bool move_open_stall;
bool restart_movement;

bool Orientation_Right;

unsigned long sendData(unsigned long address, unsigned long datagram);

int XACTUAL;

int command = -1;

int current_position;

int max_steps;
int move_direction;
int CLOSE_POSITION;

int open_stall_calibration_low;
int open_stall_calibration_value_low;
int close_stall_calibration_low;
int close_stall_calibration_value_low;

int open_stall_calibration_high;
int open_stall_calibration_value_high;
int close_stall_calibration_high;
int close_stall_calibration_value_high;

//int open_current_calibration;
//int open_current_calibration_value;
int open_current_calibration_high;
int open_current_calibration_value_high;
int open_current_calibration_low;
int open_current_calibration_value_low;

//int close_current_calibration;
//int close_current_calibration_value;
int close_current_calibration_high;
int close_current_calibration_value_high;
int close_current_calibration_low;
int close_current_calibration_value_low;

bool motor_running = false;

bool reverse_flag = false;

void delayStall(long timeout);
void waitStall(long timeout);
void turnMotor(int dir);

bool stall_flag = false;

int moveFreq = 1000;
int move_time_on = 100;
int moveChannel = 13;
int moveResolution = 16;

int number_open_passes;
int number_close_passes;
int calibration_direction;

// State variables will change:
int windowState = 0;         // current state of the door
int lastWindowState = 0;     // previous state of the door
int stallOpenState = 0;
int lastStallOpenState = 0;
int stallCloseState = 0;
int lastStallCloseState = 0;

int just_open_position; 
bool just_open_set_button = false;
int stepValue;
bool fast_loud;
int speed_type;

// State variables will change:
int closeState = LOW;         // current state of the door
int lastcloseState = LOW;     // previous state of the door
int openState = LOW;         // current state of the door
int lastopenState = LOW;     // previous state of the door

int run_section;
float slider_display;
float slider_XACTUAL;
int distance_cal = false;
