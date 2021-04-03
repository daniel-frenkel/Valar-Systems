void move_motor() {
  
  Serial.print("Moving to Position: ");
  Serial.println(move_to_position);
  
  move_to_position = stepper.targetPosition();
  XACTUAL = stepper.currentPosition();
  
  motor_running = true;         // mark that the track motor is running
  move_open_stall = false;
  move_close_stall = false;
  
  stepper.enableOutputs();
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY);
     
  int maxOpenTime = millis() + 120000; //max amount of time to close in case of problem


//V1 TWO Movements OPEN
if((just_open_position > XACTUAL) && (move_to_position > XACTUAL) && (move_to_position > just_open_position)){

  Serial.println("V1 Opening 2 Positions");
  move_direction=2;

  driver2.SGTHRS(stall_open_high);
  driver2.rms_current(open_current_high); 
  stepper.moveTo(just_open_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);
      
  int number_stop = 0;

  while (millis()<maxOpenTime) { // wait for position_reached flag

    stepper.run();
  
    if ((stepper.currentPosition() == stepper.targetPosition()) && number_stop == 0) { //JOP reached
      Serial.println("JOP REACHED");
      driver2.SGTHRS(stall_open_low);
      driver2.rms_current(open_current_low); 
      Serial.println("STALL LOW SET");
      stepper.moveTo(move_to_position); 
      number_stop = 1;     
      }
      
    if ((stepper.currentPosition() == stepper.targetPosition()) && number_stop == 1) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
    }
  
}else if((XACTUAL > just_open_position) && (XACTUAL > move_to_position) && (move_to_position < just_open_position)){

  Serial.println("V2 Closing 2 Positions");
  move_direction=1;
  run_section = 2;
  driver2.SGTHRS(stall_close_low);
  driver2.rms_current(close_current_low); 
  stepper.moveTo(just_open_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);
      
  int number_stop = 0;

  while (millis()<maxOpenTime) { // wait for position_reached flag

  stepper.run();
  
    if ((stepper.currentPosition() == stepper.targetPosition()) && number_stop == 0) { //JOP reached
      Serial.println("JOP REACHED");
      driver2.SGTHRS(stall_close_high);
      driver2.rms_current(close_current_high); 
      Serial.println("STALL HIGH SET");
      stepper.moveTo(move_to_position); 
      number_stop = 1;
      run_section = 1;      
      }
      
    if ((stepper.currentPosition() == stepper.targetPosition()) && number_stop == 1) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
  }
}else if((XACTUAL > just_open_position) && (XACTUAL < move_to_position) && (just_open_position < move_to_position)){

  Serial.println("V3");
  move_direction=2;

  driver2.SGTHRS(stall_open_low);
  driver2.rms_current(open_current_low); 
  stepper.moveTo(move_to_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);


  while (millis()<maxOpenTime) { // wait for position_reached flag

  stepper.run();
  
   if (stepper.currentPosition() == stepper.targetPosition()) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }

  }
}else if((XACTUAL > just_open_position) && (XACTUAL > move_to_position) && (just_open_position < move_to_position)){

  Serial.println("V4");
  move_direction=1;

  driver2.SGTHRS(stall_close_low);
  driver2.rms_current(close_current_low); 
  stepper.moveTo(move_to_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);
  

  while (millis()<maxOpenTime) { // wait for position_reached flag

  stepper.run();
  
  if (stepper.currentPosition() == stepper.targetPosition()) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
  
  }
}else if((XACTUAL < just_open_position) && (XACTUAL > move_to_position) && (just_open_position > move_to_position)){

  Serial.println("V5");
  move_direction=1;

  driver2.SGTHRS(stall_close_high);
  driver2.rms_current(close_current_high); 
  stepper.moveTo(move_to_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);

  
  while (millis()<maxOpenTime) { // wait for position_reached flag

  stepper.run();
  
  if (stepper.currentPosition() == stepper.targetPosition()) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
  
  }
}else if((XACTUAL < just_open_position) && (XACTUAL < move_to_position) && (just_open_position > move_to_position)){ //V6

  Serial.println("V6");
  move_direction=2;

  driver2.SGTHRS(stall_open_high);
  driver2.rms_current(open_current_high); 
  stepper.moveTo(move_to_position); 
  stepper.setMaxSpeed(MOVE_VELOCITY);
  

  while (millis()<maxOpenTime) { // wait for position_reached flag


  stepper.run();
  
  if (stepper.currentPosition() == stepper.targetPosition()) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
}
}else { //LAST RESORT COMMAND TO CLOSE //V7 else if(XACTUAL > move_to_position) {
Serial.println("ALREADY THERE!");
}


  XACTUAL = stepper.currentPosition();
  
  preferences_local.putInt("XACTUAL", XACTUAL);

  Serial.print("MOVED TO POSITION: ");
  Serial.println(preferences_local.getInt("XACTUAL", 404));

  String payload;
  
  if(move_open_stall==true){
  payload = "STALLED OPENED";
  }else if(move_close_stall==true){
  payload = "STALLED CLOSING";
  }else{
  payload = int((XACTUAL/max_steps)*100);
  }

  if(client.connected()){
  client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
  }

  stepper.disableOutputs();
  motor_running = false;
  blynk_update=true;
  
}

void position_close(){

     Serial.println("force_close function");

     motor_running = true; 
     stepper.enableOutputs();
     stepper.setAcceleration(MOVE_ACCEL);
     stepper.setSpeed(MOVE_VELOCITY);
     driver2.rms_current(close_current_high);          
     driver2.SGTHRS(stall_close_high);

     stepper.moveTo(-one_inch*40);
     
     attachInterrupt(btn1, button_stop, FALLING); //NEW
     attachInterrupt(btn2, button_stop, FALLING); //NEW
     
     int maxOpenTime = millis() + 20000; //max amount of time to close in case of       
             
     while (stepper.currentPosition() != stepper.targetPosition()) { 
    
     stepper.run();
     
     }
    
    stepper.disableOutputs();
    motor_running = false;
    
    detachInterrupt(btn1);
    detachInterrupt(btn2);
 
}


void position_open(){

    Serial.println("force_close function");

     motor_running = true; 
     stepper.enableOutputs();
     stepper.setAcceleration(MOVE_ACCEL);
     stepper.setSpeed(MOVE_VELOCITY);
     driver2.rms_current(open_current_high);          
     driver2.SGTHRS(stall_open_high);

     stepper.moveTo(one_inch*40);
     
     attachInterrupt(btn1, button_stop, FALLING); //NEW
     attachInterrupt(btn2, button_stop, FALLING); //NEW
     
     int maxOpenTime = millis() + 20000; //max amount of time to close in case of       
             
     while (stepper.currentPosition() != stepper.targetPosition()) { 
    
     stepper.run();
     
     }
    
    stepper.disableOutputs();
    motor_running = false;
    
    detachInterrupt(btn1);
    detachInterrupt(btn2);
}


void position_adjust(){

    Serial.println("force_close function");

     motor_running = true; 
     stepper.enableOutputs();
     stepper.setAcceleration(MOVE_ACCEL);
     stepper.setSpeed(MOVE_VELOCITY);
     driver2.rms_current(open_current_high);          
     driver2.SGTHRS(stall_open_high);

     stepper.moveTo(one_inch*2);
     
     attachInterrupt(btn1, button_stop, FALLING); //NEW
     attachInterrupt(btn2, button_stop, FALLING); //NEW
     
     int maxOpenTime = millis() + 20000; //max amount of time to close in case of       
             
     while (stepper.currentPosition() != stepper.targetPosition()) { 
    
     stepper.run();
     
     }
    
    stepper.disableOutputs();
    motor_running = false;
    
    detachInterrupt(btn1);
    detachInterrupt(btn2);
}


/*  ====================================

      BEGIN MOTOR DRIVER BACKEND

    ====================================
*/


void setup_motors(){ 
   
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STALLGUARD ,INPUT);
  SERIAL_PORT_2.begin(115200);

  driver2.begin();
  driver2.toff(4);
  driver2.blank_time(24);
  driver2.rms_current(300); 
  driver2.microsteps(motor_microsteps);
  driver2.TCOOLTHRS(300); // 
  driver2.TPWMTHRS(0);
  driver2.semin(0);
  driver2.semax(2);
  driver2.sedn(0b00);
  
  driver2.en_spreadCycle(false);
  driver2.pdn_disable(true);

  stepper.setEnablePin(ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.disableOutputs();
  
  attachInterrupt(STALLGUARD, stalled_position, RISING);
  attachInterrupt(SENSOR1, sensor_long, FALLING);
  attachInterrupt(SENSOR2, sensor_short, FALLING);
  
  if(CLOSE_POSITION==1){
    driver2.shaft(true);
  }else{
    driver2.shaft(false);
    }
}
