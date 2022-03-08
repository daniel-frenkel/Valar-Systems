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

     
    }else
    {
     Serial.println(sender->value); 

     
    }
}

void numberMaxStepsCall(Control* sender, int type)
{
    Serial.println(sender->value);
}

void numberCurrentCall(Control* sender, int type)
{
    Serial.println(sender->value);
}

void numberStallCall(Control* sender, int type)
{
    Serial.println(sender->value);
}

void numberOpenHourCall(Control* sender, int type)
{
    Serial.println(sender->value);
    open_hour = sender->value.toInt();
    Serial.println(open_hour);
    preferences.putInt ("open_hour", open_hour);
}

void numberOpenMinuteCall(Control* sender, int type)
{
    Serial.println(sender->value);
    open_minute = sender->value.toInt();
    Serial.println(open_minute);
    preferences.putInt ("open_minute", open_minute);    
}

void numberCloseHourCall(Control* sender, int type)
{
    Serial.println(sender->value);
    close_hour = sender->value.toInt();
    Serial.println(close_hour);
    preferences.putInt ("close_hour", close_hour);
}

void numberCloseMinuteCall(Control* sender, int type)
{
    Serial.println(sender->value);
    close_minute = sender->value.toInt();
    Serial.println(close_minute);
    preferences.putInt ("close_minute", close_minute);
}


void sliderPosition(Control* sender, int type)
{
    Serial.print("Slider: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
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

void buttonClearNetworkCall(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Button Pressed");
        break;
    }
}

void buttonSaveNetworkCall(Control* sender, int type)
{
    switch (type)
    {
    case B_DOWN:
        Serial.println("Button Pressed");
        break;
    }
}

void padPosition(Control* sender, int value)
{
    switch (value)
    {
    case P_LEFT_UP:
        Serial.print("left");
        break;

    case P_RIGHT_UP:
        Serial.print("right");
        break;
    }
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
    Serial.print("Text: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
}

void textPasswordCall(Control* sender, int type)
{
    Serial.print("Text: ID: ");
    Serial.print(sender->id);
    Serial.print(", Value: ");
    Serial.println(sender->value);
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
   ESPUI.addControl(ControlType::Slider, "Position", "0", ControlColor::Alizarin, tab1, &sliderPosition);
   //Pad: Move to position (update other values after press)
   ESPUI.addControl(ControlType::Pad, "Pad without center", "", ControlColor::Carrot, tab1, &padPosition);

//Tab2: Settings
   ESPUI.addControl(ControlType::Separator, "Home Position", "", ControlColor::Peterriver, tab2);
   //Button: Set Zero
   ESPUI.addControl(ControlType::Button, "Set Zero", "Set", ControlColor::Alizarin, tab2, &buttonSetZeroCall);
   
   ESPUI.addControl(ControlType::Separator, "Motor Setting", "", ControlColor::Peterriver, tab2);
   //Number: Max Steps
   ESPUI.addControl(ControlType::Number, "Max Steps:", "0", ControlColor::Peterriver, tab2, &numberMaxStepsCall);
   //Number: Current
   ESPUI.addControl(ControlType::Number, "Current(mA):", "400", ControlColor::Alizarin, tab2, &numberCurrentCall);
   //Number: Stall
   ESPUI.addControl(ControlType::Number, "Stall:", "0", ControlColor::Carrot, tab2, &numberStallCall);


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
   ESPUI.addControl(ControlType::Number, "Hour:", String(open_hour), ControlColor::Sunflower, tab3, &numberOpenHourCall);
   //Number: minute
   ESPUI.addControl(ControlType::Number, "Minute:", String(open_minute), ControlColor::Sunflower, tab3, &numberOpenMinuteCall);
   //Drop Down: AM/PM
   uint16_t openTime = ESPUI.addControl(ControlType::Select, "AM/PM:", "AM", ControlColor::Sunflower, tab3, &selectOpenAmPmCall);
   ESPUI.addControl(ControlType::Option, "AM", "AM", ControlColor::None, openTime);
   ESPUI.addControl(ControlType::Option, "PM", "PM", ControlColor::None, openTime);

   //Close Schedule
   ESPUI.addControl(ControlType::Separator, "Close Scheduler", "", ControlColor::Dark, tab3);
   //Switch: Turn off close
   ESPUI.addControl(ControlType::Switcher, "Close Schedule", String(open_timer), ControlColor::Dark, tab3, &switchOpenScheduleCall);
   //Number: Hour
   ESPUI.addControl(ControlType::Number, "Hour", String(close_hour), ControlColor::Dark, tab3, &numberCloseHourCall);
   //Number: minute
   ESPUI.addControl(ControlType::Number, "Minute", String(close_minute), ControlColor::Dark, tab3, &numberCloseMinuteCall);
   //Drop Down: AM/PM
   uint16_t closeTime = ESPUI.addControl(ControlType::Select, "AM/PM:", "AM", ControlColor::Dark, tab3, &selectCloseAmPmCall);
   ESPUI.addControl(ControlType::Option, "AM", "AM", ControlColor::None, closeTime);
   ESPUI.addControl(ControlType::Option, "PM", "PM", ControlColor::None, closeTime);

//Tab4: WiFi 
    ESPUI.addControl(ControlType::Separator, "Wifi Status", "", ControlColor::None, tab4);
   //Label: Is wifi set?
    ESPUI.addControl(ControlType::Label, "Wifi Status", wifiStatus, ControlColor::Emerald, tab4);
     //Button: Clear Network Settings
   ESPUI.addControl(ControlType::Button, "Clear Settings", "CLEAR", ControlColor::Emerald, tab4, &buttonClearNetworkCall);
   //Text: Name

   ESPUI.addControl(ControlType::Separator, "Set Wifi", "", ControlColor::None, tab4);
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
