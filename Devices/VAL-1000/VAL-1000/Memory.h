bool stalled_motor = false;
bool stop_motor = false;
volatile bool sensor1_trip = false;
volatile bool sensor2_trip = false;
int current_position;
int max_steps;
int current;
int stall; 
int accel;
int max_speed;
int tcools;
int move_to_position = 0;
int move_percent = 0;
int set_zero = 0; // Set to 1 to set home position
bool run_motor = false;
int wifi_set;
bool wifi_button = false;;
String ssid;
String pass;

String MYTIMEZONE;
int close_timer = 0;
int close_hour;
int close_minute;
int open_timer = 0;
int open_hour;
int open_minute;
String open_time_string;
String close_time_string;
int openEvent;
int closeEvent;
int openEventNow;
int closeEventNow;

// Filter anti-rebond (debouncer)
#define DEBOUNCE_TIME 250
volatile uint32_t DebounceTimer = 0;

Preferences preferences;
  
void load_preferences(){
  
  Serial.println("LOADING PREFERENCES");

  wifi_set = preferences.getInt("wifi_set", 0);
  ssid = preferences.getString ("ssid", "NO_SSID");
  pass = preferences.getString ("pass", "NO_PASSWORD");
  max_steps = preferences.getInt("max_steps", 2000);
  current = preferences.getLong("current", 400);
  stall = preferences.getInt("stall", 10);
  accel = preferences.getInt("accel", 100);
  max_speed = preferences.getInt("max_speed", 100);
  tcools = (3089838.00*pow(float(max_speed),-1.00161534))*1.5;

  Serial.println("FINISHED LOADING PREFERENCES");
  }

  
