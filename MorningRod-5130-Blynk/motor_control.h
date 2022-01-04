#include <Arduino.h>
#include <SPI.h>

unsigned long sendData(unsigned long address, unsigned long datagram);
void stopMotor(); // track motor is motor two
void delayStall(long timeout);
void waitStall(long timeout);
void turnMotor(int dir);
bool motor_running = false;

void move_close(){
  
  Serial.println("Closing to: ");
  Serial.println(-move_percent);
  
  digitalWrite(ENABLE_PIN,LOW); // enable the TMC5130
  sendData(0x10+0x80, current_close);
  sendData(0xA0,0); //RAMPMODE=0
  sendData(0xB4, 0x000); // Disable stallguard /
  sendData(0xB4, 0x400); // Enable stallguard /
  sendData(0x14+0x80, velocity-100); // VCOOLTHRS: This value disable stallGuard below a certain velocity to prevent premature stall
  sendData(0x6D+0x80, stall_close);     // STALLGUARD_CLOSE
  
  sendData(0x24+0x80, 100); //A1
  sendData(0x26+0x80, 300); //AMAX
  sendData(0x28+0x80, 800);     // DMAX
  sendData(0x2A+0x80, 3000); //D1
  sendData(0x23+0x80, 0);     // VSTART
  sendData(0x2B+0x80, 10); //VSTOP
  sendData(0x25+0x80, 2500); //V1
  sendData(0x27+0x80, velocity); //VMAX

  if (reverse_flag==true){
  sendData(0xAD, move_percent); //XTARGET: Positive makes it move right
  }else{
  sendData(0xAD, -move_percent); //XTARGET: Positive makes it move right  
  }
  
  sendData(0xA1, 0); // set XACTUAL to zero

  int sg_result = 0;
  int sg_result_loop = 0;
  int loop_counter = 0;
  int timeout_flag = 0;

  move_close_stall = false;
  
while (((sendData(0x35, 0) & 0x200) == 0)) { // wait for position_reached flag
    delay(5);
   
    sg_result_loop = sendData(0x6F, 0) & 1023;
    //Finds the max SG result to output to the user
    if (sg_result_loop > sg_result) {
      sg_result = sg_result_loop;
    }
    Serial.print("Position Flag: ");
    Serial.println(sendData(0x35, 0) & 0x200);
    
    if (!(sendData(0x35, 0) & 0x40) == 0) { //IF STALL EVENT
      Serial.print("STALLED CLOSED");
      move_close_stall = true;
      break;
    }
  }
    String payload;
    if(move_close_stall==true){
    payload = "STALLED CLOSED";
    }
    else{
    payload = "CLOSED";
    }
    if(client.connected()){
    client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
    }
  
  Serial.print("sg_result: ");
  Serial.println(sg_result);
  

  sendData(0x26+0x80, 65535); //Decelerate super fast
  sendData(0x27+0x80, 0); //Set velocity to 0
  delay(300);
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.print("CLOSE COMPLETE");
}

void move_open(){

  Serial.println("Opening to: ");
  Serial.println(move_percent);

  digitalWrite(ENABLE_PIN,LOW);       // enable the TMC5130
  Serial.println("Enabled TMC");
  sendData(0x10+0x80, current_open);     // 7 = 0.52A current for open function
  sendData(0xA0,0); //RAMPMODE=0
  sendData(0xB4, 0x000); // Disable stallguard /
  sendData(0xB4, 0x400); // Enable stallguard /
  sendData(0x14+0x80, velocity-100); // VCOOLTHRS: This value disable stallGuard below a certain velocity to prevent premature stall
  sendData(0x6D+0x80, stall_open);     // STALLGUARD_OPEN
  
  sendData(0x24+0x80, 100); //A1
  sendData(0x26+0x80, 300); //AMAX
  sendData(0x28+0x80, 800);     // DMAX
  sendData(0x2A+0x80, 3000); //D1
  sendData(0x23+0x80, 0);     // VSTART
  sendData(0x2B+0x80, 10); //VSTOP
  sendData(0x25+0x80, 2500); //V1
  sendData(0x27+0x80, velocity); //VMAX

  if (reverse_flag==true){
     sendData(0xAD, -move_percent); //XTARGET: Positive makes it move right
     }else{
     sendData(0xAD, move_percent); //XTARGET: Positive makes it move right  
     }

  sendData(0xA1, 0); // set XACTUAL to zero
  
  motor_running = true;

  int sg_result = 0;
  int sg_result_loop = 0;
  int loop_counter = 0;
  int timeout_flag = 0;
  
  move_open_stall = false;

  Serial.println("Starting Loop");
  
while (((sendData(0x35, 0) & 0x200) == 0)) { // wait for position_reached flag
    delay(5);
   
    sg_result_loop = sendData(0x6F, 0) & 1023;

    //Finds the max SG result to output to the user
    if (sg_result_loop > sg_result) {
      sg_result = sg_result_loop;
    }
    
    Serial.print("Position Flag: ");
    Serial.println(sendData(0x35, 0) & 0x200);
    
    if (!(sendData(0x35, 0) & 0x40) == 0) { //IF STALL EVENT
      Serial.print("STALLED OPEN");
      move_open_stall = true;
      break;
    }
  }
    String payload;
    if(move_open_stall==true){
    payload = "STALLED OPENED";
    }
    else{
    payload = "OPENED";
    }
    if(client.connected()){
    client.publish(mqtt_state_topic.c_str(), (char*) payload.c_str());
    }
      
  Serial.print("sg_result: ");
  Serial.println(sg_result);
  
  sendData(0x26+0x80, 65535); //Decelerate super fast
  sendData(0x27+0x80, 0); //Set velocity to 0
  delay(300);
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.println("OPEN COMPLETE: ");
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

  sendData(0x00+0x80, 0x0);     // General settings / en_pwm_mode OFF
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
}
