#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <Preferences.h>
#include "pins.h"
#include "Memory.h"
#include "MotorControl.h"

   // setting PWM properties
  const int freq = 12000000;
  const int ledChannel = 0;
  const int resolution = 2;
  
TaskHandle_t TaskA;

void setup() {

  Serial.begin(115200);
  delay(1000);
  preferences.begin("local", false);

  load_preferences();
  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(CLOCKOUT, ledChannel);
  ledcWrite(ledChannel, 1);
  
  setup_motors();

  sendData(0xA0, 0);     // Positioning mode 0
  sendData(0x26 + 0x80, 300);       //AMAX
  sendData(0x28 + 0x80, 800);       // DMAX
  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  //sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  sendData(0x27 + 0x80, max_speed);
  sendData(0x14 + 0x80, max_speed - 100);
  int q = 10;
  double current_calc =((q+1)/(float)32)*(0.325/(0.15+0.02))*(1/sqrt(2));
  Serial.print("Open Current: ");
  Serial.print(current_calc);
  Serial.println(" Amps");
  q&=0x1F;
  q=q<<8;
  current=q;
          
  sendData(0x6D + 0x80, stall);
  Serial.println(sendData(0x6D, 0));
  sendData(0x10 + 0x80, current);
  Serial.println(sendData(0x10, 0));
  sendData(0x2D + 0x80, 1000000); //XTARGET
  sendData(0x21 + 0x80, 0);
  
  digitalWrite(ENABLE_PIN, LOW);

  Serial.println("TEST 1");
}

void loop()
{
    sendData(0x2D + 0x80, 1000000); //XTARGET
  Serial.println(sendData(0x2D, 0));
  delay(5000);
  sendData(0x2D + 0x80, 0); //XTARGET
  Serial.println(sendData(0x2D, 0));
  delay(5000);

}
