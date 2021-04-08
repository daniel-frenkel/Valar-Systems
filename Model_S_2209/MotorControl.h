void move_motor() {
  
  Serial.print("Moving to Position: ");
  Serial.println(move_to_position);
  
  move_to_position = stepper.targetPosition();
  XACTUAL = stepper.currentPosition();
  
  motor_running = true;         // mark that the track motor is running
  move_open_stall = false;
  move_close_stall = false;
  
  
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY);
   
  int start_move_position=move_to_position;
  
  int maxOpenTime = millis() + 20000; //max amount of time to close in case of problem

  
//V3
if(XACTUAL == move_to_position)
{
  Serial.println("OMG, ALREADY THERE!");
}
else if(XACTUAL < move_to_position){

  Serial.println("V3");
  stepper.enableOutputs();
  move_direction=2;
  driver2.TCOOLTHRS(TCOOLS); // 
  driver2.SGTHRS(stall_open);
  driver2.rms_current(current_open); 
  stepper.moveTo(move_to_position); 

  bool buttonInterrupt=false;
  int buttonTime = millis()+3000;

  while (stepper.currentPosition() != stepper.targetPosition()) { // wait for position_reached flag
  
  stepper.run();

  //Attach interrupt after 1 second to avoid accidental press
  if(buttonInterrupt==false && buttonTime<millis()){
  Serial.println("BUTTONS ATTACHED");
  //attachInterrupt(btn1, button_stop, FALLING); //NEW
  //attachInterrupt(btn2, button_stop, FALLING); //NEW
  buttonInterrupt=true;
  }

}

}

//V4
else if(XACTUAL > move_to_position){

  Serial.println("V4");
  stepper.enableOutputs();
  move_direction=1;
  driver2.TCOOLTHRS(TCOOLS); // 
  driver2.SGTHRS(stall_close);
  driver2.rms_current(current_close); 
  stepper.moveTo(move_to_position); 
 
  while (stepper.currentPosition() != stepper.targetPosition()) { // wait for position_reached flag

  stepper.run();

//  if(digitalRead(btn1==LOW) || digitalRead(btn2==LOW)){
//  Serial.println("New Button Stop");
//  stepper.setAcceleration(200000);
//  stepper.setMaxSpeed(0);
//  delay(100);
//  stepper.moveTo(stepper.currentPosition());
//  }

}
  
}else{

  Serial.println("NOTHING!");
}
  
  String payload;
  
  if(move_open_stall==true){
    payload = "STALLED OPENED";
    }
    else{
    payload = "OPENED";
    }
    
  if(move_close_stall==true){
  payload = "STALLED CLOSING";
  }
  else{
  payload = "CLOSED";
  }

  if(client.connected()){
  client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
  }
  
  XACTUAL = stepper.currentPosition();
  
  preferences_local.putInt("XACTUAL", XACTUAL);

  Serial.print("MOVED TO POSITION: ");
  Serial.println(preferences_local.getInt("XACTUAL", 404));
  
  stepper.disableOutputs();
  motor_running = false;
  blynk_update=true;
  
  detachInterrupt(btn1);
  detachInterrupt(btn2);
    
  
}









void force_close(){

     Serial.println("force_close function");

     motor_running = true; 
     stepper.enableOutputs();
     stepper.setAcceleration(MOVE_ACCEL);
     stepper.setMaxSpeed(MOVE_CLOSE_VELOCITY);
     driver2.TCOOLTHRS(300); //  
     driver2.rms_current(current_close);          
     driver2.SGTHRS(stall_close);


     attachInterrupt(btn1, button_stop, FALLING); //NEW
     attachInterrupt(btn2, button_stop, FALLING); //NEW
     
     int maxOpenTime = millis() + 20000; //max amount of time to close in case of       
             
     while (millis()<maxOpenTime) { // wait for stall
    
     stepper.runSpeed();
     
     }
    
    
    stepper.disableOutputs();
    motor_running = false;
    
    detachInterrupt(btn1);
    detachInterrupt(btn2);
 
}


/*  ====================================
 * 
 *    BEGIN MOTOR DRIVER BACKEND
 * 
 *  ====================================
 */



// put your setup code here, to run once:
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
  
  if(CLOSE_POSITION==1){
    driver2.shaft(true);
  }else{
    driver2.shaft(false);
    }
}
