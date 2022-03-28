#include <Arduino.h>
#include <ESPUI.h>
#include <ezTime.h>
//#include <AccelStepper.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include <Preferences.h>
#include "Memory.h"
#include "ResetButton.h"
#include "MotorControl.h"
#include "API.h"
#include "calibration.h"
#include "ESPUI.h"

// the setup function runs once when you press reset or power the board
void setup() {
  
  Serial.begin(115200);
  delay(1000);
  preferences.begin("local", false);
  
  load_preferences();
  setup_motors();
  API();
  ESPUIsetup();

  ledcAttachPin(LED1, 1); // assign a led pins to a channel
  ledcAttachPin(LED2, 0); // assign a led pins to a channel

  ledcSetup(0, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(1, 5000, 8); // 12 kHz PWM, 8-bit resolution

  ledcWrite(0, 0); // turn off LED 
  ledcWrite(1, 0); // turn off LED 



  // Now set up tasks to run independently.
  xTaskCreatePinnedToCore(
    OtherTask //Motor Task
    ,  "OtherTask"   // A name just for humans
    ,  1024*4  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  1);

  xTaskCreatePinnedToCore(
    MotorTask //Motor Task
    ,  "MotorTask"   // A name just for humans
    ,  1024*4  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  0);




}

void loop()
{
  // Empty. Things are done in Tasks.

}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void OtherTask(void *pvParameters)  // Motor Task
{
  (void) pvParameters;

  for (;;)
  {

    if(btn1Press == 1) {
    //Serial.println("BUTTON 1 PRESS");
    button1Timer = millis();
    waitButton2Timer = millis() + 1000;
    
    if (millis() < waitButton2Timer && btn2Press == 1 && motorRunning == false) {
      Serial.println("START MOTOR CLOSE");
      motorRunning = true;
      move_to_step = -10000;
      run_motor = true;
      if(motor_running == true) stepper->moveTo(move_to_step);
      }
    
    if (brightness0 <= 255 && brightness0 >= 0) { 
      ledcWrite(0, brightness0); // set the brightness of the LED
      brightness0 = brightness0 + fade0Amount;
      vTaskDelay(30);
      }

    //Fade instead of turn off
    if (brightness0 > 255) {
      brightness0 = 255;
      fade0Amount = -fade0Amount;
      }

     if (brightness0 < 0){
      
      brightness0 = 0;
      ledcWrite(0, brightness0);
      btn1Press = 0;
      fade0Amount = 15;
      motorRunning = false;
     }
    }
    
  if (btn2Press == 1) {
    //Serial.println(" BUTTON 2 PRESS");
    button2Timer = millis();
    waitButton1Timer = millis() + 1000;
    
    if (millis() < waitButton1Timer && btn1Press == 1 && motorRunning == false ) {
      Serial.println("START MOTOR OPEN");
      motorRunning = true;
      move_to_step = max_steps;
      run_motor = true;
      if(motor_running == true) stepper->moveTo(move_to_step);
      }
      
    if (brightness1 <= 255 && brightness1 >= 0) { 
      ledcWrite(1, brightness1); // set the brightness of the LED
      brightness1 = brightness1 + fade1Amount;
      vTaskDelay(30);
      }

    //Fade instead of turn off
    if (brightness1 > 255) {
      brightness1 = 255;
      fade1Amount = -fade1Amount;
      }

     if (brightness1 < 0){
      
      brightness1 = 0;
      ledcWrite(1, brightness1);
      btn2Press = 0;
      fade1Amount = 15;
      motorRunning = false;
     }  
  }

  else
    {
      vTaskDelay(1);
    }
 }
}

void MotorTask(void *pvParameters)  // Motor Task
{
  (void) pvParameters;

  for (;;)
  {
    if(run_motor == true)
    {
      Serial.println("Run Motor Function");
      move_motor();
      run_motor = false;
      ESPUI.updateLabel(positionLabel, String(int(((float)current_position/(float)max_steps)*100)));
      Serial.println("Motor Complete");
    }
    else if(calibrate == 1)
    {
      TravelDistance();
      CalibrateCurrent();
      ESPUI.updateNumber(currentMax, current);
      CalibrateStall();
      ESPUI.updateNumber(stallMax, stall);
      calibrate = 0;
    }
    else if(wifi_button == true)
    {
      button_change();
      wifi_button = false;
    }
    else
    {
      vTaskDelay(1);
    }
  }
}
