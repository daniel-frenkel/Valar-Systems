bool stalled_motor = false;
bool stop_motor = false;
volatile bool sensor1_trip = false;
volatile bool sensor2_trip = false;
int current_position;
int max_steps;
int current;
int stall; 
int accel = 150;
int max_speed = 200;
int tcools = (3089838.00*pow(float(max_speed*64),-1.00161534))*1.5;
int move_to_step = 0;
int move_to_percent = 0;
int calibrate = 0; // Set to 1 to set home position
bool run_motor = false;
int wifi_set;
bool wifi_button = false;;
String ssid;
String pass;
volatile int stall_cal;
volatile int current_cal;


String MYTIMEZONE;
int close_timer = 0;
int close_hour;
int close_hour_input;
int close_minute;
int open_timer = 0;
int open_hour;
int open_hour_input;
int open_minute;
String open_time_string;
String close_time_string;
int openEvent;
int closeEvent;
int openEventNow;
int closeEventNow;
int open_direction;

int open_am_pm;
String open_am_pm_s;
  
int close_am_pm;
String close_am_pm_s;

float gear_ratio = 1;
float inches_mm = 25.4;
int motor_microsteps = 0; 
int motor_steps_per_rev = 200;
int thread_pitch = 2;
int starts = 4;
int one_inch = gear_ratio * ((motor_steps_per_rev * 1)/(thread_pitch * starts)) * inches_mm;

Preferences preferences;
  
void load_preferences(){
  
  Serial.println("LOADING PREFERENCES");

  wifi_set = preferences.getInt("wifi_set", 0);
  ssid = preferences.getString ("ssid", "NO_SSID");
  pass = preferences.getString ("pass", "NO_PASSWORD");
  max_steps = preferences.getInt("max_steps", 2000);
  Serial.println(max_steps);
  current = preferences.getLong("current", 400);
  stall = preferences.getInt("stall", 10);

  Serial.println("FINISHED LOADING PREFERENCES");
  }

  
