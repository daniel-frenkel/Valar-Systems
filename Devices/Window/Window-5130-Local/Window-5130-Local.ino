#include "driver/ledc.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <Preferences.h>
#include "pins.h"
#include "Variables.h"
#include "ResetButton.h"
#include "API.h"
#include "motor_control.h"
#include "motor_calibration.h"

TaskHandle_t TaskA;

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("VERSION: 2.0.0");
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


  xTaskCreatePinnedToCore(
    otherTask,        /* pvTaskCode */
    "Other_Functions",          /* pcName */
    1024 * 4,                 /* usStackDepth */
    NULL,                   /* pvParameters */
    1,                      /* uxPriority */
    NULL,                 /* pxCreatedTask */
    1);                     /* xCoreID */

}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved

void loop()
{
  // Empty. Things are done in Tasks.

}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void motorTask( void * parameter ) { // Core for motor tasks
  while (true) {

    // buttons
    // A press sets the command to open or close the track motor.

    if (digitalRead(btn1) == LOW) {
      Serial.println("btn1 Pressed");
      move_to_steps = max_steps - 100000;
      command = CUSTOM_MOVE;
      Serial.println(command);
    }
    if (digitalRead(btn2) == LOW) {
      Serial.println("btn2 Pressed");
      move_to_steps = 0;
      command = CUSTOM_MOVE;
      Serial.println(command);
    }
    if (command == STOP) {
      delay(100);
      command = -1;
    } else if (command == CUSTOM_MOVE) {
      Serial.println("CUSTOM MOVE");
      motor_running = true;
      if (CLOSE_POSITION == 1) {
        move_motor_position_1();
      } else if (CLOSE_POSITION == 2) {
        move_motor_position_1(); //CHANGE BACK TO 2
      }
      command = -1;
    } else if (command == POSITION_ADJUST) {
      Serial.println("Starting POSITION_ADJUST");
      position_adjust();
      command = -1;
    } else if (command == POSITION_CLOSE) {
      Serial.println("Starting POSITION_CLOSE");
      position_close();
      command = -1;
    } else if (command == POSITION_OPEN) {
      Serial.println("Starting POSITION_OPEN");
      position_open();
      command = -1;
    } else if (command == STEP_1) {
      Serial.println("Step 1 Cal");
      new_auto_calibrate_step_1();
      position_close();
      command = -1;
    } else if (command == STEP_3) {
      Serial.println("Step 3 Cal");
      new_auto_calibrate_step_3();
      position_close();
      command = -1;
    } else if (command == STEP_4) {
      Serial.println("Step 4 Cal");
      new_auto_calibrate_step_4();
      position_close();
      command = -1;
    } else if (command == STEP_5) {
      Serial.println("Step 5 Cal");
      new_auto_calibrate_step_5();
      position_close();
      command = -1;
    } else if (command == STEP_6) {
      Serial.println("Step 6 Cal");
      new_auto_calibrate_step_6();
      position_close();
      command = -1;
    } else if (command == AUTO_TUNE) {
      Serial.println("Starting Auto Tune");
      new_auto_calibrate_step_1();
      new_auto_calibrate_step_3();
      new_auto_calibrate_step_4();
      new_auto_calibrate_step_5();
      new_auto_calibrate_step_6();
      position_close();
      command = -1;
    } else {
      vTaskDelay(1);
    }
  }
}


void otherTask(void *pvParameters) {
  while (true) {

    //Serial.println(digitalRead(ENABLE_PIN));

    closeState = digitalRead(position_1_sensor);

    // compare the buttonState to its previous state
    if (closeState != lastcloseState)
    {
      if (closeState == HIGH)
      {
        Serial.println("DEVICE OPENED");
        device_status = "OPENED";
      }
      else
      {
        Serial.println("DEVICE CLOSED");
        device_status = "CLOSED";
        XACTUAL = 0;
        sendData(0x21 + 0x80, 0);    // XACTUAL=0
        sendData(0x2D + 0x80, 0);    // XTARGET=0
        move_close_stall = false;
        move_open_stall = false;
      }
    } else
    {
      vTaskDelay(1);
    }

    lastcloseState = closeState;

    openState = digitalRead(position_2_sensor);

    // compare the state to its previous state
    if (openState != lastopenState) {
      // if the state has changed, increment the counter
      if (openState == LOW) {
        if (distance_cal == false) {
          XACTUAL = max_steps;
          sendData(0x21 + 0x80, max_steps);    // XACTUAL=0
          sendData(0x2D + 0x80, max_steps);    // XTARGET=0
        }
      }
    } else
    {
      vTaskDelay(1);
    }


    // save the current state as the last state, for next time through the loop
    lastopenState = openState;

    // read the pushbutton input pin:
    stallCloseState = move_close_stall;

    // compare the buttonState to its previous state
    if (stallCloseState != lastStallCloseState) {
      // if the state has changed, increment the counter
      if (stallCloseState == true) {
        // if the current state is HIGH then the button went from off to on:
        //Blynk.virtualWrite(V46, "STALLED");
        Serial.println("STALLED CLOSED");
      } else {
        // if the current state is LOW then the button went from on to off:
        Serial.println("CLEARED STALL");
        if (closeState == HIGH) {
          //Blynk.virtualWrite(V46, "OPENED");
        }
      }
    } else
    {
      vTaskDelay(1);
    }

    // save the current state as the last state, for next time through the loop
    lastStallCloseState = stallCloseState;

    // read the pushbutton input pin:
    stallOpenState = move_open_stall;

    // compare the buttonState to its previous state
    if (stallOpenState != lastStallOpenState) {
      // if the state has changed, increment the counter
      if (stallOpenState == true) {
        // if the current state is HIGH then the button went from off to on:
        Serial.println("STALLED OPEN");
      } else {
        // if the current state is LOW then the button went from on to off:
        Serial.println("STALL CLEARED");
        if (closeState == HIGH) {
          //Blynk.virtualWrite(V46, "OPENED");
        }
      }
    } else
    {
      vTaskDelay(1);
    }

    // save the current state as the last state, for next time through the loop
    lastStallOpenState = stallOpenState;



  }
}

void clockout_setup() {
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
