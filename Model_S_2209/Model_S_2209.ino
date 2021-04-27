#define USE_CUSTOM_BOARD            // See "Custom board configuration" in Settings.h
#define APP_DEBUG                   // Comment this out to disable debug prints
#define BLYNK_SSL_USE_LETSENCRYPT   // Comment this out, if using public Blynk Cloud
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG


//Comment out to use string
//#define Model_W


#include <Arduino.h>
#include <AccelStepper.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>
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
#include "Preferences.h"

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);


TMC2209Stepper driver2(&SERIAL_PORT_2, R_SENSE , DRIVER_ADDRESS);

#include "Functions.h"
#include "MQTT.h"
#include "BlynkPins.h"

#ifdef Model_W
#include "MotorControl_window.h"
#else
#include "MotorControl.h"
#endif

//#include "motor_calibration_new.h" //Not yet
#include "OTA_S3.h"

TaskHandle_t C0;

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
              stepper.moveTo((max_steps/100)*timer_open_percent);
              command = CUSTOM_MOVE;
              Serial.println("Timer Open"); 
          }else{
            // odd (close)
            Serial.println("4");
              stepper.moveTo(0);
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

  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STALLGUARD ,INPUT);
  pinMode(btn1,INPUT);
  pinMode(btn2,INPUT);

  disableCore0WDT();
   
  xTaskCreatePinnedToCore(
   core0assignments,       // Function that should be called
   "Core_0",               // Name of the task (for debugging)
   8192,                   // Stack size (bytes)
   NULL,                   // Parameter to pass
   1,                      // Task priority
   &C0,                    // Task handle
   0);                     // Core you want to run the task on (0 or 1)

  for(int i=0;i<4;i++)load_time(i); // load time configuration

  check_timer=millis()+10000;
  daylight_timer=millis()+9000;

  for(int i=0;i<4;i++){
    if(times[i].active)
      last_timezone_offset=times[i].offset;
  }
  
  schedulerId = blynkTimer.setInterval(35000, scheduler);
  blynkTimer.disable(schedulerId);
  
  load_preferences();
  setup_motors();
  stepper.setCurrentPosition(XACTUAL);
  BlynkProvisioning.begin();
}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved




void loop() {

// START BUTTONS
// A press sets the command to open or close the motor.

    
    
    delay(10);
    
    if(digitalRead(btn1)==LOW){
      stepper.moveTo(max_steps);
      command = CUSTOM_MOVE; 
      Serial.print("btn1: ");
      Serial.println(command);
    }
    
    else if(digitalRead(btn2)==LOW){
      stepper.moveTo(0);
      command = CUSTOM_MOVE;
      Serial.println("btn2: ");
      Serial.print(command);
    }
    
 //END BUTTTONS

 
    if(command==STOP){
      stepper.setAcceleration(200000);
      stepper.moveTo(stepper.currentPosition());
      command = -1;
      
    }else if(command==CUSTOM_MOVE){
      move_motor();
      command = -1;
    }

    #ifdef Model_W
    
    else if(command==POSITION_CLOSE){
      motor_running = true;
      position_close();
      command = -1;
    }else if(command==POSITION_OPEN){
      motor_running = true;
      position_open();
      command = -1;
    }else if(command==POSITION_ADJUST){
      motor_running = true;
      position_adjust();
      command = -1;
    }
    
    #else
    
    else if(command==FORCE_CLOSE){
      motor_running = true;
      force_close();
      setZero();
      command = -1;
    }
    
    #endif
    
}

unsigned long lastMillis;

void core0assignments( void * pvParameters ) { 
for (;;) {

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



#ifdef Model_W

closeState = digitalRead(SENSOR1);

  // compare the buttonState to its previous state
  if (closeState != lastcloseState) {
    // if the state has changed, increment the counter
    if (closeState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      Blynk.virtualWrite(V23, 100);
      Serial.println("DEVICE OPENED");
    } else {
      // if the current state is LOW then the button went from on to off:
      XACTUAL=0;
      preferences_local.putInt("XACTUAL", 0);
      stepper.setCurrentPosition(XACTUAL);
      Blynk.virtualWrite(V23, 0);
      Serial.println("DEVICE CLOSED");
      move_close_stall=false;
      move_open_stall=false;
      String payload;
      payload = "CLOSED";
      if(client.connected()){
      client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
      }
    }
  }

lastcloseState = closeState;


openState = digitalRead(SENSOR2);

  // compare the state to its previous state
  if (openState != lastopenState) {
    // if the state has changed, increment the counter
    if (openState == LOW) {
      if (distance_cal==false){
      XACTUAL=max_steps;
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
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("CLEARED STALL");
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
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("STALL CLEARED");
      Blynk.virtualWrite(V46, "OPENED");
      }
    }
  
  // save the current state as the last state, for next time through the loop
  lastStallOpenState = stallOpenState;
#endif

}
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
  
  //Blynk.virtualWrite(V25, open_current_calibration_value);
  //Blynk.virtualWrite(V26, close_current_calibration_value);
  //Blynk.virtualWrite(V121, open_stall_calibration_value);
  //Blynk.virtualWrite(V122, close_stall_calibration_value);
  
  Blynk.virtualWrite(V61, stepValue);
  Blynk.virtualWrite(V34, MOVE_VELOCITY/divider);
  Blynk.virtualWrite(V21, MOVE_ACCEL/divider);
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
  
  float update_open_current =((q+1)/(float)32)*(0.325/(R_SENSE+0.02))*(1/sqrt(2));
  
  Blynk.virtualWrite(V56, (float)update_open_current);

  q=close_current_calibration_value;
  float update_close_current =((q+1)/(float)32)*(0.325/(R_SENSE+0.02))*(1/sqrt(2));
  Blynk.virtualWrite(V55, (float)update_close_current);

  blynk_update=false;
}
