bool stalled_motor = false;
bool stop_motor = false;
volatile bool sensor1_trip = false;
volatile bool sensor2_trip = false;
int XACTUAL;
int chopconf;
int max_steps;
int current;
int current_value;
int stall; 
int stall_value; 
int accel;
int max_speed;
int tcools;
int move_to = 0;
int move_percent = 0;
int set_zero = 0; // Set to 1 to set home position
bool run_motor = false;
int wifi_set;
bool wifi_button = false;;
String ssid;
String pass;
volatile int stall_cal;
volatile int current_cal;


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
  max_steps = preferences.getInt("max_steps", 51200);
  current = preferences.getLong("current", 400);
  current_value = preferences.getInt("current_value", 1000);
  max_speed = preferences.getInt("max_speed", 120000);
  stall = preferences.getInt("stall", 4128768);
  stall_value = preferences.getInt("stall_value", 0);
  tcools = (3089838.00*pow(float(max_speed*64),-1.00161534))*1.5;
  //chopconf = preferences.getInt("chopconf", 0x101D5);
  Serial.println("FINISHED LOADING PREFERENCES");
  }

  
