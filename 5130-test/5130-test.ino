#include <TMCStepper.h>
#include <Arduino.h>
#include <SPI.h>

#define chipCS 27
#define SCLK 26
#define MISO_PIN 33
#define MOSI_PIN 25
#define ENABLE_PIN 32
#define R_SENSE 0.075f

//SW_SPIClass SW_SPI(MOSI_PIN, MISO_PIN, SCLK);

//TMC5130Stepper driver(SW_SPI, chipCS, R_SENSE);

TMC5160Stepper driver (chipCS, R_SENSE, MOSI_PIN, MISO_PIN, SCLK); // Software SPI

void setup() {

  Serial.begin(115200);
  delay(1000);
  pinMode(chipCS,OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(chipCS,HIGH);
  digitalWrite(ENABLE_PIN,LOW);

  //SPI.setBitOrder(MSBFIRST);
  //SPI.setClockDivider(SPI_CLOCK_DIV16);
  //SPI.setDataMode(SPI_MODE3);
  SPI.begin(SCLK,MISO_PIN,MOSI_PIN,chipCS);

  driver.begin();
  driver.en_pwm_mode(false);
  driver.toff(5);
  driver.hstrt(5);
  driver.hend(3);
  driver.tbl(2);
  
  sendData(0xB4, 0x000); // Disable stallguard /
  sendData(0x00+0x80, 0x0);     // General settings / en_pwm_mode OFF
  sendData(0x6C+0x80, 0x000101D5);     // CHOPCONF
  Serial.print("CHOPCONF: ");
  Serial.println(sendData(0x6C, 0x000101D5));
  
  //sendData(0x10+0x80, 0x00010D00);     // IHOLD_IRUN // 0x00011900 = 25 = 2 Amps // 0x00010D00 = 13 = 1 Amp
  //sendData(0x20+0x80, 0x00000000);      // RAMPMODE=0
  
  driver.RAMPMODE(0);
  driver.a1(100);
  driver.AMAX(300);
  driver.DMAX(800);
  driver.d1(3000);
  driver.VSTART(0);
  driver.VSTOP(10);
  driver.v1(2500);
  driver.VMAX(12000);
  
  Serial.print("CHOPCONF: ");
  Serial.println(sendData(0x6C, 0x000101D5));
  
  driver.rms_current(2000);
  driver.XTARGET(100000);
  driver.XACTUAL(0);

    Serial.print("CHOPCONF: ");
  Serial.println(sendData(0x6C, 0x000101D5));
  
  sendData(0x2D+0x80, 200000); //XTARGET:
  sendData(0x21+0x80, 0); // set XACTUAL to zero

    Serial.print("CHOPCONF: ");
  Serial.println(sendData(0x6C, 0x000101D5));  
}  
void loop()
{
driver.XTARGET(100000);
sendData(0xAD, 200000); //XTARGET:
Serial.print("driver.XTARGET(): ");
Serial.println(driver.XTARGET());
Serial.print("sendData(0x2D, 2): ");
Serial.println(sendData(0x2D, 2));

delay(5000);
driver.XTARGET(0);
sendData(0xAD, 0); //XTARGET:
Serial.print("driver.XTARGET(): ");
Serial.println(driver.XTARGET());
Serial.print("sendData(0x2D, 2): ");
Serial.println(sendData(0x2D, 2));
delay(5000);
}

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
