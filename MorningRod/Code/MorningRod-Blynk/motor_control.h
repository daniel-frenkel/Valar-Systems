//MOTOR 1 = LEFT MOTOR
//MOTOR 2 = RIGHT MOTOR


#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <SPI.h>
#include <Preferences.h>
#include "pins.h"

#ifndef DEBUG_STREAM
#define DEBUG_STREAM Serial
#endif

#define DIR_OPEN 1
#define DIR_CLOSE 2

#define MOVE_OPEN 5
#define MOVE_CLOSE 6

#define COOLCONF_DEFAULT 0

#define GET_VELOCITY preferences.getLong("velocity",100000)

//#define GET_TRACK_DISTANCE preferences.getFloat("track_distance",100000)
//#define GET_SHAFT_DEGREES preferences.getFloat("shaft_degrees",20000)
//#define GET_TRACK_DISTANCE_PERCENT preferences.getFloat("track_distance_percent",100000)
//#define GET_SHAFT_DEGREES_CUSTOM preferences.getFloat("shaft_degrees_custom",20000)

//#define TOTAL_STEPS 1500000

extern Preferences preferences;   // preferences used to configure motor stallguard and curve values.

// These empty function definitions allow the functions to be called before they are created.
// They are written towards the bottom of the file.

unsigned long sendData(unsigned long address, unsigned long datagram);
void stopMotor1(); // track motor is motor two
void stopMotor2(); // shaft motor is motor one
void delayStall1(long timeout);
void delayStall2(long timeout);
void waitStall1(long timeout);
void waitStall2(long timeout);
void turnMotor1(int dir);
void turnMotor2(int dir);

// these variables keep track of which motors are running
bool motor1_running = false;
bool motor2_running = false;

void move_close(){
  
  digitalWrite(ENABLE_PIN,LOW);       // enable the TMC5072
  sendData(0xA0, 0); // Positioning mode 0
  sendData(0xC0, 0); // Positioning mode 0

  sendData(0xB4, 0x000); // Disable stallguard
  sendData(0xD4, 0x000); // Disable stallguard
  
  sendData(0xB1, GET_VELOCITY-1); // VCOOLTHRS: This value disable stallGuard below the VMAX velocity to prevent premature stallguard
  sendData(0xD1, GET_VELOCITY-1); // VCOOLTHRS: This value disable stallGuard below the VMAX velocity to prevent premature stallguard

  
  sendData(0xA4, 1000); //A1
  sendData(0xC4, 1000); //A1
  
  sendData(0xA6, 4000); //AMAX
  sendData(0xC6, 4000); //AMAX
  
  sendData(0xA8, 800); // DMAX
  sendData(0xC8, 800); // DMAX
  
  sendData(0xAA, 3000); //D1
  sendData(0xCA, 3000); //D1
  
  sendData(0xA3, 0); // VSTART
  sendData(0xC3, 0); // VSTART
  
  sendData(0xAB, 10); //VSTOP
  sendData(0xCB, 10); //VSTOP
  
  sendData(0xA5, 40000); //V1
  sendData(0xC5, 40000); //V1
  
  sendData(0xA7, GET_VELOCITY); //VMAX
  sendData(0xC7, GET_VELOCITY); //VMAX

  sendData(0xAD, -MOVE_PERCENT); //XTARGET: Positive makes it move right
  sendData(0xCD, -MOVE_PERCENT); //XTARGET: Positive makes it move right
  
  DEBUG_STREAM.println("Closing to: ");
  DEBUG_STREAM.println(MOVE_PERCENT);

  
  // clear flags
  sendData(0x35, 0);
  sendData(0x55, 0);
  
  sendData(0xA1, 0);                  // set XACTUAL to zero
  sendData(0xC1, 0);                  // set XACTUAL to zero

  delay(5);
  
  sendData(0xB4, 0x400); // Enable stallguard
  sendData(0xD4, 0x400); // Enable stallguard

  //sendData(0x2C+0x80, 100000); // Enable stallguard
  //sendData(0x4C+0x80, 100000); // Enable stallguard

  motor1_running = true;
  motor2_running = true;

  //int stall_1_flag = 0;
  //int stall_2_flag = 0;

  while(motor1_running||motor2_running){   // wait for position_reached flag OR a STALL EVENT
    uint32_t m1_pos = sendData(0x35, 0);
    //uint32_t m1_sg  = sendData(0x6F,0);
    
    uint32_t m2_pos = sendData(0x55, 0);
    //uint32_t m2_sg  = sendData(0x7F,0);

    if((m1_pos&0x40)!=0){//((m1_sg&0x1000000)!=0)){
      Serial.println("Motor 1 stalled!");
      //stopMotor1();//break;
      motor1_running = false;
      sendData(0xC3, 0);    // set VMAX and VSTART to zero, then enable positioning mode ////
      sendData(0xC7, 0);    //  > doing this stops the motor ////
    }else if((m1_pos&0x200)!=0){
      Serial.println("Motor 1 completed motion");
      stopMotor1();
    }
    
    
    if((m2_pos&0x40)!=0){//((m1_sg&0x1000000)!=0)){
      Serial.println("Motor 2 stalled!");
      //stopMotor2();//break;
      motor2_running = false;
      sendData(0xA3, 0);    // set VMAX and VSTART to zero, then enable positioning mode ////
      sendData(0xA7, 0);    //  > doing this stops the motor ////
    }else if((m2_pos&0x200)!=0){
      Serial.println("Motor 2 completed motion");
      stopMotor2();
    }
    
    /*if(((m2_pos&0x200)!=0)||((m2_pos&0x40)!=0)){//((m2_sg&0x1000000)!=0)){
      stopMotor2();
    }*/
    
    delay(1);
  }

  stopMotor1();
  stopMotor2();

  Serial.println("[close complete]");
   
  digitalWrite(ENABLE_PIN,HIGH);
}

void move_open(){

  digitalWrite(ENABLE_PIN,LOW);       // enable the TMC5072
  sendData(0xA0, 0);     // Positioning mode 0
  sendData(0xC0, 0); 

  //sendData(0xAC, 6000000);     // TZEROWAIT
  //sendData(0xCC, 6000000);     // TZEROWAIT
  
  sendData(0xB1, GET_VELOCITY-1); // VCOOLTHRS: This value disable stallGuard below a certain velocity to prevent premature stall
  sendData(0xD1, GET_VELOCITY-1);
  
  sendData(0xA4, 1000); //A1
  sendData(0xC4, 1000); //A1
  
  sendData(0xA6, 4000); //AMAX
  sendData(0xC6, 4000); //AMAX
  
  sendData(0xA8, 800);     // DMAX
  sendData(0xC8, 800);     // DMAX
  
  sendData(0xAA, 3000); //D1
  sendData(0xCA, 3000); //D1
  
  sendData(0xA3, 0);     // VSTART
  sendData(0xC3, 0);     // VSTART
  
  sendData(0xAB, 10); //VSTOP
  sendData(0xCB, 10); //VSTOP
  
  sendData(0xA5, 40000); //V1
  sendData(0xC5, 40000); //V1
  
  sendData(0xA7, GET_VELOCITY); //VMAX
  sendData(0xC7, GET_VELOCITY); //VMAX

  sendData(0xAD, MOVE_PERCENT); //XTARGET: Positive makes it move right
  sendData(0xCD, MOVE_PERCENT); //XTARGET: Positive makes it move right

  delay(5);

  // clear flags
  sendData(0x35, 0);
  sendData(0x55, 0);
  
  sendData(0xB4, 0x400); // Enable stallguard
  sendData(0xD4, 0x400); 
  
  DEBUG_STREAM.println("Closing to: ");
  DEBUG_STREAM.println(-MOVE_PERCENT);
  
  sendData(0xA1, 0);                  // set XACTUAL to zero // Left Motor
  sendData(0xC1, 0);                  // set XACTUAL to zero // Right Motor
  
  motor1_running = true;
  motor2_running = true;

  while(motor1_running||motor2_running){   // wait for position_reached flag OR a STALL EVENT
    uint32_t m1_pos = sendData(0x35, 0);
    //uint32_t m1_sg  = sendData(0x6F,0);
    
    uint32_t m2_pos = sendData(0x55, 0);
    //uint32_t m2_sg  = sendData(0x7F,0);

    if((m1_pos&0x40)!=0){//((m1_sg&0x1000000)!=0)){
      Serial.println("Motor 1 stalled!");
      //stopMotor1();//break;
      motor1_running = false;
      sendData(0xC3, 0);    // set VMAX and VSTART to zero, then enable positioning mode ////
      sendData(0xC7, 0);    //  > doing this stops the motor ////
    }else if((m1_pos&0x200)!=0){
      Serial.println("Motor 1 completed motion");
      stopMotor1();
    }
    
    
    if((m2_pos&0x40)!=0){//((m1_sg&0x1000000)!=0)){
      Serial.println("Motor 2 stalled!");
      //stopMotor2();//break;
      motor2_running = false;
      sendData(0xA3, 0);    // set VMAX and VSTART to zero, then enable positioning mode ////
      sendData(0xA7, 0);    //  > doing this stops the motor ////
    }else if((m2_pos&0x200)!=0){
      Serial.println("Motor 2 completed motion");
      stopMotor2();
    }
    
    /*if(((m2_pos&0x200)!=0)||((m2_pos&0x40)!=0)){//((m2_sg&0x1000000)!=0)){
      stopMotor2();
    }*/
    
    delay(1);
  }

  stopMotor1();
  stopMotor2();
  digitalWrite(ENABLE_PIN,HIGH);
  Serial.println("[open complete]");
}

/*  ====================================
 * 
 *    BEGIN MOTOR DRIVER BACKEND
 * 
 *  ====================================
 */

// this function disables the TMC5072 if both motors are off.
//  Under no circumstance does it enable the driver, this is done elsewhere!
void opt_motors(){
  if(!(motor1_running||motor2_running))digitalWrite(ENABLE_PIN,HIGH);
}

// gracefully stops motor1 and handles background optimizations
void stopMotor1(){
  sendData(0xA3, 0);          // set VMAX and VSTART to zero, then enable positioning mode////
  sendData(0xA7, 0);          //  > doing this stops the motor////
  sendData(0xA0, 0);          // ////
  while(sendData(0x22, 0)!=0) // wait for the motor to stop (VACTUAL != 0 until stopped)////
    delayMicroseconds(10);
  sendData(0xA1, 0);          // target=xactual=0 to keep motor stopped////
  sendData(0xAD, 0);          // ////
  
  sendData(0xA3, 0x180);      // fix VMAX and VSTART to previous values so motor can run again////
  sendData(0xA7, GET_VELOCITY); ////
  motor1_running = false;// mark that the shaft motor is stopped
  opt_motors();               // disable the motor driver if possible
}

// gracefully stops motor1 and handles background optimizations
void stopMotor2(){
  sendData(0xC3, 0);          // set VMAX and VSTART to zero, then enable positioning mode ////
  sendData(0xC7, 0);          //  > doing this stops the motor ////
  sendData(0xC0, 0);          // ////
  while(sendData(0x42, 0)!=0) // wait for the motor to stop (VACTUAL != 0 until stopped) ////
    delayMicroseconds(10);
  sendData(0xC1, 0);          // target=xactual=0 to keep motor stopped ////
  sendData(0xCD, 0);          // ////
  
  sendData(0xC3, 0x180);      // fix VMAX and VSTART to previous values so motor can run again ////
  sendData(0xC7, GET_VELOCITY); ////
  motor2_running = false;// mark that the shaft motor is stopped
  opt_motors();               // disable the motor driver if possible
}


// exchange data with the TMC5072
unsigned long sendData(unsigned long address, unsigned long datagram){
  //TMC5072 takes 40 bits of data: 8 address and 32 data
  
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

void setup_motors(){
  // put your setup code here, to run once:
  pinMode(chipCS,OUTPUT);
  pinMode(CLOCKOUT,OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(chipCS,HIGH);
  //digitalWrite(ENABLE_PIN,HIGH);
  digitalWrite(ENABLE_PIN,LOW);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin(22,23,19,21); // SCLK, MISO, MOSI, SS

  sendData(0x00+0x80, 0x00000000);     // GCONF
  
  sendData(0x34+0x80, 0x400);          // make stallguard stop the motor
  sendData(0x54+0x80, 0x400);          // make stallguard stop the motor
  
  sendData(0x6C+0x80, 0x000101D5);     // CHOPCONF_M1
  sendData(0x7C+0x80, 0x000101D5);     // CHOPCONF_M2 // My configuration /0x00010134
  
  sendData(0x30+0x80, 0x00011400);     // IHOLD_IRUN_M1 // 0x00071600 0x11400
  sendData(0x50+0x80, 0x00011400);     // IHOLD_IRUN_M2 // SHAFT MOTOR // 0x00071400
  
  sendData(0x6D+0x80, 0); //COOLCONF 1  
  sendData(0x7D+0x80, 0); //COOLCONF 2


  sendData(0xBA, 0x00010000); // ENC_CONST_M1
  sendData(0xE0, 0xAAAAB554); //MSLUT0
  sendData(0xE1, 0x4A9554AA); //MSLUT0
  sendData(0xE2, 0x24492929); //MSLUT0
  sendData(0xE3, 0x10104222); //MSLUT0
  sendData(0xE4, 0xFBFFFFFF); //MSLUT0
  sendData(0xE5, 0xB5BB777D); //MSLUT0
  sendData(0xE6, 0x49295556); //MSLUT0
  sendData(0xE7, 0x00404222); //MSLUT0
  sendData(0xE8, 0xFFFF8056); //MSLUT0
  sendData(0xE9, 0x00F70000); //MSLUT0
  
  //Standard values for speed and acceleration
  int q=preferences.getInt("stallguard_1", -9);
  DEBUG_STREAM.print("Stall_1 value: ");
  DEBUG_STREAM.println(q);
  q&=0x7F;
  q=q<<16;
  sendData(0xED, COOLCONF_DEFAULT|q);     // STALLGUARD_M1
  
  q=preferences.getInt("stallguard_2", -9);
  DEBUG_STREAM.print("Stall_2 value: ");
  DEBUG_STREAM.println(q);
  q&=0x7F;
  q=q<<16;
  sendData(0xFD, COOLCONF_DEFAULT|q);     // STALLGUARD_M2
  
  stopMotor1();
  stopMotor2();
}
#endif
