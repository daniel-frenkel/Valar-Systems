void move_motor_position_1() {

  motor_running = true;         // mark that the track motor is running
  restart_movement = false;
  move_open_stall = false;
  move_close_stall = false;
  
  Serial.println("Moving Pos 1 Motor Function");
  buzzer_flag=false;
  move_close_stall=false;
  move_open_stall=false;

  if(commandBuzzer==true){
  for (int i=1; i <= 2; i++){
    ledcWriteTone(moveChannel, moveFreq);
    unsigned long time_now;
    time_now = millis();
    while(millis() < time_now + move_time_on){
    ledcWrite(moveChannel, 255);
    }
    ledcWrite(moveChannel, 0);
    delay(100);
    }
  } 
  
  digitalWrite(ENABLE_PIN, LOW);      // enable the TMC5072
  
  //Get Xactual Position
  delay(50);
  XACTUAL = sendData(0x21, 0);
  Serial.print("XACTUAL: ");
  Serial.println(XACTUAL);
  delay(50);
  XACTUAL = sendData(0x21, 0);
  Serial.println(XACTUAL);
  delay(50);  
  Serial.println(XACTUAL);
  XACTUAL = sendData(0x21, 0);
  
  Serial.print("move_to_position: ");
  Serial.println(move_to_position);

  sendData(0xA0, 0);     // Positioning mode 0
  sendData(0x26 + 0x80, MOVE_ACCEL);//AMAX
  sendData(0x28 + 0x80, MOVE_DECEL);//DMAX
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
    
  int start_move_position=move_to_position;

  int maxOpenTime = millis() + 120000; //max amount of time to close in case of problem


if(move_to_position == XACTUAL){
  Serial.println("move_to_position == XACTUAL");
}


//V1 TWO Movements OPEN
else if((just_open_position > XACTUAL) && (move_to_position > XACTUAL) && (move_to_position > just_open_position)){

  Serial.println("V1 Opening 2 Positions");
  move_direction=2;
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_open_high); // 
  sendData(0x10 + 0x80, open_current_high);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  sendData(0x2D + 0x80, just_open_position); //XTARGET: Starts Movement 
  
  int number_stop = 0;

  while (millis()<maxOpenTime) { // wait for position_reached flag

    sendData(0x35, 0) & 0x200;
  
    if (!(sendData(0x35, 0) & 0x200) == 0 & number_stop == 0) { //JOP reached
      Serial.println("JOP REACHED");
      sendData(0x6D + 0x80, stall_open_low); //
      sendData(0x10 + 0x80, open_current_low);   
      Serial.println("STALL LOW SET");
      sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement
      number_stop = 1;     
      }
      
    sendData(0x35, 0) & 0x200;
  
    if (!(sendData(0x35, 0) & 0x200) == 0 & number_stop == 1) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }

    sendData(0x35, 0) & 0x40;
    
    if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction==2) { //If Stall while moving 1 to 2
      Serial.println("MOVE OPEN STALLED");
      //buzzer_flag = true;
      move_open_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_2_sensor) && move_direction==2) {
      Serial.println("TRIPPED AT POSITION 2");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else if((XACTUAL > just_open_position) && (XACTUAL > move_to_position) && (move_to_position < just_open_position)){

  Serial.println("V2 Closing 2 Positions");
  move_direction=1;
  run_section = 2;
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_close_low); // 
  sendData(0x10 + 0x80, close_current_low);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, just_open_position); //XTARGET: Starts Movement 
  
  int number_stop = 0;

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0 && number_stop == 0) { //JOP reached
      Serial.println("JOP REACHED");
      sendData(0x6D + 0x80, stall_close_high); //
      sendData(0x10 + 0x80, close_current_high);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
      Serial.println("STALL HIGH SET");
      //sendData(0x2D+0x80, move_to_position); //XTARGET: Starts Movement
      if(move_to_position==0){ //If 0, then full close until sensor
        sendData(0x20 + 0x80, 2); // RAMPMODE close
        }else{
        number_stop = 1;
        }
   run_section = 1;     
   }
      
  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0 && number_stop == 1) { //move_to_position reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
      
  sendData(0x35, 0) & 0x40; 
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction == 1) { //If Stall while moving 1 to 2
      Serial.println("MOVE CLOSE STALLED");
      buzzer_flag = true;
      move_close_stall = true;       
      sendData(0x27+0x80, 0); //VMAX
      restart_movement = true;
      break;
      
      /*
      if(run_section==2){
          sendData(0x2D+0x80, move_to_position); //XTARGET: Starts Movement
          sendData(0x34 + 0x80, 0x000); // Disable stallguard /
          sendData(0x34 + 0x80, 0x400); // Enable stallguard /
          }else{
          sendData(0x27+0x80, 0); //VMAX
          break;
          }
          */
    }

  if (!digitalRead(position_1_sensor) && move_direction == 1) {
      Serial.println("TRIPPED AT POSITION 1");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else if((XACTUAL > just_open_position) && (XACTUAL < move_to_position) && (just_open_position < move_to_position)){

  Serial.println("V3");
  move_direction=2;
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_open_low); // 
  sendData(0x10 + 0x80, open_current_low);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 


  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }

  if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction==2) { //If Stall while moving 1 to 2
      Serial.println("MOVE OPEN STALLED");
      //buzzer_flag = true;
      move_open_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_2_sensor) && move_direction==2) {
      Serial.println("TRIPPED AT POSITION 2");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else if((XACTUAL > just_open_position) && (XACTUAL > move_to_position) && (just_open_position < move_to_position)){

  Serial.println("V4");
  move_direction=1;
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_close_low); // 
  sendData(0x10 + 0x80, close_current_low);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction==1) { //If Stall while moving 1 to 2
      Serial.println("MOVE CLOSE STALLED");
      //buzzer_flag = true;
      move_close_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_1_sensor) && move_direction==1) {
      Serial.println("TRIPPED AT POSITION 1");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else if((XACTUAL < just_open_position) && (XACTUAL > move_to_position) && (just_open_position > move_to_position)){

  Serial.println("V5");
  move_direction=1;
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_close_high); // 
  sendData(0x10 + 0x80, close_current_high);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction==1) { //If Stall while moving 1 to 2
      Serial.println("MOVE OPEN STALLED");
      //buzzer_flag = true;
      move_close_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_2_sensor) && move_direction==1) {
      Serial.println("TRIPPED AT POSITION 2");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else if((XACTUAL < just_open_position) && (XACTUAL < move_to_position) && (just_open_position > move_to_position)){ //V6

  Serial.println("V6");
  move_direction=2;
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_open_high); // 
  sendData(0x10 + 0x80, open_current_high);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;  
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //JOP reached
      Serial.println("move_to_position REACHED");
      Serial.println("BREAKING");
      break;    
      }
  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0 && move_direction==2) { //If Stall while moving 1 to 2
      Serial.println("MOVE OPEN STALLED");
      //buzzer_flag = true;
      move_open_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_2_sensor) && move_direction==2) {
      Serial.println("TRIPPED AT POSITION 2");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}else { //LAST RESORT COMMAND TO CLOSE //V7 else if(XACTUAL > move_to_position) {

  Serial.println("V7 LAST RESORT");
  move_direction=1;
  sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
  sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX     
  sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_close_high); // 
  sendData(0x10 + 0x80, close_current_high);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x40;
  
  if (!(sendData(0x35, 0) & 0x40) == 0) { //If Stall while moving 1 to 2
      Serial.println("MOVE CLOSE STALLED");
      //buzzer_flag = true;
      move_close_stall = true;       
      sendData(0x27+0x80, 0); //VMAX  
      break;
    }

  if (!digitalRead(position_1_sensor)) {
      Serial.println("TRIPPED AT POSITION 1");
      sendData(0x26 + 0x80, MOVE_DECEL);//AMAX
      sendData(0x27+0x80, 0); //Set velocity to 0
      break;  // end loop and stop motor
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
}

  if(commandBuzzer==true){
  for (int i=1; i <= 1; i++){
    ledcWriteTone(moveChannel, moveFreq);
    unsigned long time_now;
    time_now = millis();
    while(millis() < time_now + 800){
    ledcWrite(moveChannel, 255);
    }
    ledcWrite(moveChannel, 0);
    delay(100);
    }
  } 
    
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  
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

  digitalWrite(ENABLE_PIN, HIGH);
  motor_running = false;
  blynk_update=true;
  
}


void position_close(){

     Serial.println("position_close function");
     digitalWrite(ENABLE_PIN, LOW);
     sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
     sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
     sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
     sendData(0x34 + 0x80, 0x00000000); //Stallguard off
     sendData(0x10 + 0x80, setup_current);//close_current);
     sendData(0x21 + 0x80, 0);      // XACTUAL=0 to start motion
              
              int trip = 0;
              while(trip<1){
                  
                  if  (digitalRead(position_1_sensor)==LOW) {
                      Serial.println("Sensor 1 Tripped");
                      sendData(0x26+0x80, MOVE_DECEL_CLOSED); //DMAX
                      sendData(0x27+0x80, 0); //VMAX Set velocity to 0
                      trip=1;
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
              
    digitalWrite(ENABLE_PIN, HIGH);

}

void position_open(){

     Serial.println("position_open function");
     digitalWrite(ENABLE_PIN, LOW);
     sendData(0x20 + 0x80, 1); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
     sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
     sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
     sendData(0x34 + 0x80, 0x00000000); //Stallguard off
     sendData(0x10 + 0x80, setup_current);//close_current);
     sendData(0x21 + 0x80, 0);      // XACTUAL=0 to start motion
              
              int trip = 0;
              while(trip<1){
                  
                  if  (digitalRead(position_2_sensor)==LOW) {
                      Serial.println("Sensor 2 Tripped");
                      sendData(0x26+0x80, MOVE_DECEL_CLOSED); //DMAX
                      sendData(0x27+0x80, 0); //VMAX Set velocity to 0
                      trip=1;
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
              
    digitalWrite(ENABLE_PIN, HIGH);

}





/*  ====================================

      BEGIN MOTOR DRIVER BACKEND

    ====================================
*/

// exchange data with the TMC5130 (DO NOT EDIT)
unsigned long sendData(unsigned long address, unsigned long datagram){
  //TMC5130 takes 40 bits of data: 8 address and 32 data
  delay(10);
  uint8_t stat;
  unsigned long i_datagram=0;
  digitalWrite(chipCS,LOW);
  delayMicroseconds(10);
  stat = SPI.transfer(address);
  i_datagram |= SPI.transfer((datagram >> 24) & 0xff);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram >> 16) & 0xff);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram >> 8) & 0xff);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram) & 0xff);
  digitalWrite(chipCS,HIGH);
  return i_datagram;
}

void setup_motors() {
  // put your setup code here, to run once:
  pinMode(chipCS, OUTPUT);
  pinMode(CLOCKOUT, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(chipCS, HIGH);
  digitalWrite(ENABLE_PIN, HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin(SCLK, MISO, MOSI, chipCS); // SCLK, MISO, MOSI, SS

  #ifdef Model_D
  //sendData(0x00 + 0x80, 0x04);     // General settings /GCONF
  //sendData(0x00 + 0x80, 0x14);     // General settings /GCONF

  if(CLOSE_POSITION==2){
    sendData(0x00+0x80, 0x00);     // General settings /GCONF
    }else{
    sendData(0x00+0x80, 0x10);     // General settings /GCONF
   }

  sendData(0x6C + 0x80, 0x000101D5);//0x00010134);   // CHOPCONF
  sendData(0x90, 0x00001100);     // IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x20+0x80, 0);      // RAMPMODE=0
  sendData(0x28 + 0x80, MOVE_DECEL); //Decelerate super fast DMAX
  sendData(0xA4, 30); //A1
  sendData(0xAA, MOVE_DECEL);     //D1
  sendData(0xA3, 0);         // VSTART
  sendData(0xAB, 10);        //VSTOP
  sendData(0xA5, 0);      //V1

  sendData(0x21+0x80, XACTUAL);//XACTUAL
  sendData(0x2D+0x80, XACTUAL); //XTARGET

  
  //sendData(0x09 + 0x80,  0x00010606);  // writing value 0x00010606 = 67078 = 0.0 to address 5 = 0x09(SHORT_CONF)
  //sendData(0x0A + 0x80,  0x00080400);  // writing value 0x00080400 = 525312 = 0.0 to address 6 = 0x0A(DRV_CONF)
  //sendData(0x0B + 0x80,  0x00000000);  // writing value 0x00000000 = 0 = 0.0 to address 7 = 0x0B(GLOBAL_SCALER)
  //sendData(0x14 + 0x80,  0x00000000);  // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  //sendData(0x15 + 0x80,  0x00000000);  // writing value 0x00000010 = 16 = 0.0 to address 12 = 0x15(THIGH)
  //sendData(0x34 + 0x80,  0x00000000);  // writing value 0x00000400 = 1024 = 0.0 to address 26 = 0x34(SW_MODE) //Disable stallGuard
  //sendData(0x6D + 0x80,  0x01000000);  //  Enable SG filtering

  
  sendData(0x3A + 0x80,  0x00010000);  // writing value 0x00010000 = 65536 = 0.0 to address 29 = 0x3A(ENC_CONST)
  sendData(0x3D + 0x80,  0x00000000);  // writing value 0x00000000 = 0 = 0.0 to address 30 = 0x3D(ENC_DEVIATION)
  sendData(0x60 + 0x80,  0xAAAAB554);  // writing value 0xAAAAB554 = 0 = 0.0 to address 31 = 0x60(MSLUT[0])
  sendData(0x61 + 0x80,  0x4A9554AA);  // writing value 0x4A9554AA = 1251300522 = 0.0 to address 32 = 0x61(MSLUT[1])
  sendData(0x62 + 0x80,  0x24492929);  // writing value 0x24492929 = 608774441 = 0.0 to address 33 = 0x62(MSLUT[2])
  sendData(0x63 + 0x80,  0x10104222);  // writing value 0x10104222 = 269500962 = 0.0 to address 34 = 0x63(MSLUT[3])
  sendData(0x64 + 0x80,  0xFBFFFFFF);  // writing value 0xFBFFFFFF = 0 = 0.0 to address 35 = 0x64(MSLUT[4])
  sendData(0x65 + 0x80,  0xB5BB777D);  // writing value 0xB5BB777D = 0 = 0.0 to address 36 = 0x65(MSLUT[5])
  sendData(0x66 + 0x80,  0x49295556);  // writing value 0x49295556 = 1227445590 = 0.0 to address 37 = 0x66(MSLUT[6])
  sendData(0x67 + 0x80,  0x00404222);  // writing value 0x00404222 = 4211234 = 0.0 to address 38 = 0x67(MSLUT[7])
  sendData(0x68 + 0x80,  0xFFFF8056);  // writing value 0xFFFF8056 = 0 = 0.0 to address 39 = 0x68(MSLUTSEL)
  sendData(0x69 + 0x80,  0x00F70000);  // writing value 0x00F70000 = 16187392 = 0.0 to address 40 = 0x69(MSLUTSTART)
  #endif


  #ifdef Model_W
  sendData(0x28 + 0x80, MOVE_DECEL); //Decelerate super fast DMAX
  sendData(0xA4, 30); //A1
  sendData(0xAA, MOVE_DECEL);     //D1
  sendData(0xA3, 0);         // VSTART
  sendData(0xAB, 10);        //VSTOP
  sendData(0xA5, 0);      //V1

  sendData(0x21+0x80, XACTUAL);//XACTUAL
  sendData(0x2D+0x80, XACTUAL); //XTARGET
  sendData(0x6C+0x80, 0x000101D5);     // CHOPCONF
  sendData(0x10+0x80, 0x00010D00);     // IHOLD_IRUN // 0x00011900 = 25 = 2 Amps // 0x00010D00 = 13 = 1 Amp
  sendData(0x20+0x80, 0x00000000);      // RAMPMODE=0

  sendData(0x60+0x80,  0xAAAAB554);    // writing value 0xAAAAB554 = 0 = 0.0 to address 25 = 0x60(MSLUT[0])
  sendData(0x61+0x80,  0x4A9554AA);    // writing value 0x4A9554AA = 1251300522 = 0.0 to address 26 = 0x61(MSLUT[1])
  sendData(0x62+0x80,  0x24492929);    // writing value 0x24492929 = 608774441 = 0.0 to address 27 = 0x62(MSLUT[2])
  sendData(0x63+0x80,  0x10104222);    // writing value 0x10104222 = 269500962 = 0.0 to address 28 = 0x63(MSLUT[3])
  sendData(0x64+0x80,  0xFBFFFFFF);    // writing value 0xFBFFFFFF = 0 = 0.0 to address 29 = 0x64(MSLUT[4])
  sendData(0x65+0x80,  0xB5BB777D);    // writing value 0xB5BB777D = 0 = 0.0 to address 30 = 0x65(MSLUT[5])
  sendData(0x66+0x80,  0x49295556);    // writing value 0x49295556 = 1227445590 = 0.0 to address 31 = 0x66(MSLUT[6])
  sendData(0x67+0x80,  0x00404222);    // writing value 0x00404222 = 4211234 = 0.0 to address 32 = 0x67(MSLUT[7])
  sendData(0x68+0x80,  0xFFFF8056);    // writing value 0xFFFF8056 = 0 = 0.0 to address 33 = 0x68(MSLUTSEL)
  sendData(0x69+0x80,  0x00F70000);    // writing value 0x00F70000 = 16187392 = 0.0 to address 34 = 0x69(MSLUTSTART)

  if(fast_loud==true){
    MOVE_OPEN_VELOCITY = 400000;
    MOVE_CLOSE_VELOCITY = 400000;
    sendData(0x70 + 0x80, 0);
    if(CLOSE_POSITION==2){
    sendData(0x00+0x80, 0x00);     // General settings /GCONF
    }else{
    sendData(0x00+0x80, 0x10);     // General settings /GCONF
    }
  }else{
    MOVE_OPEN_VELOCITY = 100000;
    MOVE_CLOSE_VELOCITY = 100000; 
    sendData(0x70 + 0x80, 0x000504C8);
    if(CLOSE_POSITION==2){
    sendData(0x00+0x80, 0x04);     // General settings /GCONF
    }else{
    sendData(0x00+0x80, 0x14);     // General settings /GCONF
   }
  }
  #endif
}
