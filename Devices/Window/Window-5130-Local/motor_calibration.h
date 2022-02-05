void new_auto_calibrate_step_1(){  //STEP 1 - Set Max distance 

  distance_cal=true;
  Serial.println("STEP 1: SETTING MAX STEPS!");
  digitalWrite(ENABLE_PIN, LOW);

  //Warn user to leave window unlocked
   
  sendData(0xB4, 0x000); // Disable stallguard /
  sendData(0x20+0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x10+0x80, setup_current); //
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
  //sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
            
  int trip = 0;
            
  while(trip<3){
        
  delay(1);
                  
  if (digitalRead(position_1_sensor)==LOW && trip == 0) {
  Serial.println("FIRST TRIP");
  sendData(0x26+0x80, MOVE_DECEL_CLOSED); //DMAX
  sendData(0x27+0x80, 0); //VMAX Set velocity to 0
  delay(500);
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //KEEP IT SLOW
  sendData(0x20+0x80, 1); // RAMPMODE=1 Move to POS 2 sensor
  sendData(0x21+0x80, 0); // set XACTUAL to zero. Change starts movement
  delay(2000);
  sendData(0x26+0x80, MOVE_DECEL_CLOSED); //DMAX
  sendData(0x27+0x80, 0); //VMAX 
  delay(200);
  sendData(0x20+0x80, 2); // RAMPMODE=1 Velocity mode 1 to 2 / RAMPMODE=2 Velocity mode 2 to 1
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
  trip = 1;
  }

  if (digitalRead(position_1_sensor)==LOW && trip == 1) {
  Serial.println("SECOND TRIP");
  sendData(0x26+0x80, MOVE_DECEL_CLOSED); //DMAX
  sendData(0x27+0x80, 0); //VMAX Set velocity to 0
  delay(500);
  
  sendData(0x20+0x80, 1); // RAMPMODE=1 Velocity mode 1 to 2 / RAMPMODE=2 Velocity mode 2 to 1
  
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //Set velocity SLOW
  trip = 2;
  }
        
  if (digitalRead(position_2_sensor)==LOW && trip==2) {
  Serial.println("THIRD TRIP");
  sendData(0x26+0x80, MOVE_DECEL_CLOSED); //AMAX
  sendData(0x27+0x80, 0); //Set velocity to 0
  delay(500);
  
  max_steps = sendData(0x21, 0);
  max_steps = sendData(0x21, 0);
  
  trip = 3;
  }


      #ifdef Model_W
      if(digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
      Serial.println("Button Stopped");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      break;  // end loop and stop motor
      }
      #endif
  }
         
      
  Serial.println("Finished Setting Max Steps");
  Serial.println(max_steps);     
  preferences_local.putInt("max_steps", max_steps);
  
  digitalWrite(ENABLE_PIN, HIGH);
   
  blynk_update = true;

  Serial.print("max_steps/one_inch: ");
  Serial.println(max_steps/one_inch);

  distance_cal=false;
  
}

void new_auto_calibrate_step_3(){ //CURRENT HIGH FRICTION
  
  Serial.print("STEP 3: Setting High-Friction Current: ");

  if(max_steps<(one_inch*6))return; //If max_steps isn't set then cancel
  
  digitalWrite(ENABLE_PIN, LOW);
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  sendData(0x14 + 0x80,  MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  
  Serial.print("Setting high open current: ");
  open_current_calibration_value_high=current_high_open_initial_value; //current value of 1
  open_current_calibration_high=open_current_calibration_value_high;
  Serial.println(open_current_calibration_high);
  open_current_calibration_high&=0x1F;
  open_current_calibration_high=open_current_calibration_high<<8;
  open_current_high=open_current_calibration_high;
  //sendData(0x10 + 0x80, open_current);
  Serial.println(open_current_high);

  Serial.print("setting high close current: ");
  close_current_calibration_value_high=current_high_close_initial_value; //current value of 1
  close_current_calibration_high=close_current_calibration_value_high;
  Serial.println(close_current_calibration_high);
  close_current_calibration_high&=0x1F;
  close_current_calibration_high=close_current_calibration_high<<8;
  close_current_high=close_current_calibration_high;
  //sendData(0x10 + 0x80, close_current);
  Serial.println(close_current_high);

  //Set stall open and close to 64

  Serial.print("Setting open stall: ");
  open_stall_calibration_value_high=stall_high_current_setup; //max setting
  open_stall_calibration_high=open_stall_calibration_value_high; //max setting
  Serial.println(open_stall_calibration_high);
  if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
  if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
  open_stall_calibration_high&=0x7F;
  open_stall_calibration_high=open_stall_calibration_high<<16;
  stall_open_high = open_stall_calibration_high;
  Serial.println(open_stall_calibration_high);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_high=stall_high_current_setup; //max setting
  close_stall_calibration_high=close_stall_calibration_value_high; //max setting
  Serial.println(close_stall_calibration_high);
  if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
  if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
  close_stall_calibration_high&=0x7F;
  close_stall_calibration_high=close_stall_calibration_high<<16;
  stall_close_high = close_stall_calibration_high;
  Serial.println(close_stall_calibration_high);

  blynk_update=true;
  
  sendData(0x20+0x80, 2); // RAMPMODE=1 Velocity mode to open / RAMPMODE=2 Velocity mode to Negative
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x10+0x80, close_current_high);
  sendData(0x6D+0x80, stall_close_high);
  
  Serial.println("Ramp Mode 1 Set");

  number_open_passes = 0;
  number_close_passes = 0;

  Serial.println("STARTING CURRENT CAL");
  
  //sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
  Serial.println("XACTUAL SET");
  calibration_direction = 1; //Moves away
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
  
while (number_open_passes<=current_high_open_passes || number_close_passes<=current_high_open_passes) {

      sendData(0x35, 0) & 0x200;
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2) { //IF JOP Position Reached
      Serial.println("JOP Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x10 + 0x80, close_current_high);
      sendData(0x6D + 0x80, stall_close_high);
      sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
      number_open_passes = number_open_passes+1;
      Serial.print("number_open_passes: ");
      Serial.println(number_open_passes);
      calibration_direction = 1;
     }

  if (!digitalRead(position_1_sensor) && calibration_direction==1) {
      Serial.println("Position 1 Tripped!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20 + 0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x10 + 0x80, open_current_high);
      sendData(0x6D + 0x80, stall_open_high);
      sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
      sendData(0x21 + 0x80, 0); //XACTUAL = 0
      sendData(0x2D + 0x80, just_open_position+(one_inch*1)); // XTARGET=0 to start motion
      number_close_passes = number_close_passes+1;
      Serial.print("number_close_passes: ");
      Serial.println(number_close_passes);
      calibration_direction = 2;
     }


     sendData(0x35, 0) & 0x40;
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2) { //IF STALLED OPENING DUE TO LOW CURRENT

      Serial.println("MOVE OPEN STALLED");
      
      //sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);

      sendData(0x35 + 0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE

      Serial.print("Increasing OPEN HIGH current to: ");
      open_current_calibration_value_high=open_current_calibration_value_high+1; //current value of 1
      if(open_current_calibration_value_high>26)open_current_calibration_value_high=26;
      open_current_calibration_high=open_current_calibration_value_high;
      Serial.println(open_current_calibration_high);
      open_current_calibration_high&=0x1F;
      open_current_calibration_high=open_current_calibration_high<<8;
      open_current_high=open_current_calibration_high;
      sendData(0x10 + 0x80, open_current_high);
      Serial.println(open_current_high);
      blynk_update=true;
      
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
      sendData(0x2D + 0x80, just_open_position); // XTARGET=0 to start motion

      }

      sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1) { //IF STALLED CLOSING DUE TO LOW VALUE
        
        Serial.println("STALLED CLOSING");
        sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
        sendData(0x27+0x80, 0); //Set velocity to 0
        delay(200);
     
        Serial.print("Increasing CLOSE HIGH current to: ");
        close_current_calibration_value_high=close_current_calibration_value_high+1; //current value of 1
        if(close_current_calibration_value_high>26)close_current_calibration_value_high=26;
        close_current_calibration_high=close_current_calibration_value_high;
        Serial.println(close_current_calibration_high);
        close_current_calibration_high&=0x1F;
        close_current_calibration_high=close_current_calibration_high<<8;
        close_current_high=close_current_calibration_high;
        sendData(0x10 + 0x80, close_current_high);
        Serial.println(close_current_high);
        blynk_update=true;
        
        //sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
        
        sendData(0x35 + 0x80, 0x40); //Clear Stall
        sendData(0x34 + 0x80, 0x000); // Disable stallguard /
        sendData(0x34 + 0x80, 0x400); // Enable stallguard /
        
        sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
        sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
        }

      #ifdef Model_W
      if(digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
      Serial.println("Button Stopped");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      break;  // end loop and stop motor
      }
      #endif
      
     delay(1);
  }

  Serial.println("CURRENT SET COMPLETE");
  Serial.print("Open Current: ");
  Serial.println(open_current_calibration_value_high);
  
  Serial.print("Close Current: ");
  Serial.println(close_current_calibration_value_high);

  preferences_local.putInt("O_cur_cal_h", open_current_calibration_high);
  preferences_local.putInt("2_cur_cal_val_h",open_current_calibration_value_high);

  preferences_local.putInt("C_cur_cal_h", close_current_calibration_high);
  preferences_local.putInt("1_cur_cal_val_h", close_current_calibration_value_high);
  
  preferences_local.putInt("close_current_h", close_current_high);
  preferences_local.putInt("open_current_h", open_current_high);

  sendData(0x26+0x80, 0); //AMAX
  sendData(0x27+0x80, 0); //VMAX
  
  digitalWrite(ENABLE_PIN, HIGH);
  
  blynk_update=true;


}

void new_auto_calibrate_step_4(){ //CURRENT LOW
  
  Serial.print("STEP 4: Setting Low-Friction Current: ");

  if(max_steps<(one_inch*10))return; //If max_steps isn't set then cancel
  
  digitalWrite(ENABLE_PIN, LOW);
  
  Serial.print("Setting low open current: ");
  open_current_calibration_value_low=current_low_open_initial_value; //current value of 1
  open_current_calibration_low=open_current_calibration_value_low;
  Serial.println(open_current_calibration_low);
  open_current_calibration_low&=0x1F;
  open_current_calibration_low=open_current_calibration_low<<8;
  open_current_low=open_current_calibration_low;
  //sendData(0x10 + 0x80, open_current);
  Serial.println(open_current_low);

  Serial.print("setting low close current: ");
  close_current_calibration_value_low=current_low_close_initial_value; //current value of 1
  close_current_calibration_low=close_current_calibration_value_low;
  Serial.println(close_current_calibration_low);
  close_current_calibration_low&=0x1F;
  close_current_calibration_low=close_current_calibration_low<<8;
  close_current_low=close_current_calibration_low;
  //sendData(0x10 + 0x80, close_current);
  Serial.println(close_current_low);

  //Set stall open and close to 64

  Serial.print("Setting open stall: ");
  open_stall_calibration_value_low=stall_low_current_setup; //max setting
  open_stall_calibration_low=open_stall_calibration_value_low; //max setting
  Serial.println(open_stall_calibration_low);
  if(open_stall_calibration_low>63)open_stall_calibration_low=63; // Does not allow value more than 63
  if(open_stall_calibration_low<-64)open_stall_calibration_low=-64; // Does not allows value less and -64
  open_stall_calibration_low&=0x7F;
  open_stall_calibration_low=open_stall_calibration_low<<16;
  stall_open_low = open_stall_calibration_low;
  Serial.println(open_stall_calibration_low);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_low=stall_low_current_setup; //max setting
  close_stall_calibration_low=close_stall_calibration_value_low; //max setting
  Serial.println(close_stall_calibration_low);
  if(close_stall_calibration_low>63)close_stall_calibration_low=63; // Does not allow value more than 63
  if(close_stall_calibration_low<-64)close_stall_calibration_low=-64; // Does not allows value less and -64
  close_stall_calibration_low&=0x7F;
  close_stall_calibration_low=close_stall_calibration_low<<16;
  stall_close_low = close_stall_calibration_low;
  Serial.println(close_stall_calibration_low);

  blynk_update=true;
   
   //Step 1 - Move to position 1 sensor

  Serial.print("Moving to Position 1");
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x20 + 0x80, 2); // RAMPMODE=2 Move to 1
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, close_current_high);
  sendData(0x6D + 0x80, stall_close_high); // Set to not trigger low in the low-zone
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x21 + 0x80, 0);      // XACTUAL to start motion
  
  while(true){
  Serial.println("Waiting to reach 1");
   if (!digitalRead(position_1_sensor)) {
      sendData(0x26+0x80, MOVE_DECEL); //AMAX
      sendData(0x27+0x80, 0); //VMAX
      delay(200);
      break;
      }
      
    delay(1);
    
   }
  
  Serial.println("Position 1 Reached");
  
  //Step 2 - Open to JOP using stall_open_high
  run_section = 1;
  calibration_direction = 2; //1 to close
  
  sendData(0x20 + 0x80, 0); // RAMPMODE=2 Move to 1
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, open_current_high);
  sendData(0x6D + 0x80, stall_open_high);
  Serial.println("Moving to just_open_position");
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  Serial.println(just_open_position);
  sendData(0x21 + 0x80, 0); // XACTUAL to start motion
  sendData(0x2D + 0x80, just_open_position); // XTARGET


   while(true){
    Serial.println("Waiting to reach JOP");
     if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2){ //IF Position Reached
        sendData(0x26+0x80, MOVE_DECEL); //AMAX
        sendData(0x27+0x80, 0); //VMAX
        delay(200);
        break;
        }  
      delay(1);
     }

  //Step 3 - Enable stallguard and Open to max_step-one_inch*2


  run_section = 2;
  calibration_direction = 2; //1 to close
  
  sendData(0x20 + 0x80, 0); // RAMPMODE=2 Move to 1
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, open_current_low);
  sendData(0x6D + 0x80, stall_open_low);
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  
  Serial.println("Moving to max_steps");
  //sendData(0x21 + 0x80, 0); // XACTUAL to start motion

  number_close_passes = 0;
  number_open_passes = 0;

  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  sendData(0x2D + 0x80, max_steps-one_inch/2); // XTARGET
  
while (number_open_passes<=current_low_open_passes || number_close_passes<=current_low_close_passes) {

    if (!digitalRead(position_1_sensor) &&  calibration_direction == 1 && run_section == 1) { //IF POS 1 reched
    Serial.println("Position 1 Tripped!");
    sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
    sendData(0x27+0x80, 0); //Set velocity to 0
    delay(200);
    //Reverse direction to JOP
    sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
    sendData(0x26+0x80, MOVE_ACCEL); //AMAX
    sendData(0x10+0x80, open_current_high);
    sendData(0x6D+0x80, stall_open_high);    //Disable SG?
    sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //VMAX
    sendData(0x21+0x80, 0); //XACTUAL
    sendData(0x2D+0x80, just_open_position); //XTARGET 
    Serial.print("Moving to JOP: "); 
    number_close_passes = number_close_passes + 1;
    Serial.println(number_close_passes);
    calibration_direction = 2;
    run_section = 1;
    }
  
      sendData(0x35, 0) & 0x200;
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2 && run_section == 1) { //IF JOP reached from POS 1
      //Stop motor then move to max_steps
      Serial.println("POS 1 to JOP Reached");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, open_current_low); 
      sendData(0x6D+0x80, stall_open_low);
      sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //VMAX
      Serial.println("Moving to MAX STEPS");
      sendData(0x2D + 0x80, max_steps-one_inch/2); // XTARGET
      //number_open_passes=number_open_passes+1;
      //Serial.println(number_open_passes);
      calibration_direction = 2;
      run_section = 2;
     }

   sendData(0x35, 0) & 0x200;
  
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2 && run_section == 2) { //IF max_steps reached from JOP
      //Stop motor then move to JOP
      Serial.println("open to max_steps Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      number_open_passes=number_open_passes+1;
      Serial.println(number_open_passes);
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, close_current_low);
      sendData(0x6D+0x80, stall_close_low);
      sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
      sendData(0x2D+0x80, just_open_position);      // XTARGET=0 to start motion
      calibration_direction = 1;
      run_section = 2;
     }
     
   sendData(0x35, 0) & 0x200;
   
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 1 && run_section == 2) { //IF JOP reached from max_steps

      Serial.println("Closed to just_open_position Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, close_current_high);
      sendData(0x6D+0x80, stall_close_high);
      sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
      //sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
      calibration_direction = 1;
      run_section = 1;
     }



     
  if (!digitalRead(position_2_sensor) &&  calibration_direction == 1 && run_section == 2) { //Try not to hit this
    Serial.println("Position 2 Tripped!");
    sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
    sendData(0x27+0x80, 0); //Set velocity to 0
    delay(200);
    //Reverse direction
    sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
    sendData(0x26+0x80, MOVE_ACCEL); //AMAX
    sendData(0x10+0x80, close_current_low);
    sendData(0x6D+0x80, stall_close_low);
    sendData(0x21+0x80, max_steps); //XACTUAL
    sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
    sendData(0x2D+0x80, just_open_position); //XTARGET 
    Serial.print("Moving to Position 1: "); 
    number_close_passes = number_close_passes + 1;
    Serial.println(number_close_passes);
    calibration_direction = 1;
    run_section = 2;
    }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2 && run_section == 2) { //IF STALLED OPENING DUE TO LOW VALUE
       
      Serial.println("MOVE OPEN STALLED");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      
      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
      
      //OPEN CURRENT LOW
      
      Serial.print("Increasing OPEN LOW current to: ");
      open_current_calibration_value_low=open_current_calibration_value_low+1; //current value of 1
      if(open_current_calibration_value_low>26)open_current_calibration_value_low=26;
      open_current_calibration_low=open_current_calibration_value_low;
      Serial.println(open_current_calibration_low);
      open_current_calibration_low&=0x1F;
      open_current_calibration_low=open_current_calibration_low<<8;
      open_current_low=open_current_calibration_low;
      sendData(0x10 + 0x80, open_current_low);
      Serial.println(open_current_low);
      blynk_update=true;
      //OPEN CURRENT LOW

      sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x20 + 0x80, 0); // RAMPMODE 2 to close
      sendData(0x2D + 0x80, max_steps-one_inch);      // XTARGET=0 to start motion
      sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
      Serial.print("XTARGET: ");
      Serial.println(max_steps-(one_inch*2));
      }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1 && run_section == 2) { //IF STALLED CLOSING DUE TO LOW VALUE
        
      Serial.println("MOVE CLOSE STALLED");
      sendData(0x26 + 0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27 + 0x80, 0); //Set velocity to 0
      delay(200);
      
      sendData(0x35 + 0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
      
      //CLOSE CURRENT LOW
        Serial.print("Increasing CLOSE LOW current to: ");
        close_current_calibration_value_low=close_current_calibration_value_low+1; //current value of 1
        if(close_current_calibration_value_low>26)close_current_calibration_value_low=26;
        close_current_calibration_low=close_current_calibration_value_low;
        Serial.println(close_current_calibration_low);
        close_current_calibration_low&=0x1F;
        close_current_calibration_low=close_current_calibration_low<<8;
        close_current_low=close_current_calibration_low;
        sendData(0x10 + 0x80, close_current_low);
        Serial.println(close_current_low);
        blynk_update=true;
      //CURRENT LOW

            
      sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x20 + 0x80, 0); // RAMPMODE 2 to close
      sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
      sendData(0x2D + 0x80, just_open_position);      // XTARGET=0 to start motion
      
      Serial.print("XTARGET: ");
      Serial.println(just_open_position);
      
      }


      sendData(0x35, 0) & 0x40;
      if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1 && run_section == 1) { //IF STALLED CLOSING HIGH FRICTION

      Serial.println("High Friction Close Stall");
      //Clear stall, restart movement
      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
  
      }

      sendData(0x35, 0) & 0x40;
      if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2 && run_section == 1) { //IF STALLED OPENING HIGH FRICTION

      //Clear stall, restart movement
      Serial.println("High Friction Open Stall");
      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
        
      }

      #ifdef Model_W
      if(digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
      Serial.println("Button Stopped");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      break;  // end loop and stop motor
      }
      #endif

     delay(1);
  }

  Serial.println("CURRENT SET COMPLETE");
  Serial.print("Open Current: ");
  Serial.println(open_current_calibration_value_low);
  
  Serial.print("Close Current: ");
  Serial.println(close_current_calibration_value_low);

  preferences_local.putInt("O_cur_cal_l", open_current_calibration_low);
  preferences_local.putInt("O_cur_cal_val_l",open_current_calibration_value_low);

  preferences_local.putInt("C_cur_cal_l", close_current_calibration_low);
  preferences_local.putInt("C_cur_cal_val_l", close_current_calibration_value_low);
  
  preferences_local.putInt("close_current_l", close_current_low);
  preferences_local.putInt("open_current_l", open_current_low);

  sendData(0x26+0x80, 0); //AMAX
  sendData(0x27+0x80, 0); //VMAX
  
  digitalWrite(ENABLE_PIN, HIGH);
  
  blynk_update=true;

}


void new_auto_calibrate_step_5(){
  
  Serial.println("SETTING HIGH FRICTION ZONE STALLS");

  digitalWrite(ENABLE_PIN, LOW);
  
  Serial.println("STEP 2: Setting HIGH Friction Stall");
  
  //Set stall value to -63
  Serial.print("Setting open stall: ");
  open_stall_calibration_value_high=stall_high_open_initial_value; //max setting
  open_stall_calibration_high=open_stall_calibration_value_high; //max setting
  Serial.println(open_stall_calibration_high);
  if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
  if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
  open_stall_calibration_high&=0x7F;
  open_stall_calibration_high=open_stall_calibration_high<<16;
  stall_open_high = open_stall_calibration_high;
  Serial.println(stall_open_high);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_high=stall_high_close_initial_value; //max setting
  close_stall_calibration_high=close_stall_calibration_value_high; //max setting
  Serial.println(close_stall_calibration_high);
  if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
  if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
  close_stall_calibration_high&=0x7F;
  close_stall_calibration_high=close_stall_calibration_high<<16;
  stall_close_high = close_stall_calibration_high;
  Serial.println(stall_close_high);
  
  blynk_update=true;

  sendData(0x14 + 0x80,  MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x20+0x80, 2); // Close door
  sendData(0x26+0x80, MOVE_ACCEL); //AMAX
  sendData(0x10+0x80, close_current_high);
  sendData(0x6D+0x80, stall_close_high);

  calibration_direction=1; //1 to close
  number_close_passes = 0;
  number_open_passes = 0;
  
  //Start opening
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  
  sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
  
  Serial.println("Starting While Loop");

while (number_open_passes<stall_high_open_passes && number_close_passes<stall_high_close_passes) {

   sendData(0x35, 0) & 0x200;
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2) { //IF JOP Position Reached
      Serial.println("JOP Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x10 + 0x80, close_current_high);
      sendData(0x6D + 0x80, stall_close_high);
      sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
      number_open_passes = number_open_passes+1;
      Serial.print("number_open_passes: ");
      Serial.println(number_open_passes);
      calibration_direction = 1;
     }

  if (!digitalRead(position_1_sensor) && calibration_direction==1) {
      Serial.println("Position 1 Tripped!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20 + 0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x10 + 0x80, open_current_high);
      sendData(0x6D + 0x80, stall_open_high);
      sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
      sendData(0x21 + 0x80, 0); //XACTUAL = 0
      sendData(0x2D + 0x80, just_open_position+(one_inch*2)); // XTARGET=0 to start motion
      number_close_passes = number_close_passes+1;
      Serial.print("number_close_passes: ");
      Serial.println(number_close_passes);
      calibration_direction = 2;
     }

     sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2) { //IF STALLED OPENING DUE TO LOW VALUE

      Serial.println("MOVE OPEN STALLED");
      
      //sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      //delay(200);

      open_stall_calibration_value_high=open_stall_calibration_value_high+1; //max setting
      open_stall_calibration_high=open_stall_calibration_value_high; //max setting
      Serial.print("open_stall_calibration_value_high: ");
      Serial.println(open_stall_calibration_value_high);
      if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
      if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
      open_stall_calibration_high&=0x7F;
      open_stall_calibration_high=open_stall_calibration_high<<16;
      stall_open_high = open_stall_calibration_high;
      
      sendData(0x6D + 0x80, stall_open_high);

      sendData(0x35 + 0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE

      sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //restart

      blynk_update=true;
    
      }

      sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1) { //IF STALLED CLOSING DUE TO LOW VALUE
        Serial.println("STALLED CLOSING");
        //sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
        sendData(0x27+0x80, 0); //Set velocity to 0
        delay(200);
        
       // Serial.print("Increasing HIGH CLOSE stall: ");
        close_stall_calibration_value_high=close_stall_calibration_value_high+1; //max setting
        close_stall_calibration_high=close_stall_calibration_value_high; //max setting
        if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
        if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
        close_stall_calibration_high&=0x7F;
        close_stall_calibration_high=close_stall_calibration_high<<16;
        stall_close_high = close_stall_calibration_high;
        
        sendData(0x6D + 0x80, stall_close_high); // STALLGUARD_CLOSE
        
        sendData(0x35 + 0x80, 0x40); //Clear Stall
        sendData(0x34 + 0x80, 0x000); // Disable stallguard /
        sendData(0x34 + 0x80, 0x400); // Enable stallguard /

        sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
        blynk_update=true;
        
        }

      #ifdef Model_W
      if(digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
      Serial.println("Button Stopped");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      break;  // end loop and stop motor
      }
      #endif
      
     delay(1);
  }

        //Increaseing by one just in case
        close_stall_calibration_value_high=close_stall_calibration_value_high+1; //max setting
        close_stall_calibration_high=close_stall_calibration_value_high; //max setting
        if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
        if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
        close_stall_calibration_high&=0x7F;
        close_stall_calibration_high=close_stall_calibration_high<<16;
        stall_close_high = close_stall_calibration_high;

        open_stall_calibration_value_high=open_stall_calibration_value_high+1; //max setting
        open_stall_calibration_high=open_stall_calibration_value_high; //max setting
        if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
        if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
        open_stall_calibration_high&=0x7F;
        open_stall_calibration_high=open_stall_calibration_high<<16;
        stall_open_high = open_stall_calibration_high;

        
  Serial.println("SETTING STALL VALUES");

  sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
  sendData(0x27+0x80, 0); //Set velocity to 0
  delay(1000);

  preferences_local.putInt("OStallCalHi", open_stall_calibration_high);
  preferences_local.putInt("2StallCalValHi", open_stall_calibration_value_high);

  preferences_local.putInt("CStallCalHi", close_stall_calibration_high);
  preferences_local.putInt("1StallCalValHi", close_stall_calibration_value_high);

  preferences_local.putInt("stall_close_hi", stall_close_high); 
  Serial.println(preferences_local.getInt("stall_close_hi", 404));
  
  preferences_local.putInt("stall_open_hi", stall_open_high);
  Serial.println(preferences_local.getInt("stall_open_hi", 404));
  
  Serial.println("STALL SETUP COMPLETE!");
  
  //Final Steps Overall
  digitalWrite(ENABLE_PIN, HIGH);
  blynk_update=true;
}



void new_auto_calibrate_step_6(){
  
  Serial.println("SETTING LOW FRICTION ZONE STALLS");

  digitalWrite(ENABLE_PIN, LOW);

  //Set stall value to -63
  Serial.print("Setting open stall: ");
  open_stall_calibration_value_low=stall_low_open_initial_value; //max setting
  open_stall_calibration_low=open_stall_calibration_value_low; //max setting
  Serial.println(open_stall_calibration_low);
  if(open_stall_calibration_low>63)open_stall_calibration_low=63; // Does not allow value more than 63
  if(open_stall_calibration_low<-64)open_stall_calibration_low=-64; // Does not allows value less and -64
  open_stall_calibration_low&=0x7F;
  open_stall_calibration_low=open_stall_calibration_low<<16;
  stall_open_low = open_stall_calibration_low;
  Serial.println(stall_open_low);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_low=stall_low_close_initial_value; //max setting
  close_stall_calibration_low=close_stall_calibration_value_low; //max setting
  Serial.println(close_stall_calibration_low);
  if(close_stall_calibration_low>63)close_stall_calibration_low=63; // Does not allow value more than 63
  if(close_stall_calibration_low<-64)close_stall_calibration_low=-64; // Does not allows value less and -64
  close_stall_calibration_low&=0x7F;
  close_stall_calibration_low=close_stall_calibration_low<<16;
  stall_close_low = close_stall_calibration_low;
  Serial.println(stall_close_low);

  blynk_update=true;
  
  //Step 1 - Move to position 1 sensor

  Serial.print("Moving to Position 1");
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x20 + 0x80, 2); // RAMPMODE=2 Move to 1
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, close_current_high);
  sendData(0x6D + 0x80, stall_close_high); // Set to not trigger low in the low-zone
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x21 + 0x80, 0);      // XACTUAL to start motion
  
  while(true){
  Serial.println("Waiting to reach 1");
   if (!digitalRead(position_1_sensor)) {
      sendData(0x26+0x80, MOVE_DECEL); //AMAX
      sendData(0x27+0x80, 0); //VMAX
      delay(200);
      break;
      }
      
    delay(1);
    
   }
  
  Serial.println("Position 1 Reached");
  
  //Step 2 - Open to JOP using stall_open_high
  run_section = 1;
  calibration_direction = 2; //1 to close
  
  sendData(0x20 + 0x80, 0); // RAMPMODE=2 Move to 1
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, open_current_high);
  sendData(0x6D + 0x80, stall_open_high);
  Serial.println("Moving to just_open_position");
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  Serial.println(just_open_position);
  sendData(0x21 + 0x80, 0); // XACTUAL to start motion
  sendData(0x2D + 0x80, just_open_position); // XTARGET
  

  while(true){
  Serial.println("Waiting to reach JOP");
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2){ //IF Position Reached
      sendData(0x26+0x80, MOVE_DECEL); //AMAX
      sendData(0x27+0x80, 0); //VMAX
      delay(200);
      break;
      }  
    delay(1);
   }

  //Step 3 - Enable stallguard and Open to max_step-one_inch*2

  run_section = 2;
  calibration_direction = 2; //1 to close
  
  sendData(0x20 + 0x80, 0); // RAMPMODE=2 Move to 1
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x10 + 0x80, open_current_low);
  sendData(0x6D + 0x80, stall_open_low);
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  
  Serial.println("Moving to max_steps");
  Serial.println(max_steps-one_inch*2);
  //sendData(0x21 + 0x80, 0); // XACTUAL to start motion

  number_close_passes = 0;
  number_open_passes = 0;

  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  sendData(0x2D + 0x80, max_steps-one_inch/2); // XTARGET
  
while (number_open_passes<stall_low_open_passes || number_close_passes<stall_low_close_passes) {

    sendData(0x35, 0) & 0x200;
  
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2 && run_section == 1) { //IF JOP reached from POS 1
      //Stop motor then move to max_steps
      Serial.println("POS 1 to JOP Reached");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, open_current_low); 
      sendData(0x6D+0x80, stall_open_low);
      sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //VMAX
      Serial.println("Moving to MAX STEPS");
      sendData(0x2D + 0x80, max_steps-one_inch/2); // XTARGET
      //number_open_passes=number_open_passes+1;
      //Serial.println(number_open_passes);
      calibration_direction = 2;
      run_section = 2;
     }

   sendData(0x35, 0) & 0x200;
  
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 2 && run_section == 2) { //IF max_steps reached from JOP
      //Stop motor then move to JOP
      Serial.println("open to max_steps Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      number_open_passes=number_open_passes+1;
      Serial.println(number_open_passes);
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, close_current_low);
      sendData(0x6D+0x80, stall_close_low);
      sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
      sendData(0x2D+0x80, just_open_position);      // XTARGET=0 to start motion
      calibration_direction = 1;
      run_section = 2;
     }
     
   sendData(0x35, 0) & 0x200;
   
   if (!(sendData(0x35, 0) & 0x200) == 0 && calibration_direction == 1 && run_section == 2) { //IF JOP reached from max_steps

      Serial.println("Closed to just_open_position Reached!");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      //Reverse direction
      sendData(0x20+0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26+0x80, MOVE_ACCEL); //AMAX
      sendData(0x10+0x80, close_current_high);
      sendData(0x6D+0x80, stall_close_high);
      sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
      //sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
      calibration_direction = 1;
      run_section = 1;
     }

  if (!digitalRead(position_1_sensor) &&  calibration_direction == 1 && run_section == 1) { //IF POS 1 reched
    Serial.println("Position 1 Tripped!");
    sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
    sendData(0x27+0x80, 0); //Set velocity to 0
    delay(200);
    //Reverse direction to JOP
    sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
    sendData(0x26+0x80, MOVE_ACCEL); //AMAX
    sendData(0x10+0x80, open_current_high);
    //Disable SG?
    sendData(0x6D+0x80, stall_open_high);
    sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //VMAX
    sendData(0x21+0x80, 0); //XACTUAL
    sendData(0x2D+0x80, just_open_position); //XTARGET 
    Serial.print("Moving to JOP: "); 
    number_close_passes = number_close_passes + 1;
    Serial.println(number_close_passes);
    calibration_direction = 2;
    run_section = 1;
    }

     
  if (!digitalRead(position_2_sensor) &&  calibration_direction == 1 && run_section == 2) { //Try not to hit this
    Serial.println("Position 2 Tripped!");
    sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
    sendData(0x27+0x80, 0); //Set velocity to 0
    delay(200);
    //Reverse direction
    sendData(0x20+0x80, 0); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
    sendData(0x26+0x80, MOVE_ACCEL); //AMAX
    sendData(0x10+0x80, close_current_low);
    sendData(0x6D+0x80, stall_close_low);
    sendData(0x21+0x80, max_steps); //XACTUAL
    sendData(0x27+0x80, MOVE_CLOSE_VELOCITY); //VMAX
    sendData(0x2D+0x80, just_open_position); //XTARGET 
    Serial.print("Moving to Position 1: "); 
    number_close_passes = number_close_passes + 1;
    Serial.println(number_close_passes);
    calibration_direction = 1;
    run_section = 2;
    }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2 && run_section == 2) { //IF STALLED OPENING DUE TO LOW VALUE
       
      Serial.println("MOVE OPEN STALLED");
      //sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      
      Serial.println("STALLED OPENING");
      Serial.print("Increasing open stall: ");
        
      open_stall_calibration_value_low=open_stall_calibration_value_low+1; //max setting
      open_stall_calibration_low=open_stall_calibration_value_low; //max setting
      if(open_stall_calibration_low>63)open_stall_calibration_low=63; // Does not allow value more than 63
      if(open_stall_calibration_low<-64)open_stall_calibration_low=-64; // Does not allows value less and -64
      open_stall_calibration_low&=0x7F;
      open_stall_calibration_low=open_stall_calibration_low<<16;
      stall_open_low = open_stall_calibration_low;
      
      sendData(0x6D+0x80, stall_open_low); // STALLGUARD_OPEN

      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE

      sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX

      blynk_update=true;

      }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1 && run_section == 2) { //IF STALLED CLOSING DUE TO LOW VALUE
        
      Serial.println("MOVE CLOSE STALLED");

      sendData(0x27 + 0x80, 0); //Set velocity to 0
      
      Serial.println("STALLED CLOSING");
      Serial.print("Increasing close stall: ");
        
      close_stall_calibration_value_low=close_stall_calibration_value_low+1; //max setting
      close_stall_calibration_low=close_stall_calibration_value_low; //max setting
      Serial.println(close_stall_calibration_value_low);
      if(close_stall_calibration_low>63)close_stall_calibration_low=63; // Does not allow value more than 63
      if(close_stall_calibration_low<-64)close_stall_calibration_low=-64; // Does not allows value less and -64
      close_stall_calibration_low&=0x7F;
      close_stall_calibration_low=close_stall_calibration_low<<16;
      stall_close_low = close_stall_calibration_low;

      sendData(0x6D+0x80, stall_close_low); // STALLGUARD_CLOSE

      sendData(0x35 + 0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
  
      sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
      
      Serial.print("XTARGET: ");
      Serial.println(just_open_position);

      blynk_update=true;
      
      }

      sendData(0x35, 0) & 0x40;
      
      if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1 && run_section == 1) { //IF STALLED CLOSING HIGH FRICTION

      Serial.println("High Friction Close Stall");
      //Clear stall, restart movement
      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
  
      }

      sendData(0x35, 0) & 0x40;
      
      if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 2 && run_section == 1) { //IF STALLED OPENING HIGH FRICTION

      //Clear stall, restart movement
      Serial.println("High Friction Open Stall");
      sendData(0x35+0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE
        
      }

      #ifdef Model_W
      if(digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
      Serial.println("Button Stopped");
      sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
      sendData(0x27+0x80, 0); //Set velocity to 0
      delay(200);
      break;  // end loop and stop motor
      }
      #endif

     delay(1);
    
}
      //Increase by one, just in case
      open_stall_calibration_value_low=open_stall_calibration_value_low+1; //max setting
      open_stall_calibration_low=open_stall_calibration_value_low; //max setting
      Serial.println(open_stall_calibration_value_low);
      if(open_stall_calibration_low>63)open_stall_calibration_low=63; // Does not allow value more than 63
      if(open_stall_calibration_low<-64)open_stall_calibration_low=-64; // Does not allows value less and -64
      open_stall_calibration_low&=0x7F;
      open_stall_calibration_low=open_stall_calibration_low<<16;
      stall_open_low = open_stall_calibration_low;

      close_stall_calibration_value_low=close_stall_calibration_value_low+1; //max setting
      close_stall_calibration_low=close_stall_calibration_value_low; //max setting
      Serial.println(close_stall_calibration_value_low);
      if(close_stall_calibration_low>63)close_stall_calibration_low=63; // Does not allow value more than 63
      if(close_stall_calibration_low<-64)close_stall_calibration_low=-64; // Does not allows value less and -64
      close_stall_calibration_low&=0x7F;
      close_stall_calibration_low=close_stall_calibration_low<<16;
      stall_close_low = close_stall_calibration_low;

      
  Serial.println("SETTING LOW STALL VALUES");

  sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
  sendData(0x27+0x80, 0); //Set velocity to 0
  delay(1000);

  preferences_local.putInt("OStallCalLo", open_stall_calibration_low);
  preferences_local.putInt("2StallCalValLo",open_stall_calibration_value_low);

  preferences_local.putInt("CStallCalLo", close_stall_calibration_low);
  preferences_local.putInt("1StallCalValLo", close_stall_calibration_value_low);

  preferences_local.putInt("stall_close_lo", stall_close_low); 
  Serial.println(preferences_local.getInt("stall_close_lo", 404));
  
  preferences_local.putInt("stall_open_lo", stall_open_low);
  Serial.println(preferences_local.getInt("stall_open_lo", 404));
  
  Serial.println("STALL SETUP COMPLETE!");

  digitalWrite(ENABLE_PIN, HIGH);
  
  blynk_update=true;
  
}


void position_adjust(){

     Serial.println("Position Adjust");
     digitalWrite(ENABLE_PIN, LOW);
     sendData(0xB4, 0x000); // Disable stallguard /
     sendData(0x10 + 0x80, setup_current);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A

     if(CLOSE_POSITION ==1){
      
     sendData(0x20 + 0x80, 1); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
     sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
     sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
     //sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
              
              int trip = 0;
              int max_time = millis() + 6000;
              while(trip<1){
        
                  delay(1);
                  
                  if ((digitalRead(position_2_sensor)==LOW) || millis()>max_time) {
                      sendData(0x26+0x80, MOVE_DECEL); //DMAX
                      sendData(0x27+0x80, 0); //VMAX Set velocity to 0
                      delay(500);
                      trip=1;
                  }
              }
              
     }else if(CLOSE_POSITION ==2) {

     Serial.println("Moving Away (Position 2)");
     sendData(0x20+0x80, 1); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
     sendData(0x26+0x80, MOVE_ACCEL); //AMAX
     sendData(0x27+0x80, MOVE_OPEN_VELOCITY); //VMAX
     sendData(0x21+0x80, 0);      // XACTUAL=0 to start motion
              
              int trip = 0;
              int max_time = millis() + 6000;
              while(trip<1){
        
                  delay(1);
                  
                  if ((digitalRead(position_1_sensor)==LOW) || millis()>max_time) {
                      sendData(0x26+0x80, MOVE_DECEL); //DMAX
                      sendData(0x27+0x80, 0); //VMAX Set velocity to 0
                      delay(500);
                      trip=1;
                  }
              }
       }  
   digitalWrite(ENABLE_PIN, HIGH);
}
