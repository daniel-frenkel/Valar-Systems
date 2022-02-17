bool stalled_motor = false;
bool stop_motor = false;
volatile bool sensor1_trip = false;
volatile bool sensor2_trip = false;
int XACTUAL = 0;
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

int current_calibration_value;

String MYTIMEZONE;
int close_timer;
int close_hour;
int close_minute;
int open_timer;
int open_hour;
int open_minute;
String open_time_string;
String close_time_string;
int openEvent;
int closeEvent;

unsigned long sendData(unsigned long address, unsigned long datagram);

int gear_ratio = 1;
float circumference_gear = 25.13;
float inches_mm = 25.4;
int steps_revolution = 51200;
float steps_inch = (inches_mm/circumference_gear)*steps_revolution;

Preferences preferences;
  
void load_preferences(){
  
  Serial.println("LOADING PREFERENCES");

  wifi_set = preferences.getInt("wifi_set", 0);
  ssid = preferences.getString ("ssid", "NO_SSID");
  pass = preferences.getString ("pass", "NO_PASSWORD");
  max_steps = preferences.getInt("max_steps", 51200);
  current = preferences.getInt("current", 15);
  current_value = preferences.getInt("current_value", 15);
  max_speed = preferences.getInt("max_speed", 120000);
  stall = preferences.getInt("stall", 4128768);
  stall_value = preferences.getInt("stall_value", 0);
  tcools = (3089838.00*pow(float(max_speed*64),-1.00161534))*1.5;
  MYTIMEZONE = preferences.getString ("MYTIMEZONE", "America/Los_Angeles");
  open_timer = preferences.getInt("open_timer", 0);
  open_hour = preferences.getInt("open_hour", 0);
  open_minute = preferences.getInt("open_minute", 0);
  close_timer = preferences.getInt("close_timer", 0);
  close_hour = preferences.getInt("close_hour", 0);
  close_minute = preferences.getInt("close_minute", 0);
  open_time_string = preferences.getString("open_string", "00:00");
  close_time_string = preferences.getString("close_string", "00:00");
  
  //chopconf = preferences.getInt("chopconf", 0x101D5);
  Serial.println("FINISHED LOADING PREFERENCES");
  }

  
