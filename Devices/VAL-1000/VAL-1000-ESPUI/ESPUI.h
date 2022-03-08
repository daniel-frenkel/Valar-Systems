uint16_t button1;
uint16_t switchOne;
uint16_t status;

void selectOpenAmPmCall(Control* sender, int value) //Dropdown
{
    if(sender->value == "AM")
    {
     Serial.println(sender->value);
     open_am_pm = 0;
     preferences.putInt("open_am_pm", open_am_pm);
     open_am_pm_s = "AM";
     preferences.putString("open_am_pm_s", open_am_pm_s);
     
    }else
    {
     Serial.println(sender->value); 
     open_am_pm = 1;
     preferences.putInt("open_am_pm", open_am_pm);
     open_am_pm_s = "PM";
     preferences.putString("open_am_pm_s", open_am_pm_s);
    }
}

void selectCloseAmPmCall(Control* sender, int value) //Dropdown
{
    if(sender->value == "AM")
    {
     Serial.println(sender->value);
     close_am_pm = 0;
     close_am_pm_s = "AM";
     preferences.putInt("close_am_pm", close_am_pm);
     preferences.putString("close_am_pm_s", close_am_pm_s);
     
    }else
    {
     Serial.println(sender->value); 
     close_am_pm = 1;
     close_am_pm_s = "PM";
     preferences.putInt("close_am_pm", close_am_pm);
     preferences.putString("close_am_pm_s", close_am_pm_s);
    }
}

void numberMaxStepsCall(Control* sender, int type)
{
    max_steps = sender->value.toInt();
    preferences.putInt ("max_steps", max_steps);
    Serial.print("max_steps: ");
    Serial.println(max_steps);
}

void numberCurrentCall(Control* sender, int type)
{
    current = sender->value.toInt();
    preferences.putInt ("current", current);
    Serial.print("current: ");
    Serial.println(current);
}

void numberStallCall(Control* sender, int type)
{
    Serial.println(sender->value);
    stall = sender->value.toInt();
    driver.SGTHRS(stall);
    preferences.putInt ("stall", stall);
    Serial.print("stall: ");
    Serial.println(stall);
}

void numberOpenHourCall(Control* sender, int type)
{
    Serial.println(sender->value);
    open_hour = sender->value.toInt();
    preferences.putInt ("open_hour", open_hour);
}

void numberOpenMinuteCall(Control* sender, int type)
{
    Serial.println(sender->value);
    open_minute = sender->value.toInt();
    preferences.putInt ("open_minute", open_minute);    
}

void numberCloseHourCall(Control* sender, int type)
{
    Serial.println(sender->value);
    close_hour = sender->value.toInt();
    preferences.putInt ("close_hour", close_hour);
}

void numberCloseMinuteCall(Control* sender, int type)
{
    Serial.println(sender->value);
    close_minute = sender->value.toInt();
    preferences.putInt ("close_minute", close_minute);
}


void sliderPosition(Control* sender, int type)
{
    Serial.print("Slider: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);

    move_percent = sender->value;
    move_to_position = (max_steps/100)*move_percent;
    run_motor=true;
      
}

void buttonSetZeroCall(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Button Pressed");
        set_zero = 1;
        Serial.print("set_zero: ");
        Serial.println(set_zero);  
        break;
    }
}


void switchChangeDirectionCall(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("Active");
        open_direction = 1;
        preferences.putInt("open_dir", open_direction);
        driver.shaft(true); 
        break;

    case S_INACTIVE:
        Serial.print("Inactive");
        open_direction = 0;
        preferences.putInt("open_dir", open_direction);
        driver.shaft(false); 
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void switchOpenScheduleCall(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("Active");
        open_timer = 1;
        preferences.putInt("open_timer", open_timer);
        
        // determine open time
        newOpenTime = makeTime(open_hour, open_minute, 0, myTZ.day(), myTZ.month(), myTZ.year());
          
        // set next time to wakeup
        openEventNow = myTZ.setEvent(scheduleOpen, newOpenTime);
        break;

    case S_INACTIVE:
        Serial.print("Inactive");
        open_timer = 0;
        preferences.putInt("open_timer", open_timer);
        deleteEvent(openEvent); 
        deleteEvent(openEventNow);
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);
}

void switchCloseScheduleCall(Control* sender, int value)
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

void textNetworkCall(Control* sender, int type)
{
    ssid = sender->value;
    Serial.print(ssid);

}

void textPasswordCall(Control* sender, int type)
{
    pass = sender->value;
    Serial.print(pass);
}

void buttonSaveNetworkCall(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Button Pressed");
        preferences.putString ("ssid", ssid);
        preferences.putString ("pass", pass);
        wifi_set = 1;
        preferences.putInt ("wifi_set", 1);
        connectWifi();
        break;
    }
}

void buttonClearNetworkCall(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        preferences.putInt ("wifi_set", 0);
        preferences.putString ("ssid", "NOT_SET");
        preferences.putString ("pass", "NOT_SET");
        preferences.end();
        ESP.restart();
        break;
    }
}

void ESPUIsetup(){

    uint16_t tab1 = ESPUI.addControl(ControlType::Tab, "Positioning", "Positioning");
    uint16_t tab2 = ESPUI.addControl(ControlType::Tab, "Settings", "Settings");
    uint16_t tab3 = ESPUI.addControl(ControlType::Tab, "Schedule", "Schedule");
    uint16_t tab4 = ESPUI.addControl(ControlType::Tab, "WiFi", "WiFi");
    uint16_t tab5 = ESPUI.addControl(ControlType::Tab, "API", "API");

    // shown above all tabs. Not part of any tab
    //status = ESPUI.addControl(ControlType::Label, "Status:", "Stop", ControlColor::Turquoise);

//Tab1: Positioning
   //Text: Current position percent
   status = ESPUI.addControl(ControlType::Label, "Current Position:", String(move_percent), ControlColor::Turquoise, tab1);
   //Slider: Move to position
   uint16_t positionMax = ESPUI.addControl(ControlType::Slider, "Position", "0", ControlColor::Alizarin, tab1, &sliderPosition);
   ESPUI.addControl(ControlType::Min, "", "0", ControlColor::None, positionMax);
   ESPUI.addControl(ControlType::Max, "", "100", ControlColor::None, positionMax);

//Tab2: Settings
   ESPUI.addControl(ControlType::Separator, "Home Position", "", ControlColor::Peterriver, tab2);
   //Button: Set Zero
   ESPUI.addControl(ControlType::Switcher, "Change Direction", String(open_direction), ControlColor::Dark, tab2, &switchChangeDirectionCall);
   ESPUI.addControl(ControlType::Button, "Set Zero", "Set", ControlColor::Dark, tab2, &buttonSetZeroCall);
   
   ESPUI.addControl(ControlType::Separator, "Motor Setting", "", ControlColor::Peterriver, tab2);
   //Number: Max Steps
   ESPUI.addControl(ControlType::Number, "Max Steps:", String(max_steps), ControlColor::Peterriver, tab2, &numberMaxStepsCall);
   //Number: Current
   uint16_t currentMax = ESPUI.addControl(ControlType::Number, "Current(mA):", String(current), ControlColor::Alizarin, tab2, &numberCurrentCall);
   ESPUI.addControl(ControlType::Min, "", "400", ControlColor::None, currentMax);
   ESPUI.addControl(ControlType::Max, "", "2000", ControlColor::None, currentMax);
   //Number: Stall
   uint16_t stallMax = ESPUI.addControl(ControlType::Number, "Stall:", String(stall), ControlColor::Carrot, tab2, &numberStallCall);
   ESPUI.addControl(ControlType::Min, "", "0", ControlColor::None, stallMax);
   ESPUI.addControl(ControlType::Max, "", "255", ControlColor::None, stallMax);


//Tab3: Schedule
   //Timezone
   //Seperator
   ESPUI.addControl(ControlType::Separator, "Timezone", "", ControlColor::Dark, tab3);
   //Dropdown: Timezone

   //Open Schedule
   ESPUI.addControl(ControlType::Separator, "Open Scheduler", "", ControlColor::Dark, tab3);
   //Switch: Turn on open timer
   ESPUI.addControl(ControlType::Switcher, "Open Schedule", String(open_timer), ControlColor::Sunflower, tab3, &switchOpenScheduleCall);
   //Number: Hour
   uint16_t openHourMax = ESPUI.addControl(ControlType::Number, "Open Hour", String(open_hour), ControlColor::Sunflower, tab3, &numberOpenHourCall);
   ESPUI.addControl(ControlType::Min, "", "1", ControlColor::None, openHourMax);
   ESPUI.addControl(ControlType::Max, "", "12", ControlColor::None, openHourMax);
   //Number: minute
   uint16_t openMinuteMax = ESPUI.addControl(ControlType::Number, "Open Minute", String(open_minute), ControlColor::Sunflower, tab3, &numberOpenMinuteCall);
   ESPUI.addControl(ControlType::Min, "", "0", ControlColor::None, openMinuteMax);
   ESPUI.addControl(ControlType::Max, "", "59", ControlColor::None, openMinuteMax);
   //Drop Down: AM/PM
   uint16_t openTime = ESPUI.addControl(ControlType::Select, "AM/PM:", open_am_pm_s, ControlColor::Sunflower, tab3, &selectOpenAmPmCall);
   ESPUI.addControl(ControlType::Option, "AM", "AM", ControlColor::None, openTime);
   ESPUI.addControl(ControlType::Option, "PM", "PM", ControlColor::None, openTime);

   //Close Schedule
   ESPUI.addControl(ControlType::Separator, "Close Scheduler", "", ControlColor::Dark, tab3);
   //Switch: Turn off close
   ESPUI.addControl(ControlType::Switcher, "Close Schedule", String(open_timer), ControlColor::Dark, tab3, &switchOpenScheduleCall);
   //Number: Hour
   uint16_t closeHourMax = ESPUI.addControl(ControlType::Number, "Close Hour", String(close_hour), ControlColor::Dark, tab3, &numberCloseHourCall);
   ESPUI.addControl(ControlType::Min, "", "0", ControlColor::None, closeHourMax);
   ESPUI.addControl(ControlType::Max, "", "59", ControlColor::None, closeHourMax);
   //Number: minute
   uint16_t closeMinuteMax = ESPUI.addControl(ControlType::Number, "Close Minute", String(close_minute), ControlColor::Dark, tab3, &numberCloseMinuteCall);
   ESPUI.addControl(ControlType::Min, "", "0", ControlColor::None, closeMinuteMax);
   ESPUI.addControl(ControlType::Max, "", "59", ControlColor::None, closeMinuteMax);
   //Drop Down: AM/PM
   uint16_t closeTime = ESPUI.addControl(ControlType::Select, "AM/PM:", close_am_pm_s, ControlColor::Dark, tab3, &selectCloseAmPmCall);
   ESPUI.addControl(ControlType::Option, "AM", "AM", ControlColor::None, closeTime);
   ESPUI.addControl(ControlType::Option, "PM", "PM", ControlColor::None, closeTime);

//Tab4: WiFi 
    ESPUI.addControl(ControlType::Separator, "Wifi Status", "", ControlColor::None, tab4);
   //Label: Is wifi set?
    ESPUI.addControl(ControlType::Label, "Wifi Status", wifiStatus, ControlColor::Emerald, tab4);
     //Button: Clear Network Settings
   ESPUI.addControl(ControlType::Button, "Clear Settings", "CLEAR", ControlColor::Emerald, tab4, &buttonClearNetworkCall);
   //Text: Name

   String display_wifi;
   if(wifi_set)
   {
   display_wifi = "SET";  
   }else
   {
   display_wifi = "NOT SET";  
   }
   ESPUI.addControl(ControlType::Separator, "Set Wifi", display_wifi, ControlColor::None, tab4);
   ESPUI.addControl(ControlType::Text, "Network", ssid, ControlColor::Emerald, tab4, &textNetworkCall);
   
   //Text: Password
   ESPUI.addControl(ControlType::Text, "Password", "******", ControlColor::Emerald, tab4, &textPasswordCall);

   //Button: Save
   ESPUI.addControl(ControlType::Button, "Save Settings", "SAVE", ControlColor::Emerald, tab4, &buttonSaveNetworkCall);
   
//Tab5: API 
   char apiPosition[50];
   snprintf(apiPosition, sizeof(apiPosition), "http://%s:8080/position?move_percent=%i", ip_address.c_str(), move_percent);
   ESPUI.addControl(ControlType::Label, "Move to Position", apiPosition, ControlColor::Turquoise, tab5);
   
   char apisettings[50];
   snprintf(apisettings, sizeof(apisettings), "http://%s:8080/settings", ip_address.c_str());
   ESPUI.addControl(ControlType::Label, "Check Settings", apisettings, ControlColor::Turquoise, tab5);


    ESPUI.sliderContinuous = true;
    ESPUI.begin("Valar Systems");
    
}
