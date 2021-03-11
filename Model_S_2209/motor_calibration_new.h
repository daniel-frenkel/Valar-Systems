void new_auto_calibrate_step_1(){  //STEP 1 - Set Max distance 

  distance_cal=true;
  Serial.println("STEP 1: SETTING MAX STEPS!");
    
  stepper.enableOutputs();
  detachInterrupt(STALLGUARD);
   
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY/4);//quarter speed
  driver2.rms_current(2000); //current = 2000

  stepper.setCurrentPosition(0);
  stepper.moveTo(-one_inch*20);
            
  trip = 0;
            
  while(trip<5 && (stepper.currentPosition() != stepper.targetPosition())){
        
  stepper.run();
                  
  if (trip == 1) { //position 1 sensor tripped
  Serial.println("TRIP 1");

  stepper.setAcceleration(MOVE_ACCEL); 
  stepper.setMaxSpeed(MOVE_VELOCITY/4);//quarter speed
  stepper.setCurrentPosition(0);
  stepper.moveTo(one_inch*2);
      while(stepper.currentPosition() != stepper.targetPosition()){
      stepper.run();
      }
  stepper.setCurrentPosition(0);
  stepper.moveTo(-one_inch*20);
  trip = 2;  
  }

  if (trip == 3) { // Start measuring now
  Serial.println("TRIP 3");
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY/4);//quarter speed
  stepper.setCurrentPosition(0);
  stepper.moveTo(one_inch*40);  //move 40 inches away
  trip = 4;
  }
        
  if (trip==5) {
  Serial.println("TRIP 4");
  max_steps = stepper.currentPosition();
  }


}
         
      
  Serial.println("Finished Setting Max Steps");
  Serial.println(max_steps);     
  preferences_local.putInt("max_steps", max_steps);
  
  stepper.disableOutputs();
  attachInterrupt(STALLGUARD, stalled_position, RISING); 
   
  blynk_update = true;

  Serial.print("max_steps/one_inch: ");
  Serial.println(max_steps/one_inch);

  distance_cal=false;
  
}




void new_auto_calibrate_step_3(){ //AUTO CURRENT

  Serial.println("STEP 3: Setting High-Friction Current: ");

  current_set = false;
  
  stepper.enableOutputs();
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY);

  stepper.setCurrentPosition(0);
  stepper.moveTo(max_steps);
  
  detachInterrupt(STALLGUARD);
  attachInterrupt(STALLGUARD, current_calibration, RISING); 
  

//  Set stallguard fixed value
  driver2.SGTHRS(open_stall_current_setup); //stall 0...255 / =20
    
//  Set highest current value
  driver2.rms_current(open_current_setup_value); //current = 2000

  int number = 0; 
  
  while ((stepper.currentPosition() != stepper.targetPosition()) && current_set == false) { // wait for position_reached flag
  
  number = number + 1;
  
  stepper.run();

  if (number >= 10000){ //  Decrease current quickly. Once every 100000 cycles
  open_current_setup_value = open_current_setup_value-20;
  if(open_current_setup_value < 200)open_current_setup_value = 200;
  driver2.rms_current(open_current_setup_value);
  //Serial.println(open_current_setup_value-1);
  number = 0; 
  }
 
}
  
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delay(100);
  stepper.moveTo(stepper.currentPosition());
  stepper.disableOutputs();

  detachInterrupt(STALLGUARD);
  attachInterrupt(STALLGUARD, stalled_position, RISING); 
  Serial.println("STEP 3: CURRENT SET");
}
