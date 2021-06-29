#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

const char *ap_ssid = "VALAR-AP";
const char *ap_password = "password";

AsyncWebServer server(80);

const char WIFI_HTML[] = "Enter your home Wifi Name and Password <br> <br> <form action=\"/set_wifi\">\n    <label class=\"label\">Network Name</label>\n    <input type = \"text\" name = \"ssid\"/>\n    <br/>\n    <label>Network Password</label>\n    <input type = \"text\" name = \"pass\"/>\n    <br/>\n    <input type=\"submit\" value=\"Set Values\">\n</form>";
const char SETTINGS_HTML[] = "<h1>Enter your motor parameters</h1>\n<br> \n<br>\n<form action=\"/position\">\n    <p>move_to Instructions</p>\n    <label><b>move_to :</b></label>\n    <input value = \"%PLACEHOLDER_PERCENT%\" type = \"text\" name = \"move_percent\"/>\n    <br/>\n    <input type=\"submit\" value=\"Set Position\">\n</form>\n<br>\n<br>\n<form action=\"/set_motor\">\n    <p>max_steps Instructions</p>\n    <label><b>max_steps :</b></label>\n    <input value = \"%PLACEHOLDER_MAX_STEPS%\" type = \"text\" name = \"max_steps\"/>\n    <br/>\n    <p>current Instructions</p>\n    <label><b>current</b></label>\n    <input value = \"%PLACEHOLDER_CURRENT%\" type = \"text\" name = \"current\"/>\n    <br/>\n    <p>stall Instructions</p>\n    <label><b>stall</b></label>\n    <input value = \"%PLACEHOLDER_STALL%\" type = \"text\" name = \"stall\"/>\n    <br/>\n    <p>accel Instructions</p>\n    <label><b>accel</b></label>\n    <input value = \"%PLACEHOLDER_ACCEL%\" type = \"text\" name = \"accel\"/>\n    <br/>\n    <p>max_speed Instructions</p>\n    <label><b>max_speed</b></label>\n    <input value = \"%PLACEHOLDER_MAX_SPEED%\" type = \"text\" name = \"max_speed\"/>\n    <br/>\n    <input type=\"submit\" value=\"Set Parameters\">\n</form>\n<br>\n<br>\n<p>Press this button to set the home position of your motor to zero</p>\n<form action=\"/set_zero\">\n<input type=\"hidden\" name=\"set_zero\" value=\"1\" />\n<input type=\"submit\" value=\"Set Zero\">\n</form>\n";

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
      
      request->send(200, "text/html", "WiFi Credentials Set. Connect to your home WiFi network, find the IP address of this device, and go to http://NEW-IP-ADDRESS/settings");
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

server.on("/position", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
  
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    if(request->hasParam("move_percent"))
    {
        move_percent = request->getParam("move_percent")->value().toInt();
        move_to = (max_steps/100)*move_percent;
        Serial.print("move_percent: ");
        Serial.println(move_percent); 
        run_motor=true;

    }  

    request->redirect("/");
  
  });

  server.begin();
}
