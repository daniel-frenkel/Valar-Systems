#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "HTML.h"

Preferences preferences;

const char *ap_ssid = "VALAR-AP";
const char *ap_password = "password";

int wifi_set;
bool wifi_button = false;;
String ssid;
String pass;
String ip_address;

AsyncWebServer server(80);

String processor(const String& var)
{
 
  if(var == "PLACEHOLDER_PERCENT"){
    return String(move_to_position);
  }
  else if(var == "PLACEHOLDER_MAX_STEPS"){
    return String(max_steps);
  }
  else if(var == "PLACEHOLDER_OPEN_CURRENT_HIGH"){
    return String(open_current_high);
  }
  else if(var == "PLACEHOLDER_OPEN_CURRENT_LOW"){
    return String(open_current_low);
  }
  else if(var == "PLACEHOLDER_CLOSE_CURRENT_HIGH"){
    return String(close_current_high);
  }
  else if(var == "PLACEHOLDER_CLOSE_CURRENT_LOW"){
    return String(close_current_low);
  }
  else if(var == "PLACEHOLDER_STALL_OPEN_HIGH"){
    return String(stall_open_high);
  }
  else if(var == "PLACEHOLDER_STALL_OPEN_LOW"){
    return String(stall_open_low);
  }
  else if(var == "PLACEHOLDER_STALL_CLOSE_HIGH"){
    return String(stall_close_high);
  }
  else if(var == "PLACEHOLDER_STALL_CLOSE_LOW"){
    return String(stall_close_low);
  }
  else if(var == "PLACEHOLDER_SPEED"){
    return String(speed_type);
  }
  else if(var == "PLACEHOLDER_IP_ADDRESS"){
    return String(ip_address);
  }
 
  return String();
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
  WiFi.softAPdisconnect(true);
  Serial.println(ssid);
  Serial.println(pass);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

      while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        
  Serial.println(WiFi.localIP());
  ip_address = WiFi.localIP().toString();
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
      WiFi.softAPdisconnect(true);
      delay(500);
      WiFi.begin(ssid.c_str(), pass.c_str());  

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


    
    if(request->hasParam("close_position"))
          {
        int q = request->getParam("close_position")->value().toInt();
        switch (q)
        {
          
        case 1: // Item 1 
          Serial.println("Position 1 Close");
          CLOSE_POSITION = 1;
          preferences.putInt("close_pos", 1);
          sendData(0x00+0x80, 0x14);
          break;
          
        case 2: // Item 2
          Serial.println("Position 2 Close");
          CLOSE_POSITION = 2;
          preferences.putInt("close_pos", 2);
          sendData(0x00+0x80, 0x04);
          break;
    
      }
    }
    
     if(request->hasParam("speed_type"))
        {
          int q = request->getParam("speed")->value().toInt();
          switch (q)
          {
          case 1: // SLOW SILENT
          Serial.println("SLOW/SILENT");
          fast_loud = false;
          MOVE_CLOSE_VELOCITY = 100000;
          MOVE_OPEN_VELOCITY = 100000;
          preferences.putBool("fast_loud", false);
          preferences.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
          preferences.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
          sendData(0x70 + 0x80, 0x000504C8);
             if(CLOSE_POSITION==2){
             sendData(0x00+0x80, 0x04);     // General settings /GCONF
             }else{
             sendData(0x00+0x80, 0x14);     // General settings /GCONF
             }
          
          break;
      
          case 2: //FAST LOUD
          Serial.println("FAST/LOUD");
          fast_loud = true;
          MOVE_OPEN_VELOCITY = 400000;
          MOVE_CLOSE_VELOCITY = 400000;
          preferences.putBool("fast_loud", true);
          preferences.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
          preferences.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
          sendData(0x70 + 0x80 , 0);
             if(CLOSE_POSITION==2){
             sendData(0x00+0x80, 0x00);     // General settings /GCONF
             }else{
             sendData(0x00+0x80, 0x10);     // General settings /GCONF
             }
            break;
        }
      }
              
    if(request->hasParam("open_high_current"))
        {
          Serial.print("set open high current: ");
          int q = request->getParam("open_high_current")->value().toInt();
          open_current_calibration_value_high=q;
          if(open_current_calibration_value_high>26)open_current_calibration_value_high=26;
          preferences.putInt("2_cur_cal_val_h", q);
          Serial.println(q);
          double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
          Serial.print("Current: ");
          Serial.print(current);
          Serial.println(" Amps");
          q&=0x1F;
          q=q<<8;
          open_current_high=q;
          preferences.putInt("open_current_h", q);
          Serial.println(open_current_high);
          }
        
    if(request->hasParam("close_high_current"))
        {
          Serial.print("set close high current: ");
          int q = request->getParam("close_high_current")->value().toInt();
          close_current_calibration_value_high=q;
          if(close_current_calibration_value_high>26)close_current_calibration_value_high=26;
          preferences.putInt("1_cur_cal_val_h", q);
          Serial.println(q);
          double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
          Serial.print("Current: ");
          Serial.print(current);
          Serial.println(" Amps");
          q&=0x1F;
          q=q<<8;
          close_current_high=q;
          preferences.putInt("close_current_h", q);
          Serial.println(close_current_high);
        }

    if(request->hasParam("open_low_current"))
        {
          Serial.print("set open low current: ");
          int q = request->getParam("open_low_current")->value().toInt();
          open_current_calibration_value_low=q;
          if(open_current_calibration_value_low>26)open_current_calibration_value_low=26;
          preferences.putInt("O_cur_cal_val_l", q);
          Serial.println(q);
          double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
          Serial.print("Current: ");
          Serial.print(current);
          Serial.println(" Amps");
          //Blynk.virtualWrite(V58, current);
          q&=0x1F;
          q=q<<8;
          open_current_low=q;
          preferences.putInt("open_current_l", q);
          Serial.println(open_current_low);
        }
        
    if(request->hasParam("close_low_current"))
        {
          Serial.print("set close low current: ");
          int q = request->getParam("close_low_current")->value().toInt();
          close_current_calibration_value_low=q;
          if(close_current_calibration_value_low>26)close_current_calibration_value_low=26;
          preferences.putInt("C_cur_cal_val_l", q);
          Serial.println(q);
          double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
          Serial.print("Current: ");
          Serial.print(current);
          Serial.println(" Amps");
          //Blynk.virtualWrite(V57, current);
          q&=0x1F;
          q=q<<8;
          close_current_low=q;
          preferences.putInt("close_current_l", q);
          Serial.println(close_current_low);
        }
        
    if(request->hasParam("stall_open_low"))
        {
          int q = request->getParam("stall_open_low")->value().toInt();
          Serial.print("set OPEN LOW stall: ");
          open_stall_calibration_value_low = q;
          preferences.putInt("2StallCalValLo", q);
          Serial.println(q);
          if(q>63)q=63;
          if(q<-64)q=-64;
          q&=0x7F;
          q=q<<16;
          stall_open_low = q;
          preferences.putInt("stall_open_lo", stall_open_low); 
          Serial.println(stall_open_low);
        }

    if(request->hasParam("stall_close_low"))
        {
          int q = request->getParam("stall_close_low")->value().toInt();
          Serial.print("set CLOSE stall: ");
          close_stall_calibration_value_low = q;
          preferences.putInt("1StallCalValLo", q);
          Serial.println(q);
          if(q>63)q=63;
          if(q<-64)q=-64;
          q&=0x7F;
          q=q<<16;
          stall_close_low = q;
          preferences.putInt("stall_close_lo", stall_close_low); 
          Serial.println(stall_close_low);
        }
     
     if(request->hasParam("stall_open_high"))
        {
          int q = request->getParam("stall_open_high")->value().toInt();
          Serial.print("set OPEN stall: ");
          open_stall_calibration_value_high = q;
          preferences.putInt("2StallCalValHi", q);
          Serial.println(q);
          if(q>63)q=63;
          if(q<-64)q=-64;
          q&=0x7F;
          q=q<<16;
          stall_open_high = q;
          preferences.putInt("stall_open_hi", stall_open_high); 
          Serial.println(stall_open_high);
        }

     if(request->hasParam("stall_close_high"))
        {
          int q = request->getParam("stall_close_high")->value().toInt();
          Serial.print("set CLOSE stall: ");
          close_stall_calibration_value_high = q;
          preferences.putInt("1StallCalValHi", q);
          Serial.println(q);
          if(q>63)q=63;
          if(q<-64)q=-64;
          q&=0x7F;
          q=q<<16;
          stall_close_high = q;
          preferences.putInt("stall_close_hi", stall_close_high); 
          Serial.println(stall_close_high);
        }

    
    if(request->hasParam("speed_type")) //Either 1 or 2
        {
        int q = request->getParam("speed_type")->value().toInt();

        switch(q)
        {  
        case 1: // SLOW SILENT
        Serial.println("SLOW/SILENT");
        fast_loud = false;
        MOVE_CLOSE_VELOCITY = 100000;          
        MOVE_OPEN_VELOCITY = 100000;
        preferences.putBool("fast_loud", false);
        preferences.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
        preferences.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
        sendData(0x70 + 0x80, 0x000504C8);
      
        if(CLOSE_POSITION==2){
        sendData(0x00+0x80, 0x04);     // General settings /GCONF
        }else{
        sendData(0x00+0x80, 0x14);     // General settings /GCONF
        }
    
        break;
    
        case 2: //FAST LOUD
        Serial.println("FAST/LOUD");
        fast_loud = true;
        MOVE_OPEN_VELOCITY = 400000;
        MOVE_CLOSE_VELOCITY = 400000;
        preferences.putBool("fast_loud", true);
        preferences.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
        preferences.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
        sendData(0x70 + 0x80 , 0);
        
        if(CLOSE_POSITION==2){
        sendData(0x00+0x80, 0x00);     // General settings /GCONF
        }else{
        sendData(0x00+0x80, 0x10);     // General settings /GCONF
        }
       
        break;
        }
     }  
    request->redirect("/");
    
  });


server.on("/position_open", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    command = POSITION_OPEN;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });


server.on("/position_close", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = POSITION_CLOSE;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });

server.on("/position_adjust", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = POSITION_ADJUST;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });
  

server.on("/step_1", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = STEP_1;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });
  

server.on("/step_3", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = STEP_3;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });


server.on("/step_4", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = STEP_4;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });


server.on("/step_5", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = STEP_5;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });

server.on("/step_6", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = STEP_6;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });


server.on("/auto_tune", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
 
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }
    
    command = AUTO_TUNE;
    Serial.print("command: ");
    Serial.println(command);   
    request->redirect("/");
  
  });
  

server.on("/position", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
  
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    if(request->hasParam("move_to_position"))
    {
        int i = request->getParam("move_to_position")->value().toInt();
        int q=(max_steps/100)*i;
        
      if(motor_running==false){
         move_to_position = q;
         command = CUSTOM_MOVE;
    }
    
    Serial.println(open_percent);
    }  

    request->redirect("/");
  
  });

server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){

DynamicJsonDocument doc(100);
doc["percent_position"] = int(((float)sendData(0x21, 0)/(float)max_steps)*100);
doc["steps_position"] = sendData(0x21, 0);
doc["max_steps"] = max_steps;
doc["current_high_open"] = open_current_high;
doc["current_high_close"] = close_current_high;
doc["current_low_open"] = open_current_low;
doc["current_low_close"] = close_current_low;
doc["stall_high_open"] = stall_open_high;
doc["stall_high_close"] = stall_close_high;
doc["stall_low_open"] = stall_open_low;
doc["stall_low_close"] = stall_close_low;

String json;
serializeJson(doc, json);

request->send(200, "application/json", json);
});

  server.begin();
}
