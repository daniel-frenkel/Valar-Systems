void move_motor(){
  
  digitalWrite(ENABLE_PIN, LOW);
  
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
  
  Serial.print("move_to_steps: ");
  Serial.println(move_to_steps);

  sendData(0xA0, 0);     // Positioning mode 0
  sendData(0x26 + 0x80, 300);       //AMAX
  sendData(0x28 + 0x80, 800);       // DMAX
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /

  int maxOpenTime = millis() + 60000; //max amount of time to close in case of problem 

  if(move_to_steps == XACTUAL)
  {
  Serial.println("move_to_steps == XACTUAL");
  }
  else
  {

  sendData(0x27 + 0x80, max_speed);
  sendData(0x14 + 0x80, max_speed - 100);
  sendData(0x6D + 0x80, stall);
  sendData(0x10 + 0x80, current);
  sendData(0x2D + 0x80, move_to_steps); //XTARGET
  
  while (millis()<maxOpenTime) { // wait for position_reached flag
    
     sendData(0x35, 0) & 0x200; 

    if (!(sendData(0x35, 0) & 0x200) == 0) { //Position reached
      Serial.println("move_to_steps REACHED");
      break;    
      }

    sendData(0x35, 0) & 0x40;
    
    if (!(sendData(0x35, 0) & 0x40) == 0) { //IF STALL EVENT
      sendData(0x27+0x80, 0); //VMAX
      Serial.println("STALLED CLOSED");
      break;
    }

    delay(1);
    
   }
  }
  
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  XACTUAL = sendData(0x21, 0);
  
  Serial.print("XACTUAL: ");
  Serial.println(XACTUAL);
  
  delay(300);
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.println("MOVEMENT COMPLETE");
}



void setZero()
{
      XACTUAL=0;
      //sendData(0x2D+0x80, 0); // XTARGET to zero
      sendData(0x21+0x80, 0); // XACTUAL to zero
      Serial.print("XACTUAL: ");
      Serial.println(XACTUAL);
      
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

// put your setup code here, to run once:
void setup_motors(){ 
  pinMode(chipCS, OUTPUT);
  pinMode(CLOCKOUT, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);

  digitalWrite(chipCS, HIGH);
  digitalWrite(ENABLE_PIN, HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin(SCLK, MISO, MOSI, chipCS);

  sendData(0x00+0x80, 0x0);     // General settings / en_pwm_mode OFF
  sendData(0x6C+0x80, 0x000101D5);     // CHOPCONF
  sendData(0x10+0x80, 0x00010D00);     // IHOLD_IRUN // 0x00011900 = 25 = 2 Amps // 0x00010D00 = 13 = 1 Amp
  sendData(0x20+0x80, 0x00000000);      // RAMPMODE=0

  sendData(0x28 + 0x80, 800); 
  sendData(0xA4, 30); //A1
  sendData(0xAA, 3000);     //D1
  sendData(0xA3, 0);         // VSTART
  sendData(0xAB, 10);        //VSTOP
  sendData(0xA5, 0);      //V1
  
  sendData(0x2D+0x80, 0); //XTARGET
  sendData(0x21+0x80, 0); // set XACTUAL to zero

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
 }
