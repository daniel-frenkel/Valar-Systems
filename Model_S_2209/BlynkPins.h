time_store times[4];
BlynkTimer blynkTimer;
WidgetRTC rtc;

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


BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
  Blynk.virtualWrite(V27, BOARD_FIRMWARE_VERSION);
  blynk_update = true;

//   if (!client.connected()) {
//   reconnect();
//  }

  
}


BLYNK_WRITE(V3) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Closing to position: ");
      stepper.moveTo(0);
      Serial.println(stepper.targetPosition());
      command = CUSTOM_MOVE;
      break;

    case 2: // Item 4 //Open 100%
      Serial.print("Opening to position: ");
      stepper.moveTo(max_steps);
      Serial.println(stepper.targetPosition());
      command = CUSTOM_MOVE;
      break;

    case 3: // Item 5 //"STOP"
      Serial.println("STOPPING");
      command = STOP;

      stepper.setAcceleration(200000);
      stepper.setMaxSpeed(0);
      stepper.moveTo(stepper.currentPosition());

      
      break;
      
  }
}



BLYNK_WRITE(V5) { // open1
  times[0].active=param.asInt()!=0;
  preferences_local.putUChar("active_0", times[0].active);
}

BLYNK_WRITE(V7) { // close1
  times[1].active=param.asInt()!=0;
  preferences_local.putUChar("active_1", times[1].active);
}

BLYNK_WRITE(V9) { // open2
  times[2].active=param.asInt()!=0;
  preferences_local.putUChar("active_2", times[2].active);
}

BLYNK_WRITE(V11) { // close2
  times[3].active=param.asInt()!=0;
  preferences_local.putUChar("active_3", times[3].active);
}

int divider = 10;

BLYNK_WRITE(V21) { // set acceleration
  Serial.print("set accel: ");
  int q=param.asFloat()*divider;
  MOVE_ACCEL = q;
  Serial.println(MOVE_ACCEL);
  preferences_local.putInt("move_accel", MOVE_ACCEL);
  stepper.setAcceleration(MOVE_ACCEL);
}

BLYNK_WRITE(V34) { // set global velocity
  Serial.print("set velocity: ");
  long q=param.asInt()*divider;
  MOVE_VELOCITY = q;
  Serial.println(MOVE_VELOCITY);
  preferences_local.putInt("move_velocity", MOVE_VELOCITY);
  stepper.setMaxSpeed(MOVE_VELOCITY);

  
  TCOOLS = (3089838.00*pow(float(MOVE_VELOCITY),-1.00161534))*1.5; //Rsense = 0.12/ Recheck for 0.15

  Blynk.virtualWrite(V125, TCOOLS/10);
  
  Serial.print("TCOOLS: ");
  Serial.println(TCOOLS);
}


//STALLGUARD
/*
A higher value gives a higher sensitivity. 
A higher value makes StallGuard4 more sensitive and requires less torque to indicate a stall.
*/
BLYNK_WRITE(V123) { // set stallguard OPEN value
  Serial.print("set OPEN stall: ");
  int q = param.asInt();
  if(q > 255) q = 255;
  if(q < 0) q = 0;
  
  open_stall_calibration_value = q;
  preferences_local.putInt("2StallCalVal", q);
  Serial.println(q);

  stall_open = q;
  preferences_local.putInt("stall_open", stall_open); 
  Serial.println(stall_open);
}

BLYNK_WRITE(V124) { // set stallguard CLOSE value
  Serial.print("set CLOSE stall: ");
  int q=param.asInt();
  if(q>255)q=255;
  if(q<0)q=0;
  
  close_stall_calibration_value = q;
  preferences_local.putInt("1StallCalVal", q);
  Serial.println(q);

  stall_close = q;
  preferences_local.putInt("stall_close", stall_close); 
  Serial.println(stall_close);
}

//STALLGUARD



BLYNK_WRITE(V25) { // set Current OPEN value
  Serial.print("set open current: ");
  int q=param.asInt();
  open_current_calibration_value=q;
  if(open_current_calibration_value>2000)open_current_calibration_value=2000;
  preferences_local.putInt("2_cur_cal_val", q);
  Serial.println(q);
  Serial.print("Current: ");
  Serial.print(q);
  Serial.println(" Milli Amps");
  Blynk.virtualWrite(V56, q);
  current_open=q;
  preferences_local.putInt("current_open", q);
  driver2.rms_current(current_open);
  Serial.println(current_open);
}


BLYNK_WRITE(V26) { // set Current CLOSE value
  Serial.print("set close current: ");
  int q=param.asInt();
  close_current_calibration_value=q;
  if(close_current_calibration_value>2000)close_current_calibration_value=2000;
  preferences_local.putInt("1_cur_cal_val", q);
  Serial.println(q);
  Serial.print("Current: ");
  Serial.print(q);
  Serial.println("Milli  Amps");
  Blynk.virtualWrite(V55, q);
  current_close=q;
  driver2.rms_current(current_close);
  preferences_local.putInt("current_close", q);
  Serial.println(current_close);

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
  if(motor_running!=true){
    stepper.moveTo(q);
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

BLYNK_WRITE(V37) { 
  if(param.asInt()!=0){
    Blynk.email("{DEVICE_NAME} : Token", configStore.cloudToken);
  }
}

BLYNK_WRITE(V36) { // Reverse Motor Turn
  if(param.asInt()==1){
    Serial.println("Reverse Turn ON ");
    CLOSE_POSITION = 1; // tell control loop what to do
    driver2.shaft(true);
    preferences_local.putInt("close_pos", CLOSE_POSITION);
  }
  else{
    Serial.println("Reverse Turn OFF ");
    CLOSE_POSITION = 2;
    driver2.shaft(false);
    preferences_local.putInt("close_pos", CLOSE_POSITION);
  }
}

BLYNK_WRITE(V50) { //Set Zero
  if(param.asInt()!=0){
   setZero();
  }
}

BLYNK_WRITE(V51) { //Force Close
  if(param.asInt()!=0){
    command = FORCE_CLOSE;
  }
}

BLYNK_WRITE(V99) { //WiFi Shut-Off
  if(param.asInt()!=0){
    Blynk.disconnect();
   //WiFi.disconnect();
  }
}

BLYNK_WRITE(V80) { //Set Zero
  if(param.asInt()!=0){
   command = AUTO_CALIBRATE;
  }
}

BLYNK_WRITE(V81) { //Set Zero
  if(param.asInt()!=0){
   command = STEP_1;
  }
}


BLYNK_WRITE(V125) { // set TCOOLS
  Serial.print("set TCOOLS: ");
  int q=param.asFloat(); 
  TCOOLS = q*10;
  Serial.println(TCOOLS);
  driver2.TCOOLTHRS(TCOOLS); // 
  //preferences_local.putInt("tcools", TCOOLS);
}
