#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "HTML.h"

Timezone myTZ;

const char *ap_ssid = "VALAR-AP";
const char *ap_password = "password";

String ip_address;
String hostname = "val-1000";

AsyncWebServer server(80);

String splitTime(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void scheduleOpen(){

  Serial.println("Schedule Open Executed");
 
  // start motor!
  move_to_position = max_steps;
  run_motor = true;

  // set next time to wakeup
  openEvent = myTZ.setEvent(scheduleOpen, myTZ.now()+24*3600);

  Serial.println("Schedule Open Complete");
}

void scheduleClose(){

  Serial.println("Schedule Close Executed");
  
  // do the alarm thing!
  move_to_position = 0;
  run_motor = true;

  // set next time to wakeup
  closeEvent = myTZ.setEvent(scheduleClose, myTZ.now()+24*3600);

  Serial.println("Schedule Close Complete");
}

void startezTime(){

  if (WiFi.status() == WL_CONNECTED && (open_timer==1 || close_timer==1))
  {
  vTaskDelay(2000);
  setDebug(INFO);
  waitForSync();

  Serial.println();
  Serial.println("UTC: " + UTC.dateTime());

  myTZ.setLocation(MYTIMEZONE);
  Serial.print("Time in your set timezone: ");
  Serial.println(myTZ.dateTime());
  
    // determine close time
  time_t newOpenTime = makeTime(open_hour, open_minute, 0, myTZ.day(), myTZ.month(), myTZ.year());
  if (myTZ.now() >= newOpenTime) newOpenTime += 24*3600;
          
  // set next time to wakeup
  openEvent = myTZ.setEvent(scheduleOpen, newOpenTime);  
  
  // determine close time
  time_t newCloseTime = makeTime(close_hour, close_minute, 0, myTZ.day(), myTZ.month(), myTZ.year());
  if (myTZ.now() >= newCloseTime) newCloseTime += 24*3600;
          
  // set next time to wakeup
  closeEvent = myTZ.setEvent(scheduleClose, newCloseTime);  
  
  }
}

String processor(const String& var)
{
 
  if(var == "PLACEHOLDER_PERCENT"){
    return String(move_percent);
  }
  else if(var == "PLACEHOLDER_MAX_STEPS"){
    return String(max_steps);
  }
  else if(var == "PLACEHOLDER_CURRENT"){
    return String(current);
  }
  else if(var == "PLACEHOLDER_STALL"){
    return String(stall);
  }
  else if(var == "PLACEHOLDER_ACCEL"){
    return String(accel);
  }
  else if(var == "PLACEHOLDER_MAX_SPEED"){
    return String(max_speed);
  }
  else if(var == "PLACEHOLDER_IP_ADDRESS"){
    return String(ip_address);
  }
  else if(var == "PLACEHOLDER_TIMEZONE"){
    return String(MYTIMEZONE);
  }
  else if(var == "PLACEHOLDER_OPEN_TIME"){
    return String(open_time_string);
  }
  else if(var == "PLACEHOLDER_CLOSE_TIME"){
    return String(close_time_string);
  }
 
  return String();
}

void connectWifi(){
  WiFi.softAPdisconnect(true);
  Serial.println(ssid);
  Serial.println(pass);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); 
  WiFi.begin(ssid.c_str(), pass.c_str());

      while (WiFi.status() != WL_CONNECTED) {
            vTaskDelay(1000);
            Serial.print(".");

        if(wifi_button == true)
        {
        Serial.println("Wifi Pressed");
        button_change();
        wifi_button = false;
        }
      }
        
  Serial.println(WiFi.localIP());
  ip_address = WiFi.localIP().toString();
}

void API()
{
    //Preferences library create varaiable to save
  Serial.print("wifi_set: ");
  Serial.println(wifi_set);
  
  if (wifi_set == 0) 
  {
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  ip_address = WiFi.softAPIP().toString();
  }
  else if (wifi_set == 1)
  {
  connectWifi();
  }
  
  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", WIFI_HTML);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", SETTINGS_HTML, processor);
  });

  server.on("/set_wifi", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
  if (request->hasParam("ssid")) {
      ssid = request->getParam("ssid")->value().c_str();
      preferences.putString ("ssid", ssid);
      Serial.print("ssid: ");
      Serial.println(ssid);
      Serial.print("ssid preferences: ");
      Serial.println(preferences.getString ("ssid", "NO_SSID"));
    }
    
  if (request->hasParam("pass")) {
      pass = request->getParam("pass")->value().c_str();
      preferences.putString ("pass", pass);
      wifi_set = 1;
      preferences.putInt ("wifi_set", 1);
      
      Serial.println(pass);
      Serial.println(preferences.getString ("pass", "NO_PASSWORD"));
      
      request->send(200, "text/html", "WiFi Credentials Set. Connect to your home WiFi network, find the IP address of this device, and go to http://NEW-IP-ADDRESS");

      connectWifi();

    }
                                  
  });

  server.on("/set_motor", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
    Serial.print("Params: ");
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    if(request->hasParam("max_steps"))
        {
          max_steps = request->getParam("max_steps")->value().toInt();
          preferences.putInt ("max_steps", max_steps);
          Serial.print("max_steps: ");
          Serial.println(max_steps);
        }
    if(request->hasParam("current"))
        {
          current = request->getParam("current")->value().toInt();
          preferences.putInt ("current", current);
          Serial.print("current: ");
          Serial.println(current);
        }
    if(request->hasParam("stall"))
        {
          stall = request->getParam("stall")->value().toInt();
          driver.SGTHRS(stall);
          preferences.putInt ("stall", stall);
          Serial.print("stall: ");
          Serial.println(stall);
        }
    if(request->hasParam("accel"))
        {
          accel = request->getParam("accel")->value().toInt();
          stepper.setAcceleration(accel);
          preferences.putInt ("accel", accel);
          Serial.print("accel: ");
          Serial.println(accel);
        }
    if(request->hasParam("max_speed"))
        {
          max_speed = request->getParam("max_speed")->value().toInt();
          stepper.setMaxSpeed(max_speed);
          preferences.putInt ("max_speed", max_speed);
          Serial.print("max_speed: ");
          Serial.println(max_speed);
        }

    request->redirect("/");
    
  });

server.on("/set_zero", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    set_zero = 1;
    Serial.print("set_zero: ");
    Serial.println(set_zero);   

    request->redirect("/");
  
  });

server.on("/schedule", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    if(request->hasParam("timezone"))
        {
          MYTIMEZONE = request->getParam("timezone")->value().c_str();
          preferences.putString ("timezone", MYTIMEZONE);
          Serial.print("timezone: ");
          Serial.println(MYTIMEZONE);
          myTZ.setLocation(MYTIMEZONE);
          Serial.print("Time in your set timezone: ");
          Serial.println(myTZ.dateTime());
  
        }
    if(request->hasParam("open_time"))
        {
          open_time_string = request->getParam("open_time")->value().c_str();
          preferences.putString ("open_string", open_time_string);
          String open_hour_s = splitTime(open_time_string, ':', 0);
          open_hour = open_hour_s.toInt();
          preferences.putInt ("open_hour", open_hour);
          String open_minute_s = splitTime(open_time_string, ':', 1);
          open_minute = open_minute_s.toInt();
          preferences.putInt ("open_minute", open_minute);
        }
    if(request->hasParam("open_timer"))
        {
          open_timer = request->getParam("open_timer")->value().toInt();
          if (open_timer == 1){
            
          // determine open time
          time_t newOpenTime = makeTime(open_hour, open_minute, 0, myTZ.day(), myTZ.month(), myTZ.year());
          
          // set next time to wakeup
          openEventNow = myTZ.setEvent(scheduleOpen, newOpenTime);
          
          
          }else{
            deleteEvent(openEvent); 
            deleteEvent(openEventNow);
          }
          
          preferences.putInt ("open_timer", open_timer);
        }     

    if(request->hasParam("close_time"))
        {
          close_time_string = request->getParam("close_time")->value().c_str();
          preferences.putString("close_string", close_time_string);
          String close_hour_s = splitTime(close_time_string, ':', 0);
          close_hour = close_hour_s.toInt();
          preferences.putInt ("close_hour", close_hour);
          String close_minute_s = splitTime(close_time_string, ':', 1);
          close_minute = close_minute_s.toInt();
          preferences.putInt ("close_minute", close_minute);
        }
    if(request->hasParam("close_timer"))
        {
          close_timer = request->getParam("close_timer")->value().toInt();
          if(close_timer == 1){

          // determine open time
          time_t newCloseTime = makeTime(close_hour, close_minute, 0, myTZ.day(), myTZ.month(), myTZ.year());
          
          // set next time to wakeup
         closeEventNow = myTZ.setEvent(scheduleClose, newCloseTime);

          
          }else{
            deleteEvent(closeEvent);
            deleteEvent(closeEventNow); 
          }
          preferences.putInt ("close_timer", close_timer);
        }

    startezTime();
        
    request->redirect("/");
  
  });

server.on("/position", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
  
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    if(request->hasParam("move_percent"))
    {
        move_percent = request->getParam("move_percent")->value().toInt();
        move_to_position = (max_steps/100)*move_percent;
        Serial.print("move_percent: ");
        Serial.println(move_percent); 
        Serial.print("move_to_position: ");
        Serial.println(move_to_position); 
        run_motor=true;
    }  
    request->redirect("/");  
});


server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){

DynamicJsonDocument doc(2048);
doc["position"] = stepper.currentPosition();

String json;
serializeJson(doc, json);

request->send(200, "application/json", json);
});



  server.begin();
}
