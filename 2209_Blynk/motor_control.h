void move_motor() {
  
Serial.print("Moving to Position: ");
Serial.println(move_to_position);
move_to_position = stepper.targetPosition();
XACTUAL = stepper.currentPosition();
motor_running = true;         // mark that the track motor is running
stepper.enableOutputs();
stepper.setAcceleration(MOVE_ACCEL);
stepper.setMaxSpeed(MOVE_VELOCITY);

 
//V3
if(XACTUAL < move_to_position){

  Serial.println("V3");
  move_direction=2;
  driver2.TCOOLTHRS(TCOOLS); // 
  driver2.SGTHRS(stall_open);
  driver2.rms_current(current_open); 
  stepper.moveTo(move_to_position); 

  bool buttonInterrupt=false;
  int buttonTime = millis()+3000;

  while (stepper.currentPosition() != stepper.targetPosition()) { // wait for position_reached flag
  stepper.run();
  }

}

//V4
else if(XACTUAL > move_to_position){

  Serial.println("V4");
  move_direction=1;
  driver2.TCOOLTHRS(TCOOLS); // 
  driver2.SGTHRS(stall_close);
  driver2.rms_current(current_close); 
  stepper.moveTo(move_to_position); 
 
  while (stepper.currentPosition() != stepper.targetPosition()) { // wait for position_reached flag
  stepper.run();
  }
  
}else{
  Serial.println("ALREADY THERE!");
}
  
XACTUAL = stepper.currentPosition();
preferences_local.putInt("XACTUAL", XACTUAL);
Serial.print("MOVED TO POSITION: ");
Serial.println(preferences_local.getInt("XACTUAL", 404));

stepper.disableOutputs();
motor_running = false;
blynk_update = true;
     
}
