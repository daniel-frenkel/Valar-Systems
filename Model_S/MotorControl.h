void move_motor_position_1() {

  motor_running = true;         // mark that the track motor is running
  move_open_stall = false;
  move_close_stall = false;
  
  Serial.println("Moving Pos 1 Motor Function");

  digitalWrite(ENABLE_PIN, LOW);      // enable the TMC
  
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

  int maxOpenTime = millis() + 20000; //max amount of time to close in case of problem

  
//V3
if(XACTUAL < move_to_position){

  Serial.println("V3");
  move_direction=2;
  sendData(0x27 + 0x80, MOVE_OPEN_VELOCITY); //VMAX
  Serial.println(MOVE_OPEN_VELOCITY);
  sendData(0x14 + 0x80, MOVE_OPEN_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_open); // 
  Serial.println(stall_open);
  sendData(0x10 + 0x80, current_open);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  Serial.println(current_open);
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 
  Serial.println(move_to_position);


  while (millis()<maxOpenTime) { // wait for position_reached flag

  Serial.println(sendData(0x35, 0) & 0x200);
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //move_to_position reached
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

    delay(1);
    
}
}

//V4
else if(XACTUAL > move_to_position){

  Serial.println("V4");
  move_direction=1;
  sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
  sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
  sendData(0x6D + 0x80, stall_close); // 
  sendData(0x10 + 0x80, current_close);   // 0x00001F00 = IHOLD_IRUN 1F00 = 3.05A 1400 = 2A
  sendData(0x2D + 0x80, move_to_position); //XTARGET: Starts Movement 

  while (millis()<maxOpenTime) { // wait for position_reached flag

  sendData(0x35, 0) & 0x200;
  
  if (!(sendData(0x35, 0) & 0x200) == 0) { //move_to_position reached
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

    delay(1);
    
}
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
  
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  
  preferences_local.putInt("XACTUAL", XACTUAL);

  Serial.print("MOVED TO POSITION: ");
  Serial.println(preferences_local.getInt("XACTUAL", 404));
  
  digitalWrite(ENABLE_PIN, HIGH);
  motor_running = false;
  blynk_update=true;
  
}

void force_close(){

     Serial.println("force_close function");

     motor_running = true; 
     digitalWrite(ENABLE_PIN, LOW);
     sendData(0x20 + 0x80, 2); // RAMPMODE=1 Velocity mode to positive / RAMPMODE=2 Velocity mode to negative
     sendData(0x26 + 0x80, MOVE_ACCEL); //AMAX
     sendData(0x27 + 0x80, MOVE_CLOSE_VELOCITY); //VMAX
     sendData(0x14 + 0x80, MOVE_CLOSE_VELOCITY - 100);  //writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
     sendData(0x34 + 0x80, 0x000);                      //Disable stallguard /
     sendData(0x34 + 0x80, 0x400);                      //Enable stallguard /
     sendData(0x10 + 0x80, current_close);              //close_current);
     sendData(0x6D + 0x80, stall_close);                // 
     sendData(0x21 + 0x80, 0);                          // XACTUAL=0 to start motion

     int maxOpenTime = millis() + 20000; //max amount of time to close in case of       
             
     while (millis()<maxOpenTime) { // wait for stall
                  
//              if (!(sendData(0x35, 0) & 0x40) == 0) { //If Stalled
//                  sendData(0x27 + 0x80, 0); //VMAX
//                  Serial.println("MOVE CLOSE STALLED");
//                  move_close_stall = true;       
//                  XACTUAL = 0;
//                  sendData(0x21 + 0x80, 0);      // XACTUAL=0
//                  sendData(0x2D + 0x80, 0);      // XTARGET=0
//                  preferences_local.putInt("XACTUAL", XACTUAL);
//                  Blynk.virtualWrite(V46, "CLOSED");
//                  break;
//                  }


             if (digitalRead(btn1)==LOW || digitalRead(btn2)==LOW){ // Button to stop
                  Serial.println("Button Stopped");
                  sendData(0x26+0x80, MOVE_DECEL); //Decelerate super fast
                  sendData(0x27+0x80, 0); //Set velocity to 0
                  delay(200);
                  break;  // end loop and stop motor
                  }
      
                  
                  delay(1);
                }
    
    digitalWrite(ENABLE_PIN, HIGH);
    motor_running = false; 
}


/*  ====================================
 * 
 *    BEGIN MOTOR DRIVER BACKEND
 * 
 *  ====================================
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

// put your setup code here, to run once:
void setup_motors(){ 
  pinMode(chipCS,OUTPUT);
  pinMode(CLOCKOUT,OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(chipCS,HIGH);
  digitalWrite(ENABLE_PIN,HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin(SCLK,MISO,MOSI,chipCS); // Edit 'pins.h' to change pins

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

  if(CLOSE_POSITION==1){
    sendData(0x00+0x80, 0x04);     // General settings /GCONF
    }else{
    sendData(0x00+0x80, 0x14);     // General settings /GCONF
   }
}
