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

int STALLGUARD_OPEN = 0;
int STALLGUARD_CLOSE = 0;

#define COOLCONF_DEFAULT 0
//#define GET_VELOCITY preferences.getLong("velocity",100000)

int stall_close;// = preferences.getULong("stall_close", 0);
int stall_open;// = preferences.getULong("stall_open", 0);

int current_close = 0;
int current_open = 0;

float move_percent;
float move_distance;
int velocity;

#define CURRENT_OPEN preferences.getInt("stall_open", 63)
#define CURRENT_CLOSE preferences.getInt("stall_close", 63)

extern Preferences preferences;   // preferences used to configure motor stallguard and curve values.

unsigned long sendData(unsigned long address, unsigned long datagram);
void stopMotor(); // track motor is motor two
void delayStall(long timeout);
void waitStall(long timeout);
void turnMotor(int dir);

bool motor_running = false;

void move_close(){
  
  DEBUG_STREAM.println("Opening to: ");
  DEBUG_STREAM.println(-move_percent);
  
  digitalWrite(ENABLE_PIN,LOW);       // enable the TMC5130
  sendData(0x10+0x80, 0b00010001101000000000);     // 11900 =1.68A works //
  sendData(0xA0,0x00000000); //RAMPMODE=0
  sendData(0x14+0x80, velocity-100); // VCOOLTHRS: This value disable stallGuard below a certain velocity to prevent premature stall
  sendData(0x6D+0x80, stall_close);     // STALLGUARD_CLOSE
  
  sendData(0x24+0x80, 1000); //A1
  sendData(0x26+0x80, 4000); //AMAX
  sendData(0x28+0x80, 800);     // DMAX
  sendData(0x2A+0x80, 3000); //D1
  sendData(0x23+0x80, 0);     // VSTART
  sendData(0x2B+0x80, 10); //VSTOP
  sendData(0x25+0x80, 40000); //V1
  sendData(0x27+0x80, velocity); //VMAX

  delay(5);
 
  // clear flags
  sendData(0x35, 0);
  sendData(0x34+0x80, 0x400); // Enable stallguard
    
  sendData(0xAD, move_percent); //XTARGET: Positive makes it move right
  sendData(0xA1, 0); // set XACTUAL to zero
  
  motor_running = true;
    while(((sendData(0x35, 0)&0x200)==0)&&((sendData(0x35,0)&0x40)==0)){   // wait for position_reached flag OR a STALL EVENT
    delayMicroseconds(500);  // shortened the delay to make the following if statement more sensitive
  }

  stopMotor();
  digitalWrite(ENABLE_PIN,HIGH);
  Serial.println("[close complete]");
}

void move_open(){
  
  DEBUG_STREAM.println("Opening to: ");
  DEBUG_STREAM.println(move_percent);

  digitalWrite(ENABLE_PIN,LOW);       // enable the TMC5130
  sendData(0x10+0x80, 0x10700);     // 7 = 0.52A current for open function
  sendData(0xA0,0x00000000); //RAMPMODE=0
  sendData(0x14+0x80, velocity-100); // VCOOLTHRS: This value disable stallGuard below a certain velocity to prevent premature stall
  sendData(0x6D+0x80, stall_open);     // STALLGUARD_OPEN
  
  sendData(0x24+0x80, 1000); //A1
  sendData(0x26+0x80, 4000); //AMAX
  sendData(0x28+0x80, 800);     // DMAX
  sendData(0x2A+0x80, 3000); //D1
  sendData(0x23+0x80, 0);     // VSTART
  sendData(0x2B+0x80, 10); //VSTOP
  sendData(0x25+0x80, 40000); //V1
  sendData(0x27+0x80, velocity); //VMAX

  delay(5);
 
  // clear flags
  sendData(0x35, 0);
  sendData(0x34+0x80, 0x400); // Enable stallguard
  
  sendData(0xAD, -move_percent); //XTARGET: Positive makes it move right
  sendData(0xA1, 0); // set XACTUAL to zero
  
  motor_running = true;

    while(((sendData(0x35, 0)&0x200)==0)&&((sendData(0x35,0)&0x40)==0)){   // wait for position_reached flag OR a STALL EVENT
    delayMicroseconds(500);  // shortened the delay to make the following if statement more sensitive
  }

  stopMotor();
  digitalWrite(ENABLE_PIN,HIGH);
  Serial.println("[open complete]");
}


// this function disables the TMC5130
//  Under no circumstance does it enable the driver, this is done elsewhere!
void opt_motors(){
  if(!(motor_running))digitalWrite(ENABLE_PIN,HIGH);
}

// gracefully stops motor and handles background optimizations
void stopMotor(){
  sendData(0x23+0x80, 0);             // set VMAX and VSTART to zero, then enable positioning mode
  sendData(0x27+0x80, 0);             //  > doing this stops the motor
  sendData(0x20+0x80, 0);             //
  while(sendData(0x22, 0)!=0)         // wait for the motor to stop (VACTUAL != 0 until stopped)
    delayMicroseconds(10);
  sendData(0x21+0x80, 0);             // target=xactual=0 to keep motor stopped
  sendData(0x2D+0x80, 0);             //
  sendData(0x23+0x80, 0x180);         // fix VMAX and VSTART to previous values so motor can run again
  sendData(0x27+0x80, velocity);  //
  motor_running = false;              // mark that the shaft motor is stopped
  opt_motors();                       // disable the motor driver if possible
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
  digitalWrite(ENABLE_PIN,LOW);

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
  //sendData(0x70+0x80,  0x00000000);    // PWMCONF
   
  stopMotor();

}
#endif
