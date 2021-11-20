long check_timer = 0;
long daylight_timer = 0;
time_store times[4];
long last_timezone_offset=-1;

void save_time(int i);

int command = -1;

bool MQTT_ON = false;
bool MQTT_SETUP = false;
bool reverse_flag = false;

String mqtt_name = "MorningRod";
String mqtt_server = "192.168.50.178";

String mqtt_username = "username";
String mqtt_password = "password";

//HA Auto Discovery
String disc_prefix = "homeassistant";
String mqtt_discovery_topic = "homeassistant/cover/MorningRod/config";
String mqtt_state_topic =  "homeassistant/cover/MorningRod/state";
String mqtt_set_topic =  "homeassistant/cover/MorningRod/set";
String mqtt_discovery_payload;

BLYNK_WRITE(V15) { // MQTT
  if(param.asInt()!=0){
    Serial.println("MQTT Turned ON");
    MQTT_ON = true; // tell control loop what to do
    Blynk.virtualWrite(V24, "Connected");
  }
  else{
    Serial.println("MQTT Turned OFF");
    MQTT_ON = false;
    MQTT_SETUP = false;
    Blynk.virtualWrite(V24, "Not Connected");
  }
}

BLYNK_WRITE(V16) { //MQTT Device Name
  Serial.print("Set MQTT Device Name: ");
  mqtt_name = param.asStr();
  mqtt_discovery_topic = disc_prefix + "/cover/" + mqtt_name + "/config";
  mqtt_state_topic =  disc_prefix + "/cover/" + mqtt_name + "/state";
  mqtt_set_topic =  disc_prefix + "/cover/" + mqtt_name + "/set";
  preferences_local.putString("mqtt_name", mqtt_name);
  Serial.println(mqtt_name);
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
  mqtt_discovery_topic = disc_prefix + "/cover/" + mqtt_name + "/config";
  mqtt_state_topic = disc_prefix + "/cover/" + mqtt_name + "/state";
  mqtt_set_topic = disc_prefix + "/cover/" + mqtt_name + "/set";
  preferences_local.putString("disc_prefix", disc_prefix);
  Serial.println(disc_prefix);
}

WidgetRTC rtc;
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
  Blynk.syncVirtual(V4, V5, V6, V7, V8, V9, V10, V11, V23, V124, V123, V25, V26, V22, V35, V34, V36, V40, V41, V16, V17, V18, V19, V20);
  Blynk.virtualWrite(V27, BOARD_FIRMWARE_VERSION);
}


BLYNK_WRITE(V3) {
  switch (param.asInt())
  {
    case 1: // Item 1
      command = MOVE_CLOSE;
      break;
    case 2: // Item 2
       command = MOVE_OPEN;
      break;
    default:
      command = MOVE_CLOSE;
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

BLYNK_WRITE(V34) { // set global velocity
  Serial.print("set velocity: ");
  long q=param.asInt()*1000L;
  Serial.println(q);
  preferences_local.putLong("velocity", q);
  velocity = q;
}

BLYNK_WRITE(V123) { // set stallguard OPEN value
  Serial.print("set OPEN stall: ");
  int q=param.asInt();
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_open = q;
  preferences_local.putInt("stall_open", stall_open);
  Serial.println(stall_open);
  Serial.println(preferences_local.getInt("stall_open", stall_open));
}


BLYNK_WRITE(V124) { // set stallguard CLOSE value
  Serial.print("set CLOSE stall: ");
  int q=param.asInt();
  Serial.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  q&=0x7F;
  q=q<<16;
  stall_close = q;
  preferences_local.putInt("stall_close", stall_close);
  Serial.println(preferences_local.getInt("stall_close", stall_close));
}


BLYNK_WRITE(V25) { // set Current OPEN value
  Serial.print("set open current: ");
  int q=param.asInt();
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.12+0.02))*(1/sqrt(2));
  Serial.print("Open Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V56, current);
  q&=0x1F;
  q=q<<8;
  current_open=q;
  preferences_local.putInt("current_open", current_open);
  Serial.println(current_open);
}


BLYNK_WRITE(V26) { // set Current CLOSE value
  Serial.print("set close current: ");
  int q=param.asInt();
  Serial.println(q);
  double current =((q+1)/(float)32)*(0.325/(0.12+0.02))*(1/sqrt(2));
  Serial.print("Close Current: ");
  Serial.print(current);
  Serial.println(" Amps");
  Blynk.virtualWrite(V55, current);
  q&=0x1F;
  q=q<<8;
  current_close=q;
  preferences_local.putInt("current_close", current_close);
  Serial.println(current_close);
}

int gear_ratio = 1;
//float circumference_gear = 38.39;
float circumference_gear = 25.13;
float inches_mm = 25.4;
int steps_revolution = 51200;
float steps_inch = (inches_mm/circumference_gear)*steps_revolution;

BLYNK_WRITE(V22) { // set distance value
  Serial.print("set distance: ");
  float q=param.asInt()*steps_inch*gear_ratio;
  preferences_local.putFloat("move_distance", q);
  move_distance = q; //Opens from two sides
  move_percent = q;
  Serial.println(move_distance);
}

BLYNK_WRITE(V23) { // set percent open
  Serial.print("set move percent to: ");
//  preferences_local.putFloat("move_percent", (move_distance/100)*param.asInt());
  float q=(move_distance/100)*param.asInt();
  preferences_local.putFloat("move_percent", q);
  move_percent = q;
  Serial.println(move_percent);
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

BLYNK_WRITE(V30) { 
  if(param.asInt()!=0){
  Serial.println(ESP.getFreeHeap());  
  }
}

BLYNK_WRITE(V36) { // Reverse Motor Turn
  if(param.asInt()!=0){
    Serial.println("Reverse Turn ON ");
    reverse_flag = true; // tell control loop what to do
  }
  else{
    Serial.println("Reverse Turn OFF ");
    reverse_flag = false;
  }
  preferences_local.putBool("reverse_flag", reverse_flag);
}
