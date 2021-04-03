void load_preferences(){

  mqtt_name = preferences_local.getString("mqtt_name", mqtt_name);
  mqtt_server = preferences_local.getString("mqtt_server", mqtt_server);
  mqtt_username = preferences_local.getString("mqtt_username", mqtt_username);
  mqtt_password = preferences_local.getString("mqtt_password", mqtt_password);
  mqtt_set_topic = preferences_local.getString("set_topic", "404");
  mqtt_state_topic = preferences_local.getString("state_topic", "404");
  mqtt_discovery_topic = preferences_local.getString("disc_topic", "404");
  disc_prefix = preferences_local.getString("disc_prefix","homeassistant");
  MQTT_ON = preferences_local.getBool("mqtt_on", false);
    
  stall_close = preferences_local.getLong("stall_close", 20);
  stall_open = preferences_local.getLong("stall_open", 20);
  current_open = preferences_local.getInt("current_open", 500);
  current_close = preferences_local.getInt("current_close", 500);
  open_stall_calibration = preferences_local.getInt("OStallCal", 4128768);
  open_stall_calibration_value = preferences_local.getInt("2StallCalVal", 63);
  close_stall_calibration = preferences_local.getInt("CStallCal", 4128768);
  close_stall_calibration_value = preferences_local.getInt("1StallCalVal", 63);
  open_current_calibration = preferences_local.getInt("O_cur_cal", 512);
  open_current_calibration_value = preferences_local.getInt("2_cur_cal_val", 4);
  close_current_calibration = preferences_local.getInt("C_cur_cal", 512);
  close_current_calibration_value = preferences_local.getInt("1_cur_cal_val", 4);
  move_distance = preferences_local.getFloat("move_distance", 300000);
  move_percent = preferences_local.getFloat("move_percent", 300000);
  MOVE_VELOCITY = preferences_local.getInt("move_velocity", 30000);
  CLOSE_POSITION = preferences_local.getInt("close_pos", 1);
  move_to_position = preferences_local.getInt("move_to_position", 0); //3102236
  max_steps = preferences_local.getInt("max_steps", 30000);
  XACTUAL = preferences_local.getInt("XACTUAL", 0);
  MOVE_ACCEL = preferences_local.getInt("move_accel", 30000);
  reverse_flag = preferences_local.getBool("reverse_flag", false);
  timer_open_percent = preferences_local.getInt("timer_perc", 100);
  just_open_position = preferences_local.getInt("just_open", 1 * one_inch);
}
