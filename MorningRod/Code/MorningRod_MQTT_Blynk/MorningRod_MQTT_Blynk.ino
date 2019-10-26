/* This firmware is primarily used with the MorningRod app and allows for automatic provisioning. 
 *  
 * If you would like to add more features, you will need to change a few lines to work with the Blynk app.
 * 
 * //MorningRod-Mode// is used for automatic provisioning
 * 
 * //Blynk-Mode is used for custom Blynk apps
 */

#define USE_CUSTOM_BOARD // See "Custom board configuration" in Settings.h
#define APP_DEBUG        // Comment this out to disable debug prints
#define MOVE_DISTANCE preferences.getFloat("move_distance", 0)
#define MOVE_PERCENT preferences.getFloat("move_percent", 389911.13)
#define BLYNK_PRINT Serial

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

//#define DEBUG_STREAM terminal
#define DEBUG_STREAM Serial

WidgetTerminal terminal(V2);
#include "motor_control.h"
#include "blynk_pins.h"
#include "GPS.h"
#include "OTA_S3.h"
#include "pins.h"

TaskHandle_t TaskA;

const char* ntpServer = "pool.ntp.org"; // where to get the current date and time
String sunsetsunrisebaseurl="https://api.sunrise-sunset.org/json?formatted=0&lng="; //Sunrise/sunset API address

// Blynk Wireless settings: Used in Blynk-Mode
const char AUTH[] = "AUTH TOKEN"; //Your Blynk Auth Token
const char SSID[] = "WIFI"; //Your 2.4GHz WiFi ID
const char PASSWORD[] = "PASSWORDntony"; // Your WiFi Password

//MQTT Setup
#include <SimpleTimer.h>
SimpleTimer timer;
int counter = 0;
long int last_UP_change = millis();

//MQTT Settings
IPAddress mqtt_server(192,168,50,178); 
const char* mqtt_username = "USERNAME";
const char* mqtt_password = "PASSWORD";
char* InTopic = "room/curtain"; //subscribe to topic to be notified about
char* InTopic1 = "room";
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) { //Used for MQTT
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on the topic esp32/output, you check if the message is either "open" or "close". 
  // Changes the output state according to the message
  if (String(topic) == "room/curtain") {
    Serial.print("Changing output to ");
    if(messageTemp == "open"){
      Serial.println("open");
      command = MOVE_OPEN;
    }
    else if(messageTemp == "close"){
      Serial.println("close");
      command = MOVE_CLOSE;
    }
  }
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
   if (client.connect("MorningRod",mqtt_username,mqtt_password)) {
     Serial.println("connected");
     counter = 0;
     // ... and resubscribe
     client.subscribe(InTopic);
     client.subscribe(InTopic1);
   } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 0.3 second");
     ++counter;
     if (counter > 180) ESP.restart();
     // Wait .3 seconds before retrying
     delay(300);
   }
 }
}

//SETUP
void setup() {

  Serial.begin(115200);

  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);

  // for storing the times over power cycle
  preferences.begin("auto-curtain", false);

  clockout_setup();
  setup_motors();
   
  xTaskCreatePinnedToCore(
   IndependentTask,        /* pvTaskCode */
   "NoBlynkSafe",          /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   0);                     /* xCoreID */  

//Blynk-Mode//
WiFi.mode(WIFI_STA);
//Use this for Blynk server
Blynk.begin(AUTH, SSID, PASSWORD);
//Use this for MorningRod server
//Blynk.begin(AUTH, SSID, PASSWORD, "morningrod.blynk.cc", 8080);
  while (Blynk.connect() == false) {  // Wait until connected
    if ((millis() - last_UP_change) > 30000) { // 30s before reset
      Serial.println(F("resetting"));
      ESP.restart();
   }
 }
 
//MQTT setup
delay(10);
client.setServer(mqtt_server, 1883);
client.setCallback(callback);

//End Blynk-Mode//

  //Use For MorningRod App Provisioning Mode
  /*
  BlynkProvisioning.begin();
  Blynk.sendInternal("rtc", "sync"); 
  DEBUG_STREAM.println("Connected!");
  Blynk.syncAll();
  */
  //End MorningRod-Mode//

  
  lat=preferences.getFloat("lat", -1);
  lon=preferences.getFloat("lon", -1);
  DEBUG_STREAM.println("Loading timetables...");
  for(int i=0;i<4;i++)load_time(i); // load time configuration
  sun_delay = preferences.getInt("sun_delay",0); // load the delay time (in minutes) between sunset/sunrise and open/close
  //reset_motors(1);
  DEBUG_STREAM.println("Ready!");
  //sendData(0x21,0); Not sure what this does

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

void IndependentTask( void * parameter ) {
  // the internet should not be used AT ALL in this function!!!!
while(true) {
    // buttons

    // A press sets the command to open or close the track motor.
    if(!digitalRead(btn1)){
      command = MOVE_CLOSE;
    }
    if(!digitalRead(btn2)){
      command = MOVE_OPEN;
    }
    
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
      last_dir=DIR_CLOSE;
    }else if(command==MOVE_OPEN){
      move_open();
      last_dir=DIR_OPEN;
    }
    if(command!=-1)DEBUG_STREAM.println("[ready for next movement]");
    command = -1;
    delay(16);
  }
}

void loop() {
// MorningRod-Mode//
  //BlynkProvisioning.run();
//End MorningRod-Mode//
  
//Blynk-Mode//
Blynk.run();
 if(!Blynk.connected()) {
   Serial.println(F("Resetting in loop"));
   ESP.restart();
 }

//MQTT
 timer.run();
 if (!client.connected()) {
   reconnect();
  }
 client.loop();
//End Blynk-Mode// 

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
        //lcd.clear(); //Use it to clear the LCD Widget
      }else{
       //lcd.clear(); //Use it to clear the LCD Widget
   
   //lcd.print(4, 0, "Status:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  //lcd.print(4, 1, "World");
  // Please use timed events when LCD printintg in void loop to avoid sending too many commands
  // It will cause a FLOOD Error, and connection will be dropped
      }
      //Blynk.syncAll();
    }
  }
  
  if(check_timer<millis()){
    check_timer=millis()+55000; // check every 55 seconds. (avoid missing minutes)
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
      DEBUG_STREAM.print("Timer: ");
      DEBUG_STREAM.print(i);
      DEBUG_STREAM.print(" is ");
      DEBUG_STREAM.println((bool)times[i].active);
      // don't waste CPU time
      if(!times[i].active)continue;

      // check for different time zones
      /*if(times[i].offset!=last_timezone_offset){
        // reconfigure clock
        DEBUG_STREAM.print("Clock is being reconfigured...\nNew time zone=");
        DEBUG_STREAM.println(times[i].offset);
        configTime(times[i].offset, 0, ntpServer, "time.nist.gov", "time.windows.com");
        last_timezone_offset=times[i].offset;
        // get the current time
        getLocalTime(&ctime);
        DEBUG_STREAM.print(" ---- Time is ");
        DEBUG_STREAM.print(ctime.tm_hour);
        DEBUG_STREAM.print(":");
        DEBUG_STREAM.print(ctime.tm_min);
        DEBUG_STREAM.println(" ----");
      }*/
      
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
          // odd indexes close and even indexes open
          if(i%2==0){
            // even (open)
            if(last_dir!=DIR_OPEN)
              command = MOVE_OPEN;
          }else{
            // odd (close)
            if(last_dir!=DIR_CLOSE)
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
