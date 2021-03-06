void new_auto_calibrate_step_3(){ //CURRENT HIGH FRICTION
  
  Serial.print("STEP 3: Setting High-Friction Current: ");

  if(max_steps<(one_inch*6))return; //If max_steps isn't set then cancel
  
  digitalWrite(ENABLE_PIN, LOW);
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  sendData(0x14 + 0x80,  MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  
  Serial.print("Setting high open current: ");
  open_current_calibration_value_high=open_current_setup_value_high; //current value of 1
  open_current_calibration_high=open_current_calibration_value_high;
  Serial.println(open_current_calibration_high);
  open_current_calibration_high&=0x1F;
  open_current_calibration_high=open_current_calibration_high<<8;
  open_current_high=open_current_calibration_high;
  //sendData(0x10 + 0x80, open_current);
  Serial.println(open_current_high);

  Serial.print("setting high close current: ");
  close_current_calibration_value_high=close_current_setup_value_high; //current value of 1
  close_current_calibration_high=close_current_calibration_value_high;
  Serial.println(close_current_calibration_high);
  close_current_calibration_high&=0x1F;
  close_current_calibration_high=close_current_calibration_high<<8;
  close_current_high=close_current_calibration_high;
  //sendData(0x10 + 0x80, close_current);
  Serial.println(close_current_high);

  //Set stall open and close to 64

  Serial.print("Setting open stall: ");
  open_stall_calibration_value_high=open_stall_high_current_setup; //max setting
  open_stall_calibration_high=open_stall_calibration_value_high; //max setting
  Serial.println(open_stall_calibration_high);
  if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
  if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
  open_stall_calibration_high&=0x7F;
  open_stall_calibration_high=open_stall_calibration_high<<16;
  stall_open_high = open_stall_calibration_high;
  Serial.println(open_stall_calibration_high);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_high=close_stall_high_current_setup; //max setting
  close_stall_calibration_high=close_stall_calibration_value_high; //max setting
  Serial.println(close_stall_calibration_high);
  if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
  if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
  close_stall_calibration_high&=0x7F;
  close_stall_calibration_high=close_stall_calibration_high<<16;
  stall_close_high = close_stall_calibration_high;
  Serial.println(close_stall_calibration_high);

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
  
while (number_open_passes<current_high_open_passes || number_close_passes<current_low_open_passes) {

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


void new_auto_calibrate_step_5(){
  
  Serial.println("SETTING HIGH FRICTION ZONE STALLS");

  digitalWrite(ENABLE_PIN, LOW);
  
  Serial.println("STEP 2: Setting HIGH Friction Stall");
  
  //Set stall value to -63
  Serial.print("Setting open stall: ");
  open_stall_calibration_value_high=step_4_open_stall_high; //max setting
  open_stall_calibration_high=open_stall_calibration_value_high; //max setting
  Serial.println(open_stall_calibration_high);
  if(open_stall_calibration_high>63)open_stall_calibration_high=63; // Does not allow value more than 63
  if(open_stall_calibration_high<-64)open_stall_calibration_high=-64; // Does not allows value less and -64
  open_stall_calibration_high&=0x7F;
  open_stall_calibration_high=open_stall_calibration_high<<16;
  stall_open_high = open_stall_calibration_high;
  Serial.println(stall_open_high);

  Serial.print("Setting close stall: ");
  close_stall_calibration_value_high=step_4_close_stall_high; //max setting
  close_stall_calibration_high=close_stall_calibration_value_high; //max setting
  Serial.println(close_stall_calibration_high);
  if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
  if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
  close_stall_calibration_high&=0x7F;
  close_stall_calibration_high=close_stall_calibration_high<<16;
  stall_close_high = close_stall_calibration_high;
  Serial.println(stall_close_high);

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

while (number_open_passes<step_4_open_passes && number_close_passes<step_4_close_passes) {

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
      delay(200);

      sendData(0x35 + 0x80, 0x40); //Clear Stall
      sendData(0x34 + 0x80, 0x000); // Disable stallguard / TRY DELETE
      sendData(0x34 + 0x80, 0x400); // Enable stallguard / TRY DELETE

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


      //sendData(0x2D + 0x80, just_open_position); // XTARGET=0 to start motion

      sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
      sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
      sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
      sendData(0x10 + 0x80, close_current_high);
      sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
      //sendData(0x21 + 0x80, 0); //XACTUAL = 0
      calibration_direction = 1;
      }

      sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && calibration_direction == 1) { //IF STALLED CLOSING DUE TO LOW VALUE
        Serial.println("STALLED CLOSING");
        sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
        sendData(0x27+0x80, 0); //Set velocity to 0
        delay(200);
        
       // Serial.print("Increasing HIGH CLOSE stall: ");
        close_stall_calibration_value_high=close_stall_calibration_value_high+1; //max setting
        close_stall_calibration_high=close_stall_calibration_value_high; //max setting
        Serial.println("close_stall_calibration_value_high: ");
        Serial.println(close_stall_calibration_value_high);
        if(close_stall_calibration_high>63)close_stall_calibration_high=63; // Does not allow value more than 63
        if(close_stall_calibration_high<-64)close_stall_calibration_high=-64; // Does not allows value less and -64
        close_stall_calibration_high&=0x7F;
        close_stall_calibration_high=close_stall_calibration_high<<16;
        stall_close_high = close_stall_calibration_high;
        sendData(0x6D + 0x80, stall_close_high); // STALLGUARD_CLOSE
        
        sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
        
        sendData(0x35 + 0x80, 0x40); //Clear Stall
        sendData(0x34 + 0x80, 0x000); // Disable stallguard /
        sendData(0x34 + 0x80, 0x400); // Enable stallguard /

        sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
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
