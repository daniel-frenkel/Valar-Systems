/*
 * MODEL-S TMC5130 Curtain Opener
 * By Daniel Frenkel
 * Code has been deprecated and will never be updated
 * This is all extermely confusing
 * Don't even try to figure it out
 * Sorry
 */

#include "driver/ledc.h"
#include <Arduino.h>
#include <ezTime.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <Preferences.h>
#include "pins.h"
#include "Memory.h"
#include "API.h"
#include "ResetButton.h"
#include "MotorControl.h"

TaskHandle_t TaskA;

void setup() {

  Serial.begin(115200);
  delay(1000);
  preferences.begin("local", false);

  xTaskCreatePinnedToCore(
   motorTask,        /* pvTaskCode */
   "Motor_Functions",          /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   3,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   0);                     /* xCoreID */ 

  load_preferences();
  clockout_setup();
  setup_motors();
  API();

  // EZ Time
  if (WiFi.status() == WL_CONNECTED)
  {
  vTaskDelay(2000);
  setDebug(INFO);
  waitForSync();

  Serial.println();
  Serial.println("UTC: " + UTC.dateTime());

  myTZ.setLocation(MYTIMEZONE);
  Serial.print("Time in your set timezone: ");
  Serial.println(myTZ.dateTime());

  }
  
xTaskCreatePinnedToCore(
   otherTask,        /* pvTaskCode */
   "Other_Functions",          /* pcName */
   1024*4,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   NULL,                 /* pxCreatedTask */
   1);                     /* xCoreID */ 

   
}

void loop()
{
  // Empty. Things are done in Tasks.

}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void otherTask(void *pvParameters){
while(true) {

  if (WiFi.status() == WL_CONNECTED)
  { 
  events();
  }
  else {
  vTaskDelay(1);
  }
  
 }
}

void motorTask(void *pvParameters){
while(true) {

    if(digitalRead(btn1)==LOW){
      Serial.println("btn1 Pressed");
      move_to = max_steps;
      run_motor = true;
    }
    
    if(digitalRead(btn2)==LOW){
      Serial.println("btn2 Pressed");
      move_to = 0;
      run_motor = true;
    }
    
    if(run_motor == true)
    {
      Serial.println("Run Motor Function");
      move_motor();
      run_motor = false;
      Serial.println("Motor Complete");
    }
    else if(set_zero == 1)
    {
      setZero();
      set_zero = 0;
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

void clockout_setup(){
  periph_module_enable(PERIPH_LEDC_MODULE);
  uint32_t bit_width = 2; // 1 - 20 bits
  uint32_t divider = 320; // Q10.8 fixed point number, 0x100 — 0x3FFFF
  uint32_t duty_cycle = 1 << (bit_width - 1);
  float freq_mhz = ((uint64_t) LEDC_APB_CLK_HZ << 8) / (double) divider / (1 << bit_width) / 1000000.0;
  printf("\nfrequency: %f MHz\n", freq_mhz);
  ledc_timer_set(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, divider, bit_width, LEDC_APB_CLK);
  ledc_timer_rst(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
  ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
  ledc_channel_config_t pwm_pin_cfg = {
  CLOCKOUT, // chosen GPIO output for clock
  LEDC_HIGH_SPEED_MODE, // speed mode
  LEDC_CHANNEL_0, // ledc channel
  LEDC_INTR_DISABLE, // interrupt type
  LEDC_TIMER_0, // timer select
  duty_cycle // duty cycle
  };
  ledc_channel_config(&pwm_pin_cfg);
}
