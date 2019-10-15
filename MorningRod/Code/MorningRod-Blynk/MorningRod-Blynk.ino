

#define APP_DEBUG        // Comment this out to disable debug prints

float MOVE_DISTANCE;
float MOVE_PERCENT = 389911.13;

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
#include <SPI.h> 
#include <WiFi.h> 
#include <HTTPClient.h> 
#include <ArduinoJson.h> 
#include <WiFiClientSecure.h> 
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <Update.h>
#include <Preferences.h>
#include "time_store.cpp"
#include "time.h"
#include "cred.h"
#include "pins.h"
#include "motor_control.h"
#include "blynk_pins.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/ledc.h"
#include "OTA_S3.h"

Preferences preferences;

#define DEBUG_STREAM terminal

WidgetTerminal terminal(V2);


TaskHandle_t TaskA;

const char* ntpServer = "pool.ntp.org"; // where to get the current date and time

String sunsetsunrisebaseurl="https://api.sunrise-sunset.org/json?formatted=0&lng=";


void setup() {
  Serial.begin(115200);

  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);

  // for storing the times over power cycle
  preferences.begin("auto-blinds", false);

  clockout_setup();
  setup_motors();
  xTaskCreatePinnedToCore(
   IndependentTask,                  /* pvTaskCode */
   "NoBlynkSafe",            /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   0);                     /* xCoreID */ 

  //delay(50000);
  Serial.println("Connecting...");

  #ifdef DANIEL_FRENKEL
    Blynk.begin(auth, ssid, pass, "morningrod.blynk.cc", 8080);
  #else
    Blynk.begin(auth, ssid, pass);
  #endif
  
  while(!Blynk.connected()){
    delay(300);
    Serial.print(".");
  }
 
  DEBUG_STREAM.println("Connected!");
  //setSyncInterval(10 * 60);
  //DEBUG_STREAM.println("Done.");
  
  lat=preferences.getFloat("lat", -1);
  lon=preferences.getFloat("lon", -1);
  DEBUG_STREAM.println("Loading timetables...");
  for(int i=0;i<4;i++)load_time(i); // load time configuration
  sun_delay = preferences.getInt("sun_delay",0); // load the delay time (in minutes) between sunset/sunrise and open/close
  //reset_motors(1);
  DEBUG_STREAM.println("Ready!");
  sendData(0x21,0);

  check_timer=millis()+10000;
  daylight_timer=millis()+9000;

  for(int i=0;i<4;i++){
    if(times[i].active)
      last_timezone_offset=times[i].offset;
  }
  //configTime(last_timezone_offset, 0, ntpServer, "time.nist.gov", "time.windows.com");
}

time_store sunrise;
time_store sunset;

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved

void IndependentTask( void * parameter ){  
  // the internet should not be used AT ALL in this function!!!!
  while(true) {
    // buttons

    // A press sets the command to open or close the track motor.
    if(!digitalRead(btn1)){
      command = MOVE_CLOSE;
      //move_close();
    }
    if(!digitalRead(btn2)){
      command = MOVE_OPEN;
      //move_open();
    }

    // shaft motor is M1, track motor is M2

    // commands are sent from other threads so that blocking function calls
    // (like trackClose(), shaftClose(), trackOpen(), and shaftOpen()) can be
    // called without causing bizzare hickups in the other threads, namely the main thread
    // which controls Blynk.
    if(command!=-1){
      DEBUG_STREAM.print("Executing command ");
      DEBUG_STREAM.println(command);
    }
    if(command==MOVE_CLOSE){
      move_close();
    }else if(command==MOVE_OPEN){
      move_open();
    }
    if(command!=-1)DEBUG_STREAM.println("[ready for next movement]");
    command = -1;
    delay(16);
  }//*/
}

void loop() {
  // This handles the network and cloud connection
  Blynk.run();
  // check if the direction changed
 
  if(preferences.getChar("last_dir",-1)!=last_dir){
    preferences.putChar("last_dir",last_dir);
    Serial.printf("Saving direction %i\n", last_dir);
    // check if Blynk is connected
    // If Blynk is not connected then do NOT send the signals, this is because it may stall the rest of the program.
    if(Blynk.connected()){
      Serial.printf("Writing direction %i\n", last_dir);
      // write 1023 because Blynk writes accept values 0-1023 to control brightness
      if(last_dir==DIR_OPEN){
        DEBUG_STREAM.println("(open)");
        Blynk.virtualWrite(V50, 0);
        Blynk.virtualWrite(V51, 1023);
        //close_led.off();
        //open_led.on();
      }else{
        DEBUG_STREAM.println("(close)");
        Blynk.virtualWrite(V50, 1023);
        Blynk.virtualWrite(V51, 0);
        //close_led.on();
        //open_led.off();
      }
      Blynk.syncAll();
    }
  }


  update_count++;
  if((update_timer+5000)<millis()){
    //DEBUG_STREAM.print("Updates: ");
    //DEBUG_STREAM.print(update_count);
    //DEBUG_STREAM.print(" in ");
    //DEBUG_STREAM.print(millis()-update_timer);
    //DEBUG_STREAM.print("ms.  UPS: ");
    //DEBUG_STREAM.println(update_count/((millis()-update_timer)/1000.0));
    update_count = 0;
    update_timer = millis();
  }
  
  if(check_timer<millis()){
    check_timer=millis()+20000; // check every 20 seconds. (avoid missing minutes)
    // get the current time
    struct tm ctime;
    ctime.tm_hour=hour();
    ctime.tm_min=minute();
    DEBUG_STREAM.print(" ---- Time is ");
    DEBUG_STREAM.print(ctime.tm_hour);
    DEBUG_STREAM.print(":");
    DEBUG_STREAM.print(ctime.tm_min);
    DEBUG_STREAM.println(" ----");

    if(ctime.tm_hour==2){
      daylight_timer=millis()+10; // trigger very soon to get daylight data back
    }

    DEBUG_STREAM.print("Timezone Offset: ");
    DEBUG_STREAM.println(last_timezone_offset);
    DEBUG_STREAM.print("GPS looks valid: ");
    DEBUG_STREAM.println((lon==0||lat==0||lon==-1||lat==-1) ? "NO" : "YES");
    
    for(int i=0;i<4;i++){
      DEBUG_STREAM.print("Check: ");
      DEBUG_STREAM.print(i);
      DEBUG_STREAM.print(" is ");
      DEBUG_STREAM.println((bool)times[i].active);
      // don't waste CPU time
      if(!times[i].active)continue;
      
      if(times[i].type==1){ // sunrise
        // change times[i] to target the sunrise
        times[i].minute=sunrise.minute;
        times[i].hour=sunrise.hour;

        // add the wait time for sunrise/sunset
        /*times[i].minute+=sun_delay;
        times[i].hour+=times[i].minute/60;
        times[i].minute=times[i].minute%60;*/
      }if(times[i].type==2){ // sunset
        // change times[i] to target the sunset
        times[i].minute=sunset.minute;
        times[i].hour=sunset.hour;

        // add the wait time for sunrise/sunset
        times[i].minute+=sun_delay;
        times[i].hour+=times[i].minute/60;
        times[i].minute=times[i].minute%60;
      }
      DEBUG_STREAM.print("  Check is ");
      DEBUG_STREAM.print(times[i].hour);
      DEBUG_STREAM.print(":");
      DEBUG_STREAM.println(times[i].minute);
      // check time first (lest likely to match first = fastest computation)
      if(ctime.tm_min==times[i].minute&&ctime.tm_hour==times[i].hour){
        // check week day (day_sel is days since monday, tm_wday is days since sunday)
        if(times[i].day_sel[(ctime.tm_wday+6)%7]){
          // Activate!  Change direction!

          // odd indexes close and even indexes open
          if(i%2==0){
            // even (open)
            command = MOVE_OPEN;
          }else{
            // odd (close)
            command = MOVE_CLOSE;
            
          }
        }
      }
    }
  }
  
  // find out what time the sunrise/sunset is once a day, super early in the morning
  if(daylight_timer<millis()){
    daylight_timer=millis()+86400000;
    HTTPClient http;
    http.begin(sunsetsunrisebaseurl+String(lon)+"&lat="+String(lat));
    
    if(http.GET()<0)return; // error
    const char* data=http.getString().c_str();
    StaticJsonDocument<800> doc;
    DeserializationError error = deserializeJson(doc, data);

    // Test if parsing succeeds.
    if (error) {
      DEBUG_STREAM.print(F("deserializeJson() failed: "));
      DEBUG_STREAM.println(error.c_str());
      return;
    }
  
    // Get the root object in the document
    JsonObject root = doc.as<JsonObject>()["results"];
    String sunrise_str = root["sunrise"]; // "2018-09-09T05:55:31+00:00"
    String sunset_str = root["sunset"]; // "2018-09-09T18:34:09+00:00"
    sunrise_str=sunrise_str.substring(11,16);
    sunset_str=sunset_str.substring(11,16);
    int sunset_hr=(sunset_str.substring(0,2).toInt()+(last_timezone_offset/3600)+24)%24;
    int sunrise_hr=(sunrise_str.substring(0,2).toInt()+(last_timezone_offset/3600)+24)%24;
    int sunset_min=sunset_str.substring(3).toInt();
    int sunrise_min=sunrise_str.substring(3).toInt();
    sunrise.hour=sunrise_hr;
    sunrise.minute=sunrise_min;
    sunset.hour=sunset_hr;
    sunset.minute=sunset_min;
    //last_timezone_offset=-1;// force update of timezone and time data
  } 
}

void save_time(int i){
  DEBUG_STREAM.print("Saving time #");
  DEBUG_STREAM.println(i);
  if(times[i].type==0){
    DEBUG_STREAM.print(" > Active at ");
    DEBUG_STREAM.print(times[i].hour);
    DEBUG_STREAM.print(":");
    DEBUG_STREAM.println(times[i].minute);
  }else if(times[i].type==1){
    DEBUG_STREAM.println(" > Active at sunrise.");
  }else
    DEBUG_STREAM.println(" > Active at sunset.");
  // make one string variable to save CPU power and memory
  String num_=String(i); 
  preferences.putUInt(("hour_"+num_).c_str(), times[i].hour);
  preferences.putUInt(("minute_"+num_).c_str(), times[i].minute);
  preferences.putUInt(("type_"+num_).c_str(), times[i].type);
  preferences.putLong(("offset_"+num_).c_str(), times[i].offset);
  preferences.putUChar(("active_"+num_).c_str(), times[i].active);
  for(int n=0;n<7;n++){
    preferences.putUChar(("day_sel_"+num_+"_"+n).c_str(), times[i].day_sel[n]);
  }
}
void load_time(int i){
  // make one string variable to save CPU power and memory
  String num_=String(i); 
  DEBUG_STREAM.print("Reloading timer ");
  DEBUG_STREAM.println(i);
  times[i].hour=preferences.getUInt(("hour_"+num_).c_str(),12);
  times[i].minute=preferences.getUInt(("minute_"+num_).c_str(), 0);
  times[i].type=preferences.getUInt(("type_"+num_).c_str(), 0);
  times[i].offset=preferences.getLong(("offset_"+num_).c_str(), -25200);
  // only set the first two as active by default
  times[i].active=preferences.getUChar(("active_"+num_).c_str(), 0);
  for(int n=0;n<7;n++){
    // by default select all days.
    times[i].day_sel[n]=preferences.getUChar(("day_sel_"+num_+"_"+n).c_str(), 1);
  }
}

void clockout_setup(){
  /* Dylan Brophy from Upwork, code to create 16 MHz clock output
  *
  * This code will print the frequency output when the program starts
  * so that it is easy to verify the output frequency is correct.
  *
  * This code finds the board clock speed using compiler macros; no need
  * to specify 80 or 40 MHz.
  */
  
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
