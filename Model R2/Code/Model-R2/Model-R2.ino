#define USE_CUSTOM_BOARD          // See "Custom board configuration" in Settings.h

#define APP_DEBUG        // Comment this out to disable debug prints

#define BLYNK_PRINT Serial

#include "BlynkProvisioning.h"

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
#define COOLCONF_DEFAULT 0
#define DEBUG_STREAM Serial//terminal

WidgetTerminal terminal(V2);

WiFiClient client;

// Variables to validate
// response from S3
int contentLength = 0;
bool isValidContentType = false;

// S3 Bucket Config
String host = "morningrodupdate.s3.us-east-1.amazonaws.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "/double_window_curtain.ino.bin"; // bin file name with a slash in front.

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

TaskHandle_t TaskA;

int chipCS = 21;
int CLOCKOUT = 18;
int enable = 5;
int btn1=9, btn2=10;

const char* ntpServer = "pool.ntp.org"; // where to get the current date and time

float lat;
float lon;
String sunsetsunrisebaseurl="https://api.sunrise-sunset.org/json?formatted=0&lng=";

WidgetRTC rtc;
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}
long check_timer = 0;
long daylight_timer = 0;
time_store times[4];
long last_timezone_offset=-1;
int temp_pin = 34;



// OTA Logic 
void execOTA() {
  
  DEBUG_STREAM.println("Connecting to: " + String(host));
  // Connect to S3
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    DEBUG_STREAM.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    DEBUG_STREAM.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        DEBUG_STREAM.println("Client Timeout !");
        client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          DEBUG_STREAM.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
        DEBUG_STREAM.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        DEBUG_STREAM.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // Please try again
    // Probably a choppy network?
    DEBUG_STREAM.println("Connection to " + String(host) + " failed. Please try again");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  DEBUG_STREAM.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      DEBUG_STREAM.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        DEBUG_STREAM.println("Written : " + String(written) + " successfully");
      } else {
        DEBUG_STREAM.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        DEBUG_STREAM.println("OTA done!");
        if (Update.isFinished()) {
          DEBUG_STREAM.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          DEBUG_STREAM.println("Update not finished? Something went wrong!");
        }
      } else {
        DEBUG_STREAM.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      DEBUG_STREAM.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    DEBUG_STREAM.println("There was no content in the response");
    client.flush();
  }
}

int sun_delay = 0;

//SETUP
void setup() {
//  delay(500);
  Serial.begin(115200);

  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);

  // for storing the times over power cycle
  preferences.begin("auto-curtain", false);

  setup_motors();
  xTaskCreatePinnedToCore(
   IndependentTask,                  /* pvTaskCode */
   "NoBlynkSafe",            /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   1);                     /* xCoreID */ 
  
  BlynkProvisioning.begin();

  Blynk.sendInternal("rtc", "sync"); 

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

int q=0;
int q_ls=0;
bool stalled=false;

time_store sunrise;
time_store sunset;
/*
long led_state_timer;         // millisecond time for next LED state
long led_off_timer;           // time that the LED gets turned off, in milliseconds

int led_direction=0;          // -1 means getting dimmer, 1 means brightening
int current_brightness=0;     // scale 0-1023
int target_brightness=0;      // scale 0-1023
int led_timestep=880;         // timestep in milliseconds for LED state timer
*/
long stall_timer=100000L;

boolean one_stall = false;
boolean two_stall = false;

boolean tmp_opened=false, tmp_closed=false;

void safeDelay(long time){
  time+=millis();
  do{
    unsigned long m1_raw=sendData(0x7F,0);
    unsigned long m2_raw=sendData(0x6F,0);
    one_stall = one_stall||(((m1_raw>>24)&1)==1);
    two_stall = two_stall||(((m2_raw>>24)&1)==1);
    bool stall = one_stall&&two_stall;
    
    if((stall||(stall_timer<millis()))&&(!stalled)){
      digitalWrite(enable,HIGH);
      stalled=true;
    }
  }while(millis()<time);
}

void IndependentTask( void * parameter )
{

  // the internet should not be used AT ALL in this function!!!!
 while(true) {
  safeDelay(0);
  // buttons
  if(!digitalRead(btn1)){
    q=1;
  }
  if(!digitalRead(btn2)){
    q=2;
  }
  
  if(q_ls!=q){
    if(!stalled){
      // if we are changing direction during a motion then force direction change
      reset_motors(q);
      safeDelay(10);
    }
    reset_motors(q);
    q_ls=q;
    stalled=false;
    safeDelay(200);
  }
 }
}


void loop() {
  // This handles the network and cloud connection
  BlynkProvisioning.run();
  
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
          // Activate!  Change direction!
          q=(1+(1&i)); // odd indexes close and even indexes open
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

BLYNK_WRITE(V1) { // Update OTA
  if(param.asInt()!=0){
    execOTA(); // Executes OTA Update
  }
}

BLYNK_WRITE(V64) { // sunrise/sunset delay
  sun_delay = param.asInt();
  preferences.putInt("sun_delay", sun_delay);
}
BLYNK_WRITE(V12) { // open now
  if(param.asInt()!=0){
    q=DIR_OPEN; // tell control loop what to do
  }
}
BLYNK_WRITE(V13) { // close now
  if(param.asInt()!=0){
    q=DIR_CLOSE; // tell control loop what to do
  }
}
BLYNK_WRITE(V5) { // open1
  times[0].active=param.asInt()!=0;
  preferences.putUChar("active_0", times[0].active);
}
BLYNK_WRITE(V7) { // close1
  times[1].active=param.asInt()!=0;
  preferences.putUChar("active_1", times[0].active);
}

BLYNK_WRITE(V9) { // open2
  times[2].active=param.asInt()!=0;
  preferences.putUChar("active_2", times[0].active);
}
BLYNK_WRITE(V11) { // close2
  times[3].active=param.asInt()!=0;
  preferences.putUChar("active_3", times[0].active);
}


BLYNK_WRITE(V31) { // set acceleration value left
  DEBUG_STREAM.print("set acceleration 1: ");
  long q=param.asInt()*1000L;
  preferences.putLong("accel_1", q);
  DEBUG_STREAM.println(q);
  sendData(0xA6, q);     // AMAX_M1
}
BLYNK_WRITE(V32) { // set velocity value left
  DEBUG_STREAM.print("set velocity 1: ");
  long q=param.asInt()*1000;
  preferences.putLong("velocity_1", q);
  DEBUG_STREAM.println(q);
  sendData(0xA7, q);     // VMAX_M1
}
BLYNK_WRITE(V33) { // set acceleration value right
  DEBUG_STREAM.print("set acceleration: ");
  long q=param.asInt()*1000L;
  preferences.putLong("accel_2", q);
  DEBUG_STREAM.println(q);
  sendData(0xC6, q);     // AMAX_M2
}
BLYNK_WRITE(V34) { // set velocity value right
  DEBUG_STREAM.print("set velocity: ");
  long q=param.asInt()*1000;
  preferences.putLong("velocity_2", q);
  DEBUG_STREAM.println(q);
  sendData(0xC7, q);     // VMAX_M2
}
BLYNK_WRITE(V123) { // set stallguard value left
  DEBUG_STREAM.print("set M1 stall: ");
  int q=param.asInt()-64;
  if(q>63)q=63;
  if(q<-64)q=-64;
  preferences.putInt("stallguard_1", q);
  DEBUG_STREAM.println(q);
  q&=0x7F;
  q=q<<16;
  sendData(0xED, COOLCONF_DEFAULT|q);     // STALLGUARD_M1
}
BLYNK_WRITE(V124) { // set stallguard value right
  DEBUG_STREAM.print("set M2 stall: ");
  int q=param.asInt()-64;
  DEBUG_STREAM.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  preferences.putInt("stallguard_2", q);
  q&=0x7F;
  q=q<<16;
  sendData(0xFD, COOLCONF_DEFAULT|q);     // STALLGUARD_M2
}

void reset_motors(int dir){
  digitalWrite(enable,LOW);
  DEBUG_STREAM.print("Setting motion direction to ");
  DEBUG_STREAM.println(dir);
  sendData(0xB4, 0x000);              // reset stallguard
  sendData(0xD4, 0x000);              // reset stallguard
  sendData(0xA0, dir);                // RAMPMODE_M1
  sendData(0xC0, dir);                // RAMPMODE_M2
  delay(2);
  //for(int i=0;i<2;i++){
    sendData(0xB4, 0x000);              // reset stallguard
    sendData(0xD4, 0x000);              // reset stallguard
    sendData(0xA0, dir);                // RAMPMODE_M1
    sendData(0xC0, dir);                // RAMPMODE_M2
    sendData(0xB4, 0x400);              // make stallguard stop the motor
    sendData(0xD4, 0x400);              // make stallguard stop the motor
    //delay(2);
  //}
  stall_timer=millis()+6000;
  one_stall=false;
  two_stall=false;
}
void setup_motors(){
  // put your setup code here, to run once:
  pinMode(chipCS,OUTPUT);
  pinMode(CLOCKOUT,OUTPUT);
  pinMode(enable, OUTPUT);
  digitalWrite(chipCS,HIGH);
  digitalWrite(enable,HIGH);
  //digitalWrite(enable,LOW);
  
  /*  Dylan Brophy from Upwork, code to create 16 MHz clock output
  *  
  *  This code will print the frequency output when the program starts
  *  so that it is easy to verify the output frequency is correct.
  *  
  *  This code finds the board clock speed using compiler macros; no need
  *  to specify 80 or 40 MHz.
  */

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin(22,23,19,21); // SCLK, MISO, MOSI, SS
  sendData(0xB4, 0x000);          // make stallguard stop the motor
  sendData(0xD4, 0x000);          // make stallguard stop the motor
  
  sendData(0x80, 0x00000300);     // GCONF
  
  //2-phase configuration Motor 1
  sendData(0xEC, 0x01010134);     // CHOPCONF_M1
  sendData(0xB0, 0x00001600);     // IHOLD_IRUN_M1
  
  //2-phase configuration Motor 2
  
  sendData(0xFC, 0x01010134);     // CHOPCONF_M2 // My configuration
  sendData(0xD0, 0x00001600);     // IHOLD_IRUN_M2
  
  //Standard values for speed and acceleration
  int q=preferences.getInt("stallguard_1", 0x10);
  DEBUG_STREAM.print("Stall1 value: ");
  DEBUG_STREAM.println(q);
  q&=0x7F;
  q=q<<16;
  sendData(0xED, COOLCONF_DEFAULT|q);     // STALLGUARD_M1
  q=preferences.getInt("stallguard_2", 0x10);
  DEBUG_STREAM.print("Stall2 value: ");
  DEBUG_STREAM.println(q);
  q&=0x7F;
  q=q<<16;
  sendData(0xFD, COOLCONF_DEFAULT|q);     // STALLGUARD_M2
  //DEBUG_STREAM.print("Acceleration value: ");
  //DEBUG_STREAM.println(q);
  sendData(0xA6, preferences.getLong("accel_1",0x96));     // AMAX_M1
  sendData(0xC6, preferences.getLong("accel_2",0x96));     // AMAX_M2
  
  //DEBUG_STREAM.print("Velocity value: ");
  //DEBUG_STREAM.println(q);
  
  sendData(0xA7, preferences.getLong("velocity_1",100000));          // VMAX_M1
  sendData(0xC7, preferences.getLong("velocity_2",100000));          // VMAX_M2
}


unsigned long sendData(unsigned long address, unsigned long datagram)

{
 //TMC5130 takes 40 bit data: 8 address and 32 data
 
 delay(10);
 uint8_t stat;
  unsigned long i_datagram=0;
 
 digitalWrite(chipCS,LOW);
 delayMicroseconds(10);
 
 stat = SPI.transfer(address);
 
 i_datagram |= SPI.transfer((datagram >> 24) & 0xff);
 i_datagram <<= 8;
 i_datagram |= SPI.transfer((datagram >> 16) & 0xff);
 i_datagram <<= 8;
 i_datagram |= SPI.transfer((datagram >> 8) & 0xff);
 i_datagram <<= 8;
 i_datagram |= SPI.transfer((datagram) & 0xff);
 digitalWrite(chipCS,HIGH);
 return i_datagram;
}


BLYNK_WRITE(V4) {
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime())
  {
    times[0].type=0;
    times[0].hour=t.getStartHour();
    times[0].minute=t.getStartMinute();
    times[0].second=t.getStartSecond();
  }
  else if (t.isStartSunrise())
  {
    times[0].type=1;
  }
  else if (t.isStartSunset())
  {
    times[0].type=2;
  }
  else
  {
    // Do nothing
  }
  for (int i = 1; i <= 7; i++) {
    times[0].day_sel[i-1]=t.isWeekdaySelected(i);
  }
  times[0].offset=t.getTZ_Offset();
  save_time(0);
}
BLYNK_WRITE(V6) {
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime())
  {
    times[1].type=0;
    times[1].hour=t.getStartHour();
    times[1].minute=t.getStartMinute();
    times[1].second=t.getStartSecond();
  }
  else if (t.isStartSunrise())
  {
    times[1].type=1;
  }
  else if (t.isStartSunset())
  {
    times[1].type=2;
  }
  else
  {
    // Do nothing
  }
  for (int i = 1; i <= 7; i++) {
    times[1].day_sel[i-1]=t.isWeekdaySelected(i);
  }
  times[1].offset=t.getTZ_Offset();
  save_time(1);
}
BLYNK_WRITE(V8) {
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime())
  {
    times[2].type=0;
    times[2].hour=t.getStartHour();
    times[2].minute=t.getStartMinute();
    times[2].second=t.getStartSecond();
  }
  else if (t.isStartSunrise())
  {
    times[2].type=1;
  }
  else if (t.isStartSunset())
  {
    times[2].type=2;
  }
  else
  {
    // Do nothing
  }
  for (int i = 1; i <= 7; i++) {
    times[2].day_sel[i-1]=t.isWeekdaySelected(i);
  }
  times[2].offset=t.getTZ_Offset();
  save_time(2);
}
BLYNK_WRITE(V10) {
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime())
  {
    times[3].type=0;
    times[3].hour=t.getStartHour();
    times[3].minute=t.getStartMinute();
    times[3].second=t.getStartSecond();
  }
  else if (t.isStartSunrise())
  {
    times[3].type=1;
  }
  else if (t.isStartSunset())
  {
    times[3].type=2;
  }
  else
  {
    // Do nothing
  }
  for (int i = 1; i <= 7; i++) {
    times[3].day_sel[i-1]=t.isWeekdaySelected(i);
  }
  times[3].offset=t.getTZ_Offset();
  save_time(3);
}
// GPS code here
float lslat=-1; // NEVER use these variables in the code above, only below.  They are a security risk otherwise.
float lslon=-1;
BLYNK_WRITE(V127) {
  GpsParam gps(param);
  lslat=gps.getLat();
  lslon=gps.getLon();
}
BLYNK_WRITE(V126) {
  if(param.asInt()!=0){
    DEBUG_STREAM.println();
    DEBUG_STREAM.print("Lat: ");
    DEBUG_STREAM.println(lslat, 7);
    DEBUG_STREAM.print("Lon: ");
    DEBUG_STREAM.println(lslon, 7);
    if(lslat==-1||lslon==-1){
      DEBUG_STREAM.println("Invalid!");
    }else{
      lon=lslon;
      lat=lslat;
      preferences.putFloat("lat", lat);
      preferences.putFloat("lon", lon);
    }
  }
}
