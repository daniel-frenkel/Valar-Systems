#define USE_CUSTOM_BOARD            // See "Custom board configuration" in Settings.h
#define APP_DEBUG                   // Comment this out to disable debug prints
#define BLYNK_SSL_USE_LETSENCRYPT   // Comment this out, if using public Blynk Cloud
#define BLYNK_PRINT Serial

//COMMENT OUT THE WRONG ONE!!!
//#define Model_D
#define Model_W

//#define BLYNK_DEBUG

bool AUTO_OPEN = false;
bool AUTO_OPEN_BLYNK = false;
int AUTO_CLOSE_DELAY;

#include <Arduino.h>
#include "Variables.h"
#include "Structures.h"
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
#include "time.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include <Update.h>
#include "pins.h"
#include "blynk_pins.h"
#include "Blynk_MQTT.h"
#include "motor_control.h"
#include "motor_calibration.h"
#include "OTA_S3.h"

TaskHandle_t TaskA;

int freq = 1000;
int time_on = 300;
int channel = 13;
int resolution = 16;

void dogBuzzer() {
  ledcWriteTone(channel, freq);
  unsigned long time_now;
  time_now = millis();
  Blynk.notify("{DEVICE_NAME} NOTIFYING DOG!");
  Serial.println("NOTIFY DOG!");
  while(millis() < time_now + time_on){
  ledcWrite(channel, 255);
  }
  ledcWrite(channel, 0);
}


BLYNK_WRITE(V13) { // set number of seconds
  Serial.print("set buzzer seconds");
  int q=param.asInt();
  dogInterval = q*1000;
  dogBuzzerId = blynkTimer.setInterval(dogInterval, dogBuzzer);
  preferences_local.putInt("dogInterval", dogInterval);
  Serial.println(dogInterval);
}

void buzzerTone() {
  ledcWriteTone(channel, freq);
  unsigned long time_now;
  time_now = millis();
  //Blynk.notify("{DEVICE_NAME} STALLED!"); //Notify only a few times
  Serial.println("STALLED!(buzzerTone)");
  while(millis() < time_now + time_on){
  ledcWrite(channel, 255);
  }
  ledcWrite(channel, 0);
}



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

  Serial.begin(115200);
  preferences_local.begin("local", false);
  pinMode(position_2_sensor, INPUT);
  pinMode(position_1_sensor, INPUT);
  pinMode(buzzer_pin, OUTPUT);

  xTaskCreatePinnedToCore(
   IndependentTask,        /* pvTaskCode */
   "Motor_Functions",          /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   0);                     /* xCoreID */  
  

  Serial.println("Loading timetables...");
  for(int i=0;i<4;i++)load_time(i); // load time configuration
  Serial.println("Ready!");

  check_timer=millis()+10000;
  daylight_timer=millis()+9000;

  for(int i=0;i<4;i++){
    if(times[i].active)
      last_timezone_offset=times[i].offset;
  }

  //Load MQTT configs from preferences
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
  
  open_stall_calibration_high = preferences_local.getInt("OStallCalHi", 4128768);
  open_stall_calibration_value_high = preferences_local.getInt("2StallCalValHi", 63);
  close_stall_calibration_high = preferences_local.getInt("CStallCalHi", 4128768);
  close_stall_calibration_value_high = preferences_local.getInt("1StallCalValHi", 63);

  open_stall_calibration_low = preferences_local.getInt("OStallCalLo", 4128768);
  open_stall_calibration_value_low = preferences_local.getInt("2StallCalValLo", 63);
  close_stall_calibration_low = preferences_local.getInt("CStallCalLo", 4128768);
  close_stall_calibration_value_low = preferences_local.getInt("1StallCalValLo", 63);

  open_current_calibration_high = preferences_local.getInt("O_cur_cal_h", 512);
  open_current_calibration_value_high = preferences_local.getInt("2_cur_cal_val_h", 4);
  close_current_calibration_high = preferences_local.getInt("C_cur_cal_h", 512);
  close_current_calibration_value_high = preferences_local.getInt("1_cur_cal_val_h", 4);

  open_current_calibration_low = preferences_local.getInt("O_cur_cal_val_l", 512);
  open_current_calibration_value_low = preferences_local.getInt("O_cur_cal_val_l", 4);
  close_current_calibration_low = preferences_local.getInt("C_cur_cal_val_l", 512);
  close_current_calibration_value_low = preferences_local.getInt("C_cur_cal_val_l", 4);

  stall_close_high = preferences_local.getInt("stall_close_hi", 4128768);
  Serial.print("Preferences stall_close: ");
  Serial.println(stall_close_high);
  
  stall_open_high = preferences_local.getInt("stall_open_hi", 4128768);
  Serial.print("Preferences stall_open: ");
  Serial.println(stall_open_high);

  stall_close_low = preferences_local.getInt("stall_close_lo", 4128768);
  Serial.print("Preferences stall_close: ");
  Serial.println(stall_close_low);
  
  stall_open_low = preferences_local.getInt("stall_open_lo", 4128768);
  Serial.print("Preferences stall_open: ");
  Serial.println(stall_open_low);
  
  open_current_high = preferences_local.getInt("open_current_h", current_setup);
  Serial.print("Preferences open_current: ");
  Serial.println(open_current_high);

  close_current_high = preferences_local.getInt("close_current_h", current_setup);
  Serial.print("Preferences close_current: ");
  Serial.println(close_current_high);

  open_current_low = preferences_local.getInt("open_current_l", current_setup);
  Serial.print("Preferences open_current: ");
  Serial.println(open_current_low);

  close_current_low = preferences_local.getInt("close_current_l", current_setup);
  Serial.print("Preferences close_current: ");
  Serial.println(close_current_low);

  move_to_position = preferences_local.getInt("move_to_position", 0); //3102236
  Serial.print("Preferences move_to_position: ");
  Serial.println(move_to_position);

  max_steps = preferences_local.getInt("max_steps", 0);
  Serial.print("Preferences max_steps: ");
  Serial.println(max_steps);

  XACTUAL = preferences_local.getInt("XACTUAL", 0);
  Serial.print("XACTUAL Position: ");
  Serial.println(XACTUAL);

  CLOSE_POSITION = preferences_local.getInt("close_pos", 1);
  Serial.print("Preferences Close Position: ");
  Serial.println(CLOSE_POSITION);

  OPEN_CLOSE_position = preferences_local.getFloat("open_close_pos", 18*one_inch);
  AUTO_CLOSE_DELAY = preferences_local.getInt("aut_close_del", 2000);
  AUTO_OPEN_BLYNK = preferences_local.getInt("auto_open_perm", 0);
  dogInterval = preferences_local.getInt("dogInterval", 3000);

  just_open_position = preferences_local.getInt("just_open", 1 * one_inch);
  Serial.print("just_open_position: ");
  Serial.println(just_open_position);

  fast_loud = preferences_local.getBool("fast_loud", true);
  commandBuzzer = preferences_local.getBool("commandBuzzer", false);
  stall_buzzer_blynk = preferences_local.getBool("stallBuzz", false);
  timer_open_percent = preferences_local.getInt("timer_perc", 100);

  clockout_setup();
  setup_motors();

  ledcSetup(channel, freq, resolution); // 12 kHz PWM, 12-bit resolution
  ledcAttachPin(buzzer_pin, channel); // assign RGB led pins to channels
  
  buzzerId = blynkTimer.setInterval(3000, buzzerTone);
  blynkTimer.disable(buzzerId);

  dogBuzzerId = blynkTimer.setInterval(dogInterval, dogBuzzer);
  blynkTimer.disable(dogBuzzerId);

  schedulerId = blynkTimer.setInterval(35000, scheduler);
  blynkTimer.disable(schedulerId);
  
  dogInterval = preferences_local.getInt("dogInterval", 5000);

  Serial.println("END SETUP");

  Serial.println("BEGIN PROVISION");
  BlynkProvisioning.begin();
}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved


void IndependentTask( void * parameter ){  // Core for motor tasks
while(true) {

    #ifdef Model_W
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
      
      buzzer_flag = false;
      motor_running = true;
    
      if(CLOSE_POSITION==1){
      move_motor_position_1();
      }else if(CLOSE_POSITION==2){
      move_motor_position_1(); //CHANGE BACK TO 2
      }

    }
  #endif 

  #ifdef Model_D
    if(command!=-1){
      Serial.print("Executing command ");
      Serial.println(command);
      
    }if(command==STOP){
      delay(100);
      
    }else if(command==CUSTOM_MOVE){
      Serial.print("Starting CUSTOM MOVE to position: ");
      Serial.println(move_to_position);
      buzzer_flag = false;
      motor_running = true;
      AUTO_OPEN = false; //Need to turn off AUTO_OPEN feature first
//    last_dir=DIR_OPEN;
      move_motor_position_1();
      Serial.println("COMMAND: CUSTOM_MOVE complete: ");   

      int x = 0;
      if(move_close_stall==true){
      Serial.println("Starting OPEN_CLOSE on stall");
      move_to_position = OPEN_CLOSE_position;
      move_motor_position_1();
      move_to_position = 0;
      move_motor_position_1();
      x = x+1;
      }
    }else if (command==OPEN_CLOSE){
      Serial.println("Starting OPEN_CLOSE");
      motor_running = true;
      AUTO_OPEN = false; //Need to turn off AUTO_OPEN feature
      move_to_position = OPEN_CLOSE_position;
      move_motor_position_1();
      Serial.println("OPEN_CLOSE Delaying");
      delay(AUTO_CLOSE_DELAY);
      Serial.println("OPEN_CLOSE closing");
      move_to_position = 0;
      move_motor_position_1();
      
      int x = 0;
      if(restart_movement==true){
      Serial.println("Starting OPEN_CLOSE on stall");
      move_to_position = OPEN_CLOSE_position;
      move_motor_position_1();
      move_to_position = 0;
      move_motor_position_1();
      x = x+1;
      }
      
      AUTO_OPEN = AUTO_OPEN_BLYNK; //Sets the state of AUTO OPEN back to what was set in Blynk  
      motor_running = false;
    }
    #endif
    
    else if (command==POSITION_ADJUST){
      Serial.println("Starting POSITION_ADJUST");
      position_adjust();
    }else if (command==POSITION_CLOSE){
      Serial.println("Starting POSITION_CLOSE");
      position_close();
    }else if (command==POSITION_OPEN){
      Serial.println("Starting POSITION_OPEN");
      position_open();
    }else if (command==STEP_1){
      AUTO_OPEN=false;
      Serial.println("Step 1 Cal");
      new_auto_calibrate_step_1();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    }else if (command==STEP_2){
      AUTO_OPEN=false;
      new_auto_calibrate_step_2();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    }else if (command==STEP_3){
      AUTO_OPEN=false;
      new_auto_calibrate_step_3();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    }else if(command==STEP_4){
      AUTO_OPEN=false;
      new_auto_calibrate_step_4();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    }else if(command==STEP_5){
      AUTO_OPEN=false;
      new_auto_calibrate_step_5();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    } else if(command==STEP_6){
      AUTO_OPEN=false;
      new_auto_calibrate_step_6();
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
    }  else if(command==AUTO_TUNE){
      AUTO_OPEN=false;
      new_auto_calibrate_step_1();
      new_auto_calibrate_step_3(); //High Current
      new_auto_calibrate_step_4(); //Low Current
      new_auto_calibrate_step_5(); //High Stall
      new_auto_calibrate_step_6(); //Low Stall
      position_close();
      AUTO_OPEN=AUTO_OPEN_BLYNK;
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
  client.setCallback(callback); // Sends message "0" when not required
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
  

closeState = digitalRead(position_1_sensor);

  // compare the buttonState to its previous state
  if (closeState != lastcloseState) {
    // if the state has changed, increment the counter
    if (closeState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      Blynk.virtualWrite(V46, "OPEN");;
      Serial.println("DEVICE OPENED");
        if(AUTO_OPEN==true){
        command = OPEN_CLOSE;
        }
    } else {
      // if the current state is LOW then the button went from on to off:
      XACTUAL=0;
      sendData(0x21+0x80, 0);      // XACTUAL=0
      sendData(0x2D+0x80, 0);      // XTARGET=0
      preferences_local.putInt("XACTUAL", 0);
      Blynk.virtualWrite(V46, "CLOSED");;
      Serial.println("DEVICE CLOSED");
      buzzer_flag = false;
      AUTO_OPEN = AUTO_OPEN_BLYNK;
      move_close_stall=false;
      move_open_stall=false;
      blynkTimer.disable(buzzerId); 
      String payload;
      payload = "CLOSED";
      if(client.connected()){
      client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
      }
    }
  }

lastcloseState = closeState;


openState = digitalRead(position_2_sensor);

  // compare the state to its previous state
  if (openState != lastopenState) {
    // if the state has changed, increment the counter
    if (openState == LOW) {
      if (distance_cal==false){
      XACTUAL=max_steps;
      sendData(0x21+0x80, max_steps);      // XACTUAL=0
      sendData(0x2D+0x80, max_steps);      // XTARGET=0
      preferences_local.putInt("XACTUAL", max_steps);
      }
    }
  }
  
  
  // save the current state as the last state, for next time through the loop
  lastopenState = openState;



  // read the pushbutton input pin:
  stallCloseState = move_close_stall;

  // compare the buttonState to its previous state
  if (stallCloseState != lastStallCloseState) {
    // if the state has changed, increment the counter
    if (stallCloseState == true) {
      // if the current state is HIGH then the button went from off to on:
      Blynk.virtualWrite(V46, "STALLED");
      Blynk.virtualWrite(V3 , 5);
      Serial.println("STALLED CLOSED");
      if(stall_buzzer_blynk == true){
        blynkTimer.enable(buzzerId);
        }
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("CLEARED STALL");
      blynkTimer.disable(buzzerId); 
      if (closeState == HIGH) {
      Blynk.virtualWrite(V46, "OPENED");
     }
    }
  }
  
  // save the current state as the last state, for next time through the loop
  lastStallCloseState = stallCloseState;

  // read the pushbutton input pin:
  stallOpenState = move_open_stall;

  // compare the buttonState to its previous state
if (stallOpenState != lastStallOpenState) {
    // if the state has changed, increment the counter
    if (stallOpenState == true) {
      // if the current state is HIGH then the button went from off to on:
      Blynk.virtualWrite(V46, "STALLED");
      Blynk.virtualWrite(V3 , 5);
      Serial.println("STALLED OPEN");
      if(stall_buzzer_blynk == true){
      blynkTimer.enable(buzzerId);
      }
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("STALL CLEARED");
      blynkTimer.disable(buzzerId);
      if (closeState == HIGH) {
      Blynk.virtualWrite(V46, "OPENED");
      }
    }
  }
  
  // save the current state as the last state, for next time through the loop
  lastStallOpenState = stallOpenState;

}



void blynkUpdate(){

  Blynk.virtualWrite(V15, MQTT_ON);
  Blynk.virtualWrite(V16, mqtt_name);
  Blynk.virtualWrite(V17, mqtt_server);
  Blynk.virtualWrite(V18, mqtt_username);
  Blynk.virtualWrite(V19, mqtt_password);
  Blynk.virtualWrite(V20, disc_prefix);
  Blynk.virtualWrite(V24, timer_open_percent);
  
  Blynk.virtualWrite(V25, open_current_calibration_value_high);
  Blynk.virtualWrite(V26, close_current_calibration_value_high);
  Blynk.virtualWrite(V29, open_current_calibration_value_low);
  Blynk.virtualWrite(V28, close_current_calibration_value_low);
  
  Blynk.virtualWrite(V123, open_stall_calibration_value_low);
  Blynk.virtualWrite(V124, close_stall_calibration_value_low);
  Blynk.virtualWrite(V121, open_stall_calibration_value_high);
  Blynk.virtualWrite(V122, close_stall_calibration_value_high);
  Blynk.virtualWrite(V61, stepValue);
  
 
  Blynk.virtualWrite(V41, AUTO_CLOSE_DELAY/1000);

  Blynk.virtualWrite(V38, (int)(OPEN_CLOSE_position/one_inch));
    
  float display_inches = (float)max_steps/(float)one_inch;
  //Blynk.virtualWrite(V22, ((float)max_steps/(float)(inches_mm * full_steps_mm * microsteps * gear_ratio)));

  Blynk.virtualWrite(V22, ("%.2f", display_inches));

  Blynk.virtualWrite(V73, max_steps);
  Blynk.virtualWrite(V72, just_open_position);
  Blynk.virtualWrite(V71, XACTUAL);
  Blynk.virtualWrite(V74, move_to_position);
  

  if(XACTUAL<0){
    slider_XACTUAL = 0;
  }else{
    slider_XACTUAL = XACTUAL;
  }
  
  Serial.print("XACTUAL: ");
  Serial.println(slider_XACTUAL);

  Serial.print("max_steps: ");
  Serial.println(max_steps);

  slider_display = ((float)slider_XACTUAL/(float)max_steps)*100;
  
  Serial.print("slider_display: ");
  Serial.println(slider_display);

  Blynk.virtualWrite(V23, int(slider_display));
  
  long rssi = WiFi.RSSI();
  Blynk.virtualWrite(V70, rssi);

  Blynk.virtualWrite(V3, 5);

  if(CLOSE_POSITION==1){
  Blynk.virtualWrite(V42, 1);
  }else if(CLOSE_POSITION==2){
  Blynk.virtualWrite(V42, 2);
  }

  if(commandBuzzer == false){
  Blynk.virtualWrite(V53, 1);
  }else{
  Blynk.virtualWrite(V53, 2);
  }

  if(stall_buzzer_blynk == false){
  Blynk.virtualWrite(V50, 1);
  }else{
  Blynk.virtualWrite(V50, 2);
  }

  #ifdef Model_W

  if(fast_loud == false){
  Blynk.virtualWrite(V43, 1);
  }else{
  Blynk.virtualWrite(V43, 2);
  }
  
  int q=open_current_calibration_value_high;
  float update_open_current_high =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V56, (float)update_open_current_high);

  q=close_current_calibration_value_high;
  float update_close_current_high =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V55, (float)update_close_current_high);

  q=open_current_calibration_value_low;
  float update_open_current_low =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V58, (float)update_open_current_low);

  q=close_current_calibration_value_low;
  float update_close_current_low =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V57, (float)update_close_current_low);
  #endif


  #ifdef Model_D
  int q=open_current_calibration_value_high;
  float update_open_current_high =(255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Blynk.virtualWrite(V56, (float)update_open_current_high);

  q=close_current_calibration_value_high;
  float update_close_current_high =(255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Blynk.virtualWrite(V55, (float)update_close_current_high);

  q=open_current_calibration_value_low;
  float update_open_current_low =(255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Blynk.virtualWrite(V58, (float)update_open_current_low);

  q=close_current_calibration_value_low;
  float update_close_current_low =(255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Blynk.virtualWrite(V57, (float)update_close_current_low);
  #endif

 
  blynk_update=false;
  
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
