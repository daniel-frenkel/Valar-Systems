void move_motor() {
  
Serial.print("Moving to Position: ");
Serial.println(move_to_position);

move_to_position = stepper.targetPosition();
XACTUAL = stepper.currentPosition();

stepper.enableOutputs();
stepper.setAcceleration(MOVE_ACCEL);
stepper.setMaxSpeed(MOVE_VELOCITY);
driver2.TCOOLTHRS(TCOOLS);
 
//V3
if(move_to_position > XACTUAL){

  Serial.println("V3");
  
  driver2.SGTHRS(stall_open);
  driver2.rms_current(current_open); 

  while (stepper.currentPosition() != stepper.targetPosition()) { // wait for position_reached flag
  stepper.run();
  }

}

//V4
else if(move_to_position < XACTUAL){

  Serial.println("V4");

  driver2.SGTHRS(stall_close);
  driver2.rms_current(current_close); 
 
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
blynk_update = true;
     
}
