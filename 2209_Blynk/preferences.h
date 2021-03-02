void load_preferences(){

  stall_close = preferences_local.getLong("stall_close", 30);
  stall_open = preferences_local.getLong("stall_open", 30);
  current_open = preferences_local.getInt("current_open", 500);
  current_close = preferences_local.getInt("current_close", 500);

  MOVE_VELOCITY = preferences_local.getInt("move_velocity", 30000);
  CLOSE_POSITION = preferences_local.getInt("close_pos", 1);
  move_to_position = preferences_local.getInt("move_to_position", 0); //3102236
  max_steps = preferences_local.getInt("max_steps", 300000);
  XACTUAL = preferences_local.getInt("XACTUAL", 0);
  MOVE_ACCEL = preferences_local.getInt("move_accel", 30000);
  reverse_flag = preferences_local.getBool("reverse_flag", false);
  TCOOLS = preferences_local.getInt("tcools", 300);

  
}
