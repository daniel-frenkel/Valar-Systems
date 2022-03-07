#include <DNSServer.h>
#include <ESPUI.h>

#include "ESPUI.h"

#include <functional>

#include <ESPAsyncWebServer.h>

#include "dataControlsJS.h"
#include "dataGraphJS.h"
#include "dataIndexHTML.h"
#include "dataNormalizeCSS.h"
#include "dataSliderJS.h"
#include "dataStyleCSS.h"
#include "dataTabbedcontentJS.h"
#include "dataZeptoJS.h"

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

AsyncWebServer serverAPI(8080);

    int move_percent;
    int move_to_position;
    int max_steps = 51200;
    bool run_motor;
    

#include <WiFi.h>

const char* ssid = "ESPUI";
const char* password = "espui";
const char* hostname = "espui";

uint16_t button1;
uint16_t openScheduler;
uint16_t closeScheduler;
uint16_t status;

void numberCall(Control* sender, int type)
{
    Serial.println(sender->value);
}

void textCall(Control* sender, int type)
{
    Serial.print("Text: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
}

void slider(Control* sender, int type)
{
    Serial.print("Slider: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
}

void buttonCallback(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Button DOWN");
        break;

    case B_UP:
        Serial.println("Button UP");
        break;
    }
}

void buttonExample(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Status: Start");
        ESPUI.updateControlValue(status, "Start");

        ESPUI.getControl(button1)->color = ControlColor::Carrot;
        ESPUI.updateControl(button1);
        break;

    case B_UP:
        Serial.println("Status: Stop");
        ESPUI.updateControlValue(status, "Stop");

        ESPUI.getControl(button1)->color = ControlColor::Peterriver;
        ESPUI.updateControl(button1);
        break;
    }
}

void padExample(Control* sender, int value)
{
    switch (value)
    {
    case P_LEFT_DOWN:
        Serial.print("left down");
        break;

    case P_LEFT_UP:
        Serial.print("left up");
        break;

    case P_RIGHT_DOWN:
        Serial.print("right down");
        break;

    case P_RIGHT_UP:
        Serial.print("right up");
        break;

    case P_FOR_DOWN:
        Serial.print("for down");
        break;

    case P_FOR_UP:
        Serial.print("for up");
        break;

    case P_BACK_DOWN:
        Serial.print("back down");
        break;

    case P_BACK_UP:
        Serial.print("back up");
        break;

    case P_CENTER_DOWN:
        Serial.print("center down");
        break;

    case P_CENTER_UP:
        Serial.print("center up");
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void switchExample(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("Active:");
        break;

    case S_INACTIVE:
        Serial.print("Inactive");
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void selectExample(Control* sender, int value)
{
    Serial.print("Select: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
}

void otherSwitchExample(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("Active:");
        break;

    case S_INACTIVE:
        Serial.print("Inactive");
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void setup(void)
{
    Serial.begin(115200);

    WiFi.setHostname(hostname);

    // try to connect to existing network
    WiFi.begin(ssid, password);
    Serial.print("\n\nTry to connect to existing network");

    {
        uint8_t timeout = 10;

        // Wait for connection, 5s timeout
        do
        {
            delay(500);
            Serial.print(".");
            timeout--;
        } while (timeout && WiFi.status() != WL_CONNECTED);

        // not connected -> create hotspot
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.print("\n\nCreating hotspot");

            WiFi.mode(WIFI_AP);
            delay(100);
            WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

            uint32_t chipid = 0;
            for (int i = 0; i < 17; i = i + 8)
            {
                chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
            }

            char ap_ssid[25];
            snprintf(ap_ssid, 26, "VALAR-%08X", chipid);
            WiFi.softAP(ap_ssid);

            timeout = 5;

            do
            {
                delay(500);
                Serial.print(".");
                timeout--;
            } while (timeout);
        }
    }

    dnsServer.start(DNS_PORT, "*", apIP);

    Serial.println("\n\nWiFi parameters:");
    Serial.print("Mode: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
    Serial.print("IP address: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());

    uint16_t tab1 = ESPUI.addControl(ControlType::Tab, "Position", "Position");
    uint16_t tab2 = ESPUI.addControl(ControlType::Tab, "Motor Settings", "Motor Settings");
    uint16_t tab3 = ESPUI.addControl(ControlType::Tab, "Schedule", "Schedule");
    uint16_t tab4 = ESPUI.addControl(ControlType::Tab, "WiFi", "WiFi");

    status = ESPUI.addControl(ControlType::Label, "Position:", "0", ControlColor::Turquoise, tab1);

    ESPUI.addControl(ControlType::Slider, "Position", "30", ControlColor::Alizarin, tab1, &slider);

    ESPUI.addControl(ControlType::Number, "Current:", "50", ControlColor::Alizarin, tab2, &numberCall);
    ESPUI.addControl(ControlType::Number, "Stall:", "50", ControlColor::Alizarin, tab2, &numberCall);

    ESPUI.addControl(ControlType::Number, "Max Steps:", "50", ControlColor::Alizarin, tab2, &numberCall);
    ESPUI.addControl(ControlType::Pad, "Move Motor", "", ControlColor::Carrot, tab2, &padExample);
    ESPUI.addControl(ControlType::Button, "Set Max", "Set Max", ControlColor::Wetasphalt, tab2, &buttonExample);

    
    openScheduler = ESPUI.addControl(ControlType::Switcher, "Open Scheduler", "", ControlColor::Alizarin, tab3, &switchExample);
    closeScheduler = ESPUI.addControl(ControlType::Switcher, "Close Scheduler", "", ControlColor::Alizarin, tab3, &switchExample);


    ESPUI.addControl(ControlType::Text, "Network Name:", "network", ControlColor::Alizarin, tab4, &textCall);
    ESPUI.addControl(ControlType::Text, "Password:", "password", ControlColor::Alizarin, tab4, &textCall);
    ESPUI.addControl(ControlType::Button, "Save", "Save", ControlColor::Wetasphalt, tab4, &buttonExample);


    

    /*
     * .begin loads and serves all files from PROGMEM directly.
     * If you want to serve the files from LITTLEFS use ESPUI.beginLITTLEFS
     * (.prepareFileSystem has to be run in an empty sketch before)
     */

    // Enable this option if you want sliders to be continuous (update during move) and not discrete (update on stop)
    // ESPUI.sliderContinuous = true;

    /*
     * Optionally you can use HTTP BasicAuth. Keep in mind that this is NOT a
     * SECURE way of limiting access.
     * Anyone who is able to sniff traffic will be able to intercept your password
     * since it is transmitted in cleartext. Just add a string as username and
     * password, for example begin("ESPUI Control", "username", "password")
     */


    
    serverAPI.on("/position", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
  
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
    }

    if(request->hasParam("move_percent"))
    {
        move_percent = request->getParam("move_percent")->value().toInt();
        printf("move_percent: %i\n", move_percent);
    }  
    request->redirect("/");  
    });

    
    serverAPI.begin();
    ESPUI.begin("ESPUI Control");
    
}

void loop(void)
{
    dnsServer.processNextRequest();

}
