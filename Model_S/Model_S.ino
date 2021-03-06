#define USE_CUSTOM_BOARD            // See "Custom board configuration" in Settings.h
#define APP_DEBUG                   // Comment this out to disable debug prints
#define BLYNK_SSL_USE_LETSENCRYPT   // Comment this out, if using public Blynk Cloud
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#include <Arduino.h>
#include "Variables.h"
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
#include "TimeStore.cpp"
#include "time.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include <Update.h>
#include "Pins.h"
#include "Functions.h"
#include "MQTT.h"
#include "BlynkPins.h"
#include "MotorControl.h"
//#include "motor_calibration.h"
#include "OTA_S3.h"

TaskHandle_t TaskA;

void scheduler() {

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
        //if(times[i].day_sel[(ctime.tm_wday+6)%7]){
 
          // odd indexes close and even indexes open
          if(i%2==0){
            // even (open)
              move_to_position = (max_steps/100)*timer_open_percent;
              command = CUSTOM_MOVE;
              Serial.println("Timer Open"); 
          }else{
            // odd (close)
            Serial.println("4");
              move_to_position = 0;
              command = CUSTOM_MOVE;
              Serial.println("Timer Close");
          }
        }
        
      //}
    }
}


void setup() {
  
  delay(100);
  Serial.begin(115200);
  Serial.println("Starting SETUP");

  preferences_local.begin("local", false);

  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);
  
   xTaskCreatePinnedToCore(
   IndependentTask,        // Function that should be called
   "NoBlynkSafe",          // Name of the task (for debugging)
   8192,                   // Stack size (bytes)
   NULL,                   // Parameter to pass
   1,                      // Task priority
   &TaskA,                 // Task handle
   0);                     // Core you want to run the task on (0 or 1)


  Serial.println("Loading timetables...");
  for(int i=0;i<4;i++)load_time(i); // load time configuration
  Serial.println("Ready!");

  check_timer=millis()+10000;
  daylight_timer=millis()+9000;

  for(int i=0;i<4;i++){
    if(times[i].active)
      last_timezone_offset=times[i].offset;
  }
  
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
  
  mqtt_set_topic = preferences_local.getString("set_topic", "404");

  mqtt_state_topic = preferences_local.getString("state_topic", "404");

  mqtt_discovery_topic = preferences_local.getString("disc_topic", "404");

  disc_prefix = preferences_local.getString("disc_prefix","homeassistant");

  MQTT_ON = preferences_local.getBool("mqtt_on", false);
    
  stall_close = preferences_local.getLong("stall_close", 4128768);
  Serial.println(stall_close);
  
  stall_open = preferences_local.getLong("stall_open", 4128768);
  Serial.println(stall_open);

  current_open = preferences_local.getInt("current_open", 0);
  Serial.println(current_open);
  
  current_close = preferences_local.getInt("current_close", 0);
  Serial.println(current_close);

  open_stall_calibration = preferences_local.getInt("OStallCal", 4128768);
  open_stall_calibration_value = preferences_local.getInt("2StallCalVal", 63);
  close_stall_calibration = preferences_local.getInt("CStallCal", 4128768);
  close_stall_calibration_value = preferences_local.getInt("1StallCalVal", 63);

  open_current_calibration = preferences_local.getInt("O_cur_cal", 512);
  open_current_calibration_value = preferences_local.getInt("2_cur_cal_val", 4);
  close_current_calibration = preferences_local.getInt("C_cur_cal", 512);
  close_current_calibration_value = preferences_local.getInt("1_cur_cal_val", 4);

  move_distance = preferences_local.getFloat("move_distance", 300000);
  Serial.println(move_distance);

  move_percent = preferences_local.getFloat("move_percent", 300000);
  Serial.println(move_percent);

  velocity = preferences_local.getLong("velocity", 120000);
  Serial.println(velocity);

  CLOSE_POSITION = preferences_local.getInt("close_pos", 1);
  Serial.print("Preferences Close Position: ");
  Serial.println(CLOSE_POSITION);

  move_to_position = preferences_local.getInt("move_to_position", 0); //3102236
  Serial.print("Preferences move_to_position: ");
  Serial.println(move_to_position);

  max_steps = preferences_local.getInt("max_steps", 0);
  Serial.print("Preferences max_steps: ");
  Serial.println(max_steps);

  XACTUAL = preferences_local.getInt("XACTUAL", 0);
  Serial.print("XACTUAL Position: ");
  Serial.println(XACTUAL);

  reverse_flag = preferences_local.getBool("reverse_flag", false);
  
  schedulerId = blynkTimer.setInterval(35000, scheduler);
  blynkTimer.disable(schedulerId);
  timer_open_percent = preferences_local.getInt("timer_perc", 100);

  clockout_setup();
  setup_motors();
  
  BlynkProvisioning.begin();
}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved


void IndependentTask( void * parameter ) { // the internet should not be used AT ALL in this function!!!!
while(true) {

    // buttons
    // A press sets the command to open or close the track motor.
    
    if(digitalRead(btn1)==LOW){
      move_to_position = max_steps-100000;
      command = CUSTOM_MOVE; 
      Serial.println(command);
    }
    
    if(digitalRead(btn2)==LOW){
      move_to_position = 0;
      command = CUSTOM_MOVE;
      Serial.println(command);
    }

    if(command!=-1){
      Serial.print("Executing command ");
      Serial.println(command);
    
    }if(command==STOP){
      delay(100);
    }else if(command==CUSTOM_MOVE){
      motor_running = true;
      move_motor_position_1();
    }else if(command==FORCE_CLOSE){
      motor_running = true;
      force_close();
    }


    

//    }else if (command==STEP_1){
//    
//      Serial.println("Step 1 Cal");
//      new_auto_calibrate_step_1();
//
//    }else if (command==STEP_2){
//
//      new_auto_calibrate_step_2();
//
//    }else if (command==STEP_3){
//
//      new_auto_calibrate_step_3();
//
//    }else if(command==STEP_4){
//
//      new_auto_calibrate_step_4();
//
//    }else if(command==STEP_5){
//
//      new_auto_calibrate_step_5();
//
//    } else if(command==STEP_6){
//
//      new_auto_calibrate_step_6();
//
//    } 
 
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
 client.loop();
}

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

//Pushes parameters to Blynk
if(blynk_update==true){
  blynkUpdate();
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

void blynkUpdate(){

  Blynk.virtualWrite(V15, MQTT_ON);
  Blynk.virtualWrite(V16, mqtt_name);
  Blynk.virtualWrite(V17, mqtt_server);
  Blynk.virtualWrite(V18, mqtt_username);
  Blynk.virtualWrite(V19, mqtt_password);
  Blynk.virtualWrite(V20, disc_prefix);
  Blynk.virtualWrite(V24, timer_open_percent);
  
  Blynk.virtualWrite(V25, open_current_calibration_value);
  Blynk.virtualWrite(V26, close_current_calibration_value);
  Blynk.virtualWrite(V121, open_stall_calibration_value);
  Blynk.virtualWrite(V122, close_stall_calibration_value);
  
  Blynk.virtualWrite(V61, stepValue);
  
  Blynk.virtualWrite(V34, velocity/1000);

  Blynk.virtualWrite(V36, CLOSE_POSITION);

 
  float display_inches = (float)max_steps/(float)one_inch;
  //Blynk.virtualWrite(V22, ((float)max_steps/(float)(inches_mm * full_steps_mm * microsteps * gear_ratio)));

  Blynk.virtualWrite(V22, ("%.2f", display_inches));

  Blynk.virtualWrite(V73, max_steps);
  Blynk.virtualWrite(V71, XACTUAL);

  if(XACTUAL<=0){
    slider_XACTUAL = 0;
    Blynk.virtualWrite(V46, "CLOSED");
  }else{
    slider_XACTUAL = XACTUAL;
    Blynk.virtualWrite(V46, "OPEN");
  }
  
  Serial.print("XACTUAL: ");
  Serial.println(slider_XACTUAL);

  Serial.print("max_steps: ");
  Serial.println(max_steps);

  slider_display = (((float)slider_XACTUAL/(float)max_steps)*100);
  
  Serial.print("slider_display: ");
  Serial.println(slider_display);

  Blynk.virtualWrite(V23, int(slider_display + 0.5));
  
  long rssi = WiFi.RSSI();
  Blynk.virtualWrite(V70, rssi);
  Blynk.virtualWrite(V3, 3);

  if(CLOSE_POSITION==1){
  Blynk.virtualWrite(V42, 1);
  }else if(CLOSE_POSITION==2){
  Blynk.virtualWrite(V42, 2);
  }

  Blynk.virtualWrite(V46, "CLOSED");
  
  int q=open_current_calibration_value;
  float update_open_current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V56, (float)update_open_current);

  q=close_current_calibration_value;
  float update_close_current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V55, (float)update_close_current);

  blynk_update=false;
}
