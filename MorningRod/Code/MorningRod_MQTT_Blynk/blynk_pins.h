int sun_delay = 0;
long check_timer = 0;
long daylight_timer = 0;
time_store times[4];
long last_timezone_offset=-1;

void save_time(int i);

int command = -1;


WidgetRTC rtc;
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}



WidgetLCD lcd(V3);

BLYNK_WRITE(V64) { // sunrise/sunset delay
  sun_delay = param.asInt();
  preferences.putInt("sun_delay", sun_delay);
}

BLYNK_WRITE(V12) { // track close now
  if(param.asInt()!=0){
    command = MOVE_CLOSE; // tell control loop what to do
  }
}

BLYNK_WRITE(V13) { // track open now
  if(param.asInt()!=0){
    command = MOVE_OPEN; // tell control loop what to do
  }
}

BLYNK_WRITE(V5) { // open1
  times[0].active=param.asInt()!=0;
  preferences.putUChar("active_0", times[0].active);
}

BLYNK_WRITE(V7) { // close1
  times[1].active=param.asInt()!=0;
  preferences.putUChar("active_1", times[1].active);
}

BLYNK_WRITE(V9) { // open2
  times[2].active=param.asInt()!=0;
  preferences.putUChar("active_2", times[2].active);
}

BLYNK_WRITE(V11) { // close2
  times[3].active=param.asInt()!=0;
  preferences.putUChar("active_3", times[3].active);
}

BLYNK_WRITE(V122) { // set global velocity
  DEBUG_STREAM.print("set all velocity: ");
  long q=param.asInt()*1000L;
  preferences.putLong("velocity", q);
  DEBUG_STREAM.println(q);
  sendData(0xA7, q);     // VMAX_M1
}

BLYNK_WRITE(V123) { // set stallguard value
  DEBUG_STREAM.print("set stall: ");
  int q=param.asInt()-64;
  DEBUG_STREAM.println(q);
  if(q>63)q=63;
  if(q<-64)q=-64;
  preferences.putInt("stallguard", q);
  q&=0x7F;
  q=q<<16;
  sendData(0x6D+0x80, COOLCONF_DEFAULT|q);     // STALLGUARD
}

BLYNK_WRITE(V22) { // set distance value
  DEBUG_STREAM.print("set distance: ");
  preferences.putFloat("move_distance", param.asInt()*32492.59347);
  //float q=param.asInt()*32492.59347; // One inch require this many microsteps
  //preferences.putFloat("track_distance", q);
  DEBUG_STREAM.println(MOVE_DISTANCE);
}

BLYNK_WRITE(V23) { // set percent open
  DEBUG_STREAM.print("set open percent to: ");
  preferences.putFloat("move_percent", (MOVE_DISTANCE/100)*param.asInt());
  //float q=(GET_TRACK_DISTANCE/100)*param.asInt();
  //preferences.putFloat("track_distance_percent", q);
  DEBUG_STREAM.println(MOVE_PERCENT);
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

BLYNK_WRITE(V35) { 
  if(param.asInt()!=0){
    Blynk.email("{DEVICE_NAME} : Token", configStore.cloudToken);
  }
}
