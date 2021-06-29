bool stalled_motor = false;
bool stop_motor = false;
bool sensor1_trip = false;
bool sensor2_trip = false;
int XACTUAL;
int max_steps;
int current;
int stall; 
int accel;
int max_speed;
int tcools;
volatile uint32_t move_to = 0;
volatile uint32_t move_percent = 0;
int set_zero = 0; // Set to 1 to set home position
bool run_motor = false;
int wifi_set;
bool wifi_button = false;;
String ssid;
String pass;

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
