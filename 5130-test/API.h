#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "HTML.h"

const char *ap_ssid = "VALAR-AP";
const char *ap_password = "password";

String ip_address;

AsyncWebServer server(80);

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
  else if(var == "PLACEHOLDER_MAX_SPEED"){
    return String(max_speed/1000);
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
    if(request->hasParam("current"))
        {
          current = request->getParam("current")->value().toInt();
          //sendData(0x10+0x80, current);     // 7 = 0.52A current for open function
          driver.rms_current(current);
          preferences.putInt ("current", current);
          Serial.print("current: ");
          Serial.println(current);
        }
    if(request->hasParam("stall"))
        {
          stall = request->getParam("stall")->value().toInt();
          //sendData(0x6D+0x80, stall);     // STALLGUARD_OPEN
          driver.sgt(stall);
          preferences.putInt ("stall", stall);
          Serial.print("stall: ");
          Serial.println(stall);
        }
    if(request->hasParam("max_speed"))
        {
          int receive_max_speed = request->getParam("max_speed")->value().toInt();
          max_speed = receive_max_speed * 1000;
          //sendData(0x27+0x80, max_speed); //VMAX
          driver.VMAX(max_speed);
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

server.on("/position", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
  
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    if(request->hasParam("move_percent"))
    {
        move_percent = request->getParam("move_percent")->value().toInt();
        move_to = (max_steps/100)*move_percent;
        //sendData(0xAD, move_to); //XTARGET:
        driver.XTARGET(move_to);
        Serial.print("max_steps: ");
        Serial.println(max_steps); 
        Serial.print("move_percent: ");
        Serial.println(move_percent); 
        Serial.print("move_to: ");
        Serial.println(move_to); 
        run_motor=true;
    }  

    request->redirect("/");
  
  });

server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){

DynamicJsonDocument doc(100);
doc["percent_position"] = int(((float)sendData(0x21, 0)/(float)max_steps)*100);
doc["steps_position"] = sendData(0x21, 0);
doc["max_steps"] = max_steps;
doc["current"] = current;
doc["stall"] = stall;

String json;
serializeJson(doc, json);

request->send(200, "application/json", json);
});

  server.begin();
}
