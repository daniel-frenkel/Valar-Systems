time_store times[4];
BlynkTimer blynkTimer;
WidgetRTC rtc;

BLYNK_CONNECTED() {
  rtc.begin();
  //Blynk.syncVirtual(V4, V5, V6, V7, V8, V9, V10, V11, V26, V25, V55, V56, V30, V36, V40, V41, V16, V17, V18, V19, V20, V33); //V124, V123, V35, V34, V33
  Blynk.virtualWrite(V27, BOARD_FIRMWARE_VERSION);
  blynk_update = true;
  blynkTimer.enable(schedulerId);
}


//DOOR ONLY FUNCTIONS
#ifdef Model_D

BLYNK_WRITE(V3) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Closing to position: ");
      move_to_position = 0;
      Serial.println(move_to_position);
      command = CUSTOM_MOVE;
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.print("Opening to position: ");
      move_to_position=(max_steps/100)*25;
      Serial.println(move_to_position);
      command = CUSTOM_MOVE;
      break;

    case 3: // Item 3 //Open 50%
      Serial.print("Opening to position: ");
      move_to_position=(max_steps/100)*50;
      Serial.println(move_to_position);
      command = CUSTOM_MOVE;
      break;

    case 4: // Item 4 //Open 100%
      Serial.print("Opening to position: ");
      move_to_position=max_steps-1000;
      Serial.println(move_to_position);
      command = CUSTOM_MOVE;
      break;

    case 5: // Item 5 //"STOP"
      Serial.print("STOPPING");
      command = STOP;
      break;
      
  }
}

BLYNK_WRITE(V52) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Turn on Training Buzzer: ");
      blynkTimer.disable(dogBuzzerId);
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.print("Turn off Dog Buzzer: ");
      blynkTimer.enable(dogBuzzerId);
      break;      
  }
}

BLYNK_WRITE(V14) { // Open then close door 
  if(param.asInt()!=0){
    command = OPEN_CLOSE; // tell control loop what to do
    Blynk.virtualWrite(V14, 0);
  }
}

BLYNK_WRITE(V40) { // Auto Open
  if(param.asInt()!=0){
  AUTO_OPEN_BLYNK = true; //
  AUTO_OPEN = true; //
  preferences_local.putInt("auto_open_perm", 1); // Used to compare the AUTO_OPEN variable to what is actually set by Blynk
  Serial.println("AUTO OPEN ON");
  }else{
     AUTO_OPEN_BLYNK = false;
     AUTO_OPEN = false;
     preferences_local.putInt("auto_open_perm", 0);
     Serial.println("AUTO OPEN OFF");
 }
}


BLYNK_WRITE(V41) { // set Auto Close Delay Time
  Serial.print("set Auto Close Delay Time to: ");
  AUTO_CLOSE_DELAY = param.asInt()*1000;
  preferences_local.putInt("aut_close_del", AUTO_CLOSE_DELAY);
  Serial.println(AUTO_CLOSE_DELAY);
}

BLYNK_WRITE(V38) { // set Auto OPEN Percent
  Serial.print("set OPEN-CLOSE position: ");
  OPEN_CLOSE_position = param.asInt()*one_inch;
  preferences_local.putFloat("open_close_pos", OPEN_CLOSE_position);
  Serial.println(OPEN_CLOSE_position);
}


BLYNK_WRITE(V25) { // set Current OPEN HIGH value
  Serial.print("set open high current: ");
  int q=param.asInt();
  open_current_calibration_value_high=q;
  if(open_current_calibration_value_high>26)open_current_calibration_value_high=26;
  preferences_local.putInt("2_cur_cal_val_h", q);
  Serial.println(q);
  double current = (255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V56, current);
  q&=0x1F;
  q=q<<8;
  open_current_high=q;
  preferences_local.putInt("open_current_h", q);
  Serial.println(open_current_high);
}

BLYNK_WRITE(V26) { // set Current CLOSE HIGH value
  Serial.print("set close high current: ");
  int q=param.asInt();
  close_current_calibration_value_high=q;
  if(close_current_calibration_value_high>26)close_current_calibration_value_high=26;
  preferences_local.putInt("1_cur_cal_val_h", q);
  Serial.println(q);
  double current = (255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V55, current);
  q&=0x1F;
  q=q<<8;
  close_current_high=q;
  preferences_local.putInt("close_current_h", q);
  Serial.println(close_current_high);
}


  BLYNK_WRITE(V29) { // set Current OPEN LOW value
  Serial.print("set open low current: ");
  int q=param.asInt();
  open_current_calibration_value_low=q;
  if(open_current_calibration_value_low>26)open_current_calibration_value_low=26;
  preferences_local.putInt("O_cur_cal_val_l", q);
  Serial.println(q);
  double current = (255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V58, current);
  q&=0x1F;
  q=q<<8;
  open_current_low=q;
  preferences_local.putInt("open_current_l", q);
  Serial.println(open_current_low);
}

BLYNK_WRITE(V28) { // set Current CLOSE LOW value
  Serial.print("set close low current: ");
  int q=param.asInt();
  close_current_calibration_value_low=q;
  if(close_current_calibration_value_low>26)close_current_calibration_value_low=26;
  preferences_local.putInt("C_cur_cal_val_l", q);
  Serial.println(q);
  double current = (255/(float)256)*((q+1)/(float)32)*(0.325/0.075)*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V57, current);
  q&=0x1F;
  q=q<<8;
  close_current_low=q;
  preferences_local.putInt("close_current_l", q);
  Serial.println(close_current_low);  
}


BLYNK_WRITE(V37) { 
  if(param.asInt()!=0){
    Blynk.email("{DEVICE_OWNER_EMAIL}","{DEVICE_NAME}: IFTTT SETUP", String('configStore.cloudToken\n') + String("LEARN MORE: http://help.valarsystems.com/Tulkas/IFTTT\n") + (String("OPEN URL: ") + String("http://morningrod.blynk.cc/") + (configStore.cloudToken) + String("/update/V3?value=4\n") + 
    String("CLOSE URL: ") + String("http://morningrod.blynk.cc/") + (configStore.cloudToken) + String("/update/V3?value=1\n") )); 
   }
}

BLYNK_WRITE(V42) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.println("Position 1 Close");
      CLOSE_POSITION = 1;
      preferences_local.putInt("close_pos", 1);
      sendData(0x00+0x80, 0x14);
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.println("Position 2 Close");
      CLOSE_POSITION = 2;
      preferences_local.putInt("close_pos", 2);
      sendData(0x00+0x80, 0x04);
      break;
      
  }
}

#endif // END MODEL_D



#ifdef Model_W


BLYNK_WRITE(V3) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      if(motor_running==false){
      Serial.println("Closing FULL");
      move_to_position = 0;
      Blynk.virtualWrite(V23, 0); 
      command = CUSTOM_MOVE;
      }
      break;
      
    case 2: // Item 2 //Open 2"
      if(motor_running==false){
      Serial.print("Moving to 2-inch position: ");
      move_to_position = 2 * one_inch; // One inch require this many microsteps // 17556480//WORKS 3102236.2204724409448818897637795 ==123545.6
      Serial.println(move_to_position);
      Blynk.virtualWrite(V23, 25); 
      command = CUSTOM_MOVE;
      }
      break;

    case 3: // Item 3 //Open 6"
      if(motor_running==false){
      Serial.print("Moving to 6-inch position: ");
      move_to_position = 6 * one_inch; // One inch require this many microsteps // 17556480//WORKS 3102236.2204724409448818897637795 ==123545.6
      Serial.println(move_to_position);
      Blynk.virtualWrite(V23, 50);
      command = CUSTOM_MOVE;
      }
      break;

    case 4: // Item 4 //Open 100%
      if(motor_running==false){
      Serial.print("Moving to 100% position: ");
      move_to_position = max_steps; // One inch require this many microsteps // 17556480//WORKS 3102236.2204724409448818897637795 ==123545.6
      Serial.println(move_to_position);
      Blynk.virtualWrite(V23, 100);
      command = CUSTOM_MOVE;
      }
      break;

    case 5: // Item 5 //"STOP"
      Serial.println("STOPPING");
      command = STOP;
      break;
      
  }
}

BLYNK_WRITE(V25) { // set Current OPEN HIGH value
  Serial.print("set open current: ");
  int q=param.asInt();
  open_current_calibration_value_high=q;
  if(open_current_calibration_value_high>26)open_current_calibration_value_high=26;
  preferences_local.putInt("2_cur_cal_val_h", q);
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V56, current);
  q&=0x1F;
  q=q<<8;
  open_current_high=q;
  preferences_local.putInt("open_current_h", q);
  Serial.println(open_current_high);
}

BLYNK_WRITE(V26) { // set Current CLOSE HIGH value
  Serial.print("set close current: ");
  int q=param.asInt();
  close_current_calibration_value_high=q;
  if(close_current_calibration_value_high>26)close_current_calibration_value_high=26;
  preferences_local.putInt("1_cur_cal_val_h", q);
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V55, current);
  q&=0x1F;
  q=q<<8;
  close_current_high=q;
  preferences_local.putInt("close_current_h", q);
  Serial.println(close_current_high);
}

  BLYNK_WRITE(V29) { // set Current OPEN LOW value
  Serial.print("set open low current: ");
  int q=param.asInt();
  open_current_calibration_value_low=q;
  if(open_current_calibration_value_low>26)open_current_calibration_value_low=26;
  preferences_local.putInt("O_cur_cal_val_l", q);
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V58, current);
  q&=0x1F;
  q=q<<8;
  open_current_low=q;
  preferences_local.putInt("open_current_l", q);
  Serial.println(open_current_low);
}

BLYNK_WRITE(V28) { // set Current CLOSE LOW value
  Serial.print("set close low current: ");
  int q=param.asInt();
  close_current_calibration_value_low=q;
  if(close_current_calibration_value_low>26)close_current_calibration_value_low=26;
  preferences_local.putInt("C_cur_cal_val_l", q);
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V57, current);
  q&=0x1F;
  q=q<<8;
  close_current_low=q;
  preferences_local.putInt("close_current_l", q);
  Serial.println(close_current_low);
}


BLYNK_WRITE(V37) { //CHANGE URL
  if(param.asInt()!=0){
    Blynk.email("{DEVICE_NAME}: IFTTT SETUP", String(configStore.cloudToken) + String('\n') + String("LEARN MORE: http://help.valarsystems.com/Tulkas/IFTTT\n") + (String("OPEN URL: ") + String("http://morningrod.blynk.cc/") + (configStore.cloudToken) + String("/update/V3?value=4\n") + 
    String("CLOSE URL: ") + String("http://morningrod.blynk.cc/") + (configStore.cloudToken) + String("/update/V3?value=1\n") ));
   }
}

BLYNK_WRITE(V43) {
    switch (param.asInt()){
   
    case 1: // SLOW SILENT
    Serial.println("SLOW/SILENT");
    fast_loud = false;
    MOVE_CLOSE_VELOCITY = 100000;
    MOVE_OPEN_VELOCITY = 100000;
    preferences_local.putBool("fast_loud", false);
    preferences_local.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
    preferences_local.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
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
    preferences_local.putBool("fast_loud", true);
    preferences_local.putInt("MOVE_CLOSE_VEL", MOVE_CLOSE_VELOCITY);
    preferences_local.putInt("MOVE_OPEN_VEL", MOVE_OPEN_VELOCITY);
    sendData(0x70 + 0x80 , 0);
      
       if(CLOSE_POSITION==2){
       sendData(0x00+0x80, 0x00);     // General settings /GCONF
       }else{
       sendData(0x00+0x80, 0x10);     // General settings /GCONF
       }
       
      break;

  }
}

BLYNK_WRITE(V42) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.println("Position 1 Close");
      CLOSE_POSITION = 1;
      preferences_local.putInt("close_pos", 1);
      sendData(0x00+0x80, 0x14);
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.println("Position 2 Close");
      CLOSE_POSITION = 2;
      preferences_local.putInt("close_pos", 2);
      sendData(0x00+0x80, 0x04);
      break;

  }
}
#endif

//END MODEL_W

BLYNK_WRITE(V15) { // MQTT
  if(param.asInt()!=0){
    Serial.println("MQTT Turned ON");
    MQTT_ON = true; // tell control loop what to do
    preferences_local.putBool("mqtt_on", MQTT_ON);
    Blynk.virtualWrite(V44, "Connected");

  }
  else{
    Serial.println("MQTT Turned OFF");
    MQTT_ON = false;
    MQTT_SETUP = false;
    preferences_local.putBool("mqtt_on", MQTT_ON);
    Blynk.virtualWrite(V44, "Not Connected");

  }
}

BLYNK_WRITE(V16) { //MQTT Device Name
  Serial.print("Set MQTT Device Name: ");
  mqtt_name = param.asStr();
  preferences_local.putString("mqtt_name", mqtt_name);
  
  mqtt_discovery_topic = disc_prefix + "/cover/" + mqtt_name + "/config";
  preferences_local.putString("disc_topic", mqtt_discovery_topic);

  mqtt_state_topic =  disc_prefix + "/cover/" + mqtt_name + "/state";
  preferences_local.putString("state_topic", mqtt_state_topic);

  mqtt_set_topic =  disc_prefix + "/cover/" + mqtt_name + "/set";
  preferences_local.putString("set_topic", mqtt_set_topic);

  
  Serial.println(mqtt_name);
  Serial.println(mqtt_discovery_topic);
  Serial.println(mqtt_state_topic);
  Serial.println(mqtt_set_topic);
}

BLYNK_WRITE(V17) { //IP Address
  Serial.print("Set IP Address: ");
  mqtt_server = param.asStr();
  preferences_local.putString("mqtt_server", mqtt_server);
  Serial.println(mqtt_server);
}

BLYNK_WRITE(V18) { //MQTT Username
  Serial.print("Set MQTT Username: ");
  mqtt_username = param.asStr();
  preferences_local.putString("mqtt_username", mqtt_username);
  Serial.println(mqtt_username);
}

BLYNK_WRITE(V19) { //MQTT Password
  Serial.print("Set MQTT Password: "); 
  mqtt_password = param.asStr();
  preferences_local.putString("mqtt_password", mqtt_password);
  Serial.println(mqtt_password);
}

BLYNK_WRITE(V20) { //Discovery Prefix
  Serial.print("Set MQTT Topic 1: ");
  disc_prefix = param.asStr();
  preferences_local.putString("disc_prefix", disc_prefix);
  
  mqtt_discovery_topic = disc_prefix + "/cover/" + mqtt_name + "/config";
  preferences_local.putString("disc_topic", mqtt_discovery_topic);

  mqtt_state_topic =  disc_prefix + "/cover/" + mqtt_name + "/state";
  preferences_local.putString("state_topic", mqtt_state_topic);

  mqtt_set_topic =  disc_prefix + "/cover/" + mqtt_name + "/set";
  preferences_local.putString("set_topic", mqtt_set_topic);

  Serial.println(disc_prefix);
}


BLYNK_WRITE(V5) { // open1
  times[0].active=param.asInt()!=0;
  preferences_local.putUChar("active_0", times[0].active);
  Serial.println(times[0].active);
}

BLYNK_WRITE(V7) { // close1
  times[1].active=param.asInt()!=0;
  preferences_local.putUChar("active_1", times[1].active);
  Serial.println(times[1].active);
}

BLYNK_WRITE(V9) { // open2
  times[2].active=param.asInt()!=0;
  preferences_local.putUChar("active_2", times[2].active);
  Serial.println(times[2].active);
}

BLYNK_WRITE(V11) { // close2
  times[3].active=param.asInt()!=0;
  preferences_local.putUChar("active_3", times[3].active);
  Serial.println(times[3].active);
}


BLYNK_WRITE(V33) { // Move to Open for setup
  if(param.asInt()!=0){
    command = POSITION_OPEN;
    }
}

BLYNK_WRITE(V34) { // Move to Close for setup
  if(param.asInt()!=0){
    command = POSITION_CLOSE;
    }
}

BLYNK_WRITE(V35) { // Move open for setup
  if(param.asInt()!=0){
    command = POSITION_ADJUST;
    }
}


BLYNK_WRITE(V123) { // set stallguard OPEN value
  Serial.print("set OPEN stall: ");
  int q=param.asInt();
  open_stall_calibration_value_low = q;
  preferences_local.putInt("2StallCalValLo", q);
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_open_low = q;
  preferences_local.putInt("stall_open_lo", stall_open_low); 
  Serial.println(stall_open_low);
}


BLYNK_WRITE(V124) { // set stallguard CLOSE value
  Serial.print("set CLOSE stall: ");
  int q=param.asInt();
  close_stall_calibration_value_low = q;
  preferences_local.putInt("1StallCalValLo", q);
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_close_low = q;
  preferences_local.putInt("stall_close_lo", stall_close_low); 
  Serial.println(stall_close_low);
}

BLYNK_WRITE(V121) { // set stallguard OPEN value
  Serial.print("set OPEN stall: ");
  int q=param.asInt();
  open_stall_calibration_value_high = q;
  preferences_local.putInt("2StallCalValHi", q);
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_open_high = q;
  preferences_local.putInt("stall_open_hi", stall_open_high); 
  Serial.println(stall_open_high);
}


BLYNK_WRITE(V122) { // set stallguard HIGH CLOSE value
  Serial.print("set CLOSE stall: ");
  int q=param.asInt();
  close_stall_calibration_value_high = q;
  preferences_local.putInt("1StallCalValHi", q);
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_close_high = q;
  preferences_local.putInt("stall_close_hi", stall_close_high); 
  Serial.println(stall_close_high);
}

BLYNK_WRITE(V22) { // set distance value
  Serial.print("set distance: ");
  float q=param.asFloat()*one_inch; 
  max_steps = (int)q;
  Serial.println(max_steps);
  preferences_local.putInt("max_steps", max_steps);
}


BLYNK_WRITE(V23) { // set position slider
  Serial.print("Move to: ");
  //preferences_local.putFloat("open_percent", (move_to_position/100)*param.asInt());
  //Need to know max position first
  int q=(max_steps/100)*param.asInt();
  
  if(motor_running==false){
    move_to_position = q;
    command = CUSTOM_MOVE;
    }
    
  Serial.println(open_percent);
}

BLYNK_WRITE(V24) { // set position slider
  Serial.print("Move to: ");
  //preferences_local.putFloat("open_percent", (move_to_position/100)*param.asInt());
  //Need to know max position first
  //int q=(max_steps/100)*param.asInt();
  timer_open_percent = param.asInt();
  preferences_local.putInt("timer_perc", timer_open_percent);
  Serial.println(timer_open_percent);
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
  }else
  {
    // Do nothing
  }
  
  for (int i = 1; i <= 7; i++) {
    times[0].day_sel[i-1]=t.isWeekdaySelected(i);
    Serial.println(times[0].day_sel[i-1]);
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


BLYNK_WRITE(V61){ // step
  stepValue = (param.asInt()*one_inch) / 8; // set the global step value 1/8th inch
  just_open_position = just_open_position + stepValue;
 
}

BLYNK_WRITE(V62){ // step
  if(param.asInt()!=0){
    just_open_set_button = true;
    }
}

BLYNK_WRITE(V80){ // 
  if(param.asInt()!=0){
  command = AUTO_TUNE;
  }
}

BLYNK_WRITE(V81){ // 
  if(param.asInt()!=0){
  command = STEP_1;
  }
}

BLYNK_WRITE(V82){ // 
  if(param.asInt()!=0){
  command = STEP_2;
  }
}

BLYNK_WRITE(V83){ // 
  if(param.asInt()!=0){
  command = STEP_3;
  }
}

BLYNK_WRITE(V84){ // 
  if(param.asInt()!=0){
  command = STEP_4;
  }
}

BLYNK_WRITE(V85){ // 
  if(param.asInt()!=0){
  //AUTO_OPEN = false;
  command = STEP_5;
  }
}


BLYNK_WRITE(V86){ // 
  if(param.asInt()!=0){
  //AUTO_OPEN = false;
  command = STEP_6;
  }
}


BLYNK_WRITE(V50) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Turn off stall Buzzer: ");
      stall_buzzer_blynk = false;
      preferences_local.putBool("stallBuzz", stall_buzzer_blynk);
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.print("Turn on stall Buzzer: ");
      stall_buzzer_blynk = true;
      preferences_local.putBool("stallBuzz", stall_buzzer_blynk);
      break;      
  }
}

BLYNK_WRITE(V53) { // Buzzer movement noises
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Turn off commandBuzzer Buzzer: ");
      commandBuzzer = false;
      preferences_local.putBool("commandBuzzer", commandBuzzer);
      break;
      
    case 2: // Item 2 //Open 25%
      Serial.print("Turn on commandBuzzer Buzzer: ");
      commandBuzzer = true;
      preferences_local.putBool("commandBuzzer", commandBuzzer);
      break;      
  }
}
