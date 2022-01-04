
#define USE_CUSTOM_BOARD          // See "Custom board configuration" in Settings.h

#define APP_DEBUG        // Comment this out to disable debug prints

#define BLYNK_SSL_USE_LETSENCRYPT   // Comment this out, if using public Blynk Cloud

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#include "driver/ledc.h"
#include "BlynkProvisioning.h"
#include <PubSubClient.h>
#include <SPI.h> 
#include <WiFi.h> 
#include <HTTPClient.h> 
#include <ArduinoJson.h> 
#include <WiFiClientSecure.h> 
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "time_store.cpp"
#include "time.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include <Update.h>

#define DIR_OPEN 1
#define DIR_CLOSE 2
#define MOVE_OPEN 5
#define MOVE_CLOSE 6

int stall_close;
int stall_open;
int current_close = 0;
int current_open = 0;
float move_percent;
float move_distance;
int velocity;
bool move_close_stall;
bool move_open_stall;

#include "pins.h"
#include "blynk_pins.h"
#include "Blynk_MQTT.h"
#include "motor_control.h"
#include "OTA_S3.h"

TaskHandle_t TaskA;

BlynkTimer blynkTimer;

void setup() {
  
  delay(100);
  Serial.begin(115200);
  Serial.println("Starting SETUP");

  preferences_local.begin("local", false);

  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);
  
  // for storing the times over power cycle
  
  clockout_setup();
  setup_motors();

   xTaskCreatePinnedToCore(
   IndependentTask,        // Function that should be called
   "NoBlynkSafe",          // Name of the task (for debugging)
   8192,                   // Stack size (bytes)
   NULL,                   // Parameter to pass
   1,                      // Task priority
   &TaskA,                 // Task handle
   0);                     // Core you want to run the task on (0 or 1)


  //Load configs from preferences_local upon reset
  Serial.print("Loading values: ");
  
  mqtt_name = preferences_local.getString("mqtt_name", mqtt_name);
  Serial.println(mqtt_name);

  mqtt_server = preferences_local.getString("mqtt_server", mqtt_server);
  Serial.println(mqtt_server);
  
  mqtt_username = preferences_local.getString("mqtt_username", mqtt_username);
  Serial.println(mqtt_username);
  
  mqtt_password = preferences_local.getString("mqtt_password", mqtt_password);
  Serial.println(mqtt_password);
  
  stall_close = preferences_local.getLong("stall_close", 4128768);
  Serial.println(stall_close);
  
  stall_open = preferences_local.getLong("stall_open", 4128768);
  Serial.println(stall_open);

  current_open = preferences_local.getInt("current_open", 0);
  Serial.println(current_open);
  
  current_close = preferences_local.getInt("current_close", 0);
  Serial.println(current_close);

  move_distance = preferences_local.getFloat("move_distance", 300000);
  Serial.println(move_distance);

  move_percent = preferences_local.getFloat("move_percent", 300000);
  Serial.println(move_percent);

  velocity = preferences_local.getLong("velocity", 120000);
  Serial.println(velocity);

  reverse_flag = preferences_local.getBool("reverse_flag", true);
  Serial.println(reverse_flag);

  
  Serial.println("Loading timetables...");
  for(int i=0;i<4;i++)load_time(i); // load time configuration
  Serial.println("Ready!");

  check_timer=millis()+10000;
  daylight_timer=millis()+9000;

  for(int i=0;i<4;i++){
    if(times[i].active)
      last_timezone_offset=times[i].offset;
  }
   
  BlynkProvisioning.begin();
}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved

void IndependentTask( void * parameter ) { // the internet should not be used AT ALL in this function!!!!

while(true) {
    // buttons
    // A press sets the command to open or close the track motor.
    
    if(!digitalRead(btn1)){
      delay(100);
      if(!digitalRead(btn1)){
      Serial.println("BTN1 Pressed");
      command = MOVE_CLOSE;
     }
    }
    
    if(!digitalRead(btn2)){
      delay(100);
      if(!digitalRead(btn2)){
      Serial.println("BTN2 Pressed");
      command = MOVE_OPEN;
     }
    }
    
    if(command!=-1){
      Serial.print("Executing command ");
      Serial.println(command);
    }
    if(command==MOVE_CLOSE){
      Serial.println("MOVE CLOSE STARTING");
      move_close();
      last_dir=DIR_CLOSE;
    }
    if(command==MOVE_OPEN){
      Serial.println("MOVE OPEN STARTING");
      move_open();
      last_dir=DIR_OPEN;
    }
    
    if(command!=-1)Serial.println("[ready for next movement]");
    command = -1;
    delay(16);
  }
}

void loop() {
  
BlynkProvisioning.run();
blynkTimer.run();

if(MQTT_ON == true){
  if(MQTT_SETUP == false){
//MQTT setup Run Once
delay(10);
client.setServer(mqtt_server.c_str(), 1883);
client.setCallback(callback);
MQTT_SETUP = true;
  }
//MQTT loop
 timer.run();
 if (!client.connected()) {
   reconnect();
  }
 client.loop();
}


  if(check_timer<millis()){
    check_timer=millis()+55000; // check every 55 seconds. (avoid missing minutes)
    // get the current time
    struct tm ctime;
    ctime.tm_hour=hour();
    ctime.tm_min=minute();
    Serial.print(" ---- Time is ");
    Serial.print(ctime.tm_hour);
    Serial.print(":");
    Serial.print(ctime.tm_min);
    Serial.println(" ----");

    if(ctime.tm_hour==2){
      daylight_timer=millis()+10; // trigger very soon to get daylight data back
    }

  for(int i=0;i<4;i++){
      Serial.print("Timer: ");
      Serial.print(i);
      Serial.print(" is ");
      Serial.println((bool)times[i].active);
      if(!times[i].active)continue;
      Serial.print("  Check is ");
      Serial.print(times[i].hour);
      Serial.print(":");
      Serial.println(times[i].minute);
      // check time first (lest likely to match first = fastest computation)
      if(ctime.tm_min==times[i].minute&&ctime.tm_hour==times[i].hour){
        // check week day (day_sel is days since monday, tm_wday is days since sunday)
        if(times[i].day_sel[(ctime.tm_wday+6)%7]){
          // odd indexes close and even indexes open
          if(i%2==0){
            // even (open)
            if(last_dir!=DIR_OPEN)
              command = MOVE_OPEN;
              Serial.println("Timer Open");
              
          }else{
            // odd (close)
            if(last_dir!=DIR_CLOSE)
              command = MOVE_CLOSE;
              Serial.println("Timer Close");
          }
        }
      }
    }
  }
}
  

void clockout_setup(){
  periph_module_enable(PERIPH_LEDC_MODULE);
  uint32_t bit_width = 2; // 1 - 20 bits
  uint32_t divider = 320; // Q10.8 fixed point number, 0x100 — 0x3FFFF
  uint32_t duty_cycle = 1 << (bit_width - 1);
  float freq_mhz = ((uint64_t) LEDC_APB_CLK_HZ << 8) / (double) divider / (1 << bit_width) / 1000000.0;
  printf("\nfrequency: %f MHz\n", freq_mhz);
  ledc_timer_set(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, divider, bit_width, LEDC_APB_CLK);
  ledc_timer_rst(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
  ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
  ledc_channel_config_t pwm_pin_cfg = {
  CLOCKOUT, // chosen GPIO output for clock
  LEDC_HIGH_SPEED_MODE, // speed mode
  LEDC_CHANNEL_0, // ledc channel
  LEDC_INTR_DISABLE, // interrupt type
  LEDC_TIMER_0, // timer select
  duty_cycle // duty cycle
  };
  ledc_channel_config(&pwm_pin_cfg);
}


void save_time(int i){
  Serial.print("Saving time #");
  Serial.println(i);
  if(times[i].type==0){
    Serial.print(" > Active at ");
    Serial.print(times[i].hour);
    Serial.print(":");
    Serial.println(times[i].minute);
  }else if(times[i].type==1){
    Serial.println(" > Active at sunrise.");
  }else
    Serial.println(" > Active at sunset.");
  // make one string variable to save CPU power and memory
  String num_=String(i); 
  preferences_local.putUInt(("hour_"+num_).c_str(), times[i].hour);
  preferences_local.putUInt(("minute_"+num_).c_str(), times[i].minute);
  preferences_local.putUInt(("type_"+num_).c_str(), times[i].type);
  preferences_local.putLong(("offset_"+num_).c_str(), times[i].offset);
  preferences_local.putUChar(("active_"+num_).c_str(), times[i].active);
  for(int n=0;n<7;n++){
    preferences_local.putUChar(("day_sel_"+num_+"_"+n).c_str(), times[i].day_sel[n]);
  }
}


void load_time(int i){
  // make one string variable to save CPU power and memory
  String num_=String(i); 
  Serial.print("Reloading timer ");
  Serial.println(i);
  times[i].hour=preferences_local.getUInt(("hour_"+num_).c_str(),12);
  times[i].minute=preferences_local.getUInt(("minute_"+num_).c_str(), 0);
  times[i].type=preferences_local.getUInt(("type_"+num_).c_str(), 0);
  times[i].offset=preferences_local.getLong(("offset_"+num_).c_str(), -25200);
  // only set the first two as active by default
  times[i].active=preferences_local.getUChar(("active_"+num_).c_str(), 0);
  for(int n=0;n<7;n++){
    // by default select all days.
    times[i].day_sel[n]=preferences_local.getUChar(("day_sel_"+num_+"_"+n).c_str(), 1);
  }
}
