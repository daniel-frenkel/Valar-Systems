/*
 * MODEL-S Curtain Opener
 * By Daniel Frenkel
 */

#define chipCS 27
#define SCLK 26
#define MISO_PIN 33
#define MOSI_PIN 25
#define CLOCKOUT 15
#define ENABLE_PIN 32
#define R_SENSE 0.075f

#define btn1 12//17//12
#define btn2 13//16//13
#define DIR_OPEN 1
#define DIR_CLOSE 2
#define MOVE_OPEN 5
#define MOVE_CLOSE 6

#include <TMCStepper.h>
#include "driver/ledc.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <Preferences.h>
#include "Memory.h"
#include "ResetButton.h"
#include "MotorControl.h"
#include "API.h"

void setup() {

  Serial.begin(115200);
  delay(1000);
  preferences.begin("local", false);
  
  load_preferences();
  setup_motors();
  clockout_setup();
  API();
  
  // Now set up tasks to run independently.
  xTaskCreatePinnedToCore(
    MotorTask //Motor Task
    ,  "MotorTask"   // A name just for humans
    ,  1024*4  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
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
