#include "driver/ledc.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <Preferences.h>
#include "pins.h"
#include "Variables.h"
//#include "HTML.h"
#include "API.h"
#include "ResetButton.h"
#include "motor_control.h"
#include "motor_calibration.h"


TaskHandle_t TaskA;

void setup() {

  Serial.begin(115200);
  preferences.begin("local", false);
  pinMode(position_2_sensor, INPUT);
  pinMode(position_1_sensor, INPUT);

  xTaskCreatePinnedToCore(
   IndependentTask,        /* pvTaskCode */
   "Motor_Functions",          /* pcName */
   8192,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   &TaskA,                 /* pxCreatedTask */
   0);                     /* xCoreID */  

  wifi_set = preferences.getInt("wifi_set", 0);
  ssid = preferences.getString ("ssid", "NO_SSID");
  pass = preferences.getString ("pass", "NO_PASSWORD");
  
  open_stall_calibration_high = preferences.getInt("OStallCalHi", 4128768);
  open_stall_calibration_value_high = preferences.getInt("2StallCalValHi", 63);
  close_stall_calibration_high = preferences.getInt("CStallCalHi", 4128768);
  close_stall_calibration_value_high = preferences.getInt("1StallCalValHi", 63);

  open_stall_calibration_low = preferences.getInt("OStallCalLo", 4128768);
  open_stall_calibration_value_low = preferences.getInt("2StallCalValLo", 63);
  close_stall_calibration_low = preferences.getInt("CStallCalLo", 4128768);
  close_stall_calibration_value_low = preferences.getInt("1StallCalValLo", 63);

  open_current_calibration_high = preferences.getInt("O_cur_cal_h", 512);
  open_current_calibration_value_high = preferences.getInt("2_cur_cal_val_h", 4);
  close_current_calibration_high = preferences.getInt("C_cur_cal_h", 512);
  close_current_calibration_value_high = preferences.getInt("1_cur_cal_val_h", 4);

  open_current_calibration_low = preferences.getInt("O_cur_cal_val_l", 512);
  open_current_calibration_value_low = preferences.getInt("O_cur_cal_val_l", 4);
  close_current_calibration_low = preferences.getInt("C_cur_cal_val_l", 512);
  close_current_calibration_value_low = preferences.getInt("C_cur_cal_val_l", 4);

  stall_close_high = preferences.getInt("stall_close_hi", 4128768);
  Serial.print("Preferences stall_close: ");
  Serial.println(stall_close_high);
  
  stall_open_high = preferences.getInt("stall_open_hi", 4128768);
  Serial.print("Preferences stall_open: ");
  Serial.println(stall_open_high);

  stall_close_low = preferences.getInt("stall_close_lo", 4128768);
  Serial.print("Preferences stall_close: ");
  Serial.println(stall_close_low);
  
  stall_open_low = preferences.getInt("stall_open_lo", 4128768);
  Serial.print("Preferences stall_open: ");
  Serial.println(stall_open_low);
  
  open_current_high = preferences.getInt("open_current_h", current_setup);
  Serial.print("Preferences open_current: ");
  Serial.println(open_current_high);

  close_current_high = preferences.getInt("close_current_h", current_setup);
  Serial.print("Preferences close_current: ");
  Serial.println(close_current_high);

  open_current_low = preferences.getInt("open_current_l", current_setup);
  Serial.print("Preferences open_current: ");
  Serial.println(open_current_low);

  close_current_low = preferences.getInt("close_current_l", current_setup);
  Serial.print("Preferences close_current: ");
  Serial.println(close_current_low);

  move_to_position = preferences.getInt("move_to_position", 0); //3102236
  Serial.print("Preferences move_to_position: ");
  Serial.println(move_to_position);

  max_steps = preferences.getInt("max_steps", 0);
  Serial.print("Preferences max_steps: ");
  Serial.println(max_steps);

  XACTUAL = 0;
  Serial.print("XACTUAL Position: ");
  Serial.println(XACTUAL);

  CLOSE_POSITION = preferences.getInt("close_pos", 1);
  Serial.print("Preferences Close Position: ");
  Serial.println(CLOSE_POSITION);

  just_open_position = 1.5 * one_inch;
  Serial.print("just_open_position: ");
  Serial.println(just_open_position);

  fast_loud = preferences.getBool("fast_loud", true);

  clockout_setup();
  setup_motors();

}

long update_timer = millis();
int update_count = 0;
int last_dir; // last direction the motors moved


void IndependentTask( void * parameter ){  // Core for motor tasks
while(true) {

    // buttons
    // A press sets the command to open or close the track motor.
    
    if(digitalRead(btn1)==LOW){
      move_to_position = max_steps-100000;
      command = CUSTOM_MOVE; 
      Serial.println(command);
    }
    if(digitalRead(btn2)==LOW){
      move_to_position = 0;
      command = CUSTOM_MOVE;
      Serial.println(command);
    }
    if(command!=-1){
      Serial.print("Executing command ");
      Serial.println(command);

    }if(command==STOP){
      delay(100);
    }else if(command==CUSTOM_MOVE){
      motor_running = true;
      if(CLOSE_POSITION==1){
      move_motor_position_1();
      }else if(CLOSE_POSITION==2){
      move_motor_position_1(); //CHANGE BACK TO 2
      }
    }else if (command==POSITION_ADJUST){
      Serial.println("Starting POSITION_ADJUST");
      position_adjust();
    }else if (command==POSITION_CLOSE){
      Serial.println("Starting POSITION_CLOSE");
      position_close();
    }else if (command==POSITION_OPEN){
      Serial.println("Starting POSITION_OPEN");
      position_open();
    }else if (command==STEP_1){
      Serial.println("Step 1 Cal");
      new_auto_calibrate_step_1();
      position_close();
    }else if (command==STEP_3){
      new_auto_calibrate_step_3();
      position_close();
    }else if(command==STEP_4){
      new_auto_calibrate_step_4();
      position_close();
    }else if(command==STEP_5){
      new_auto_calibrate_step_5();
      position_close();
    }else if(command==STEP_6){
      new_auto_calibrate_step_6();
      position_close();
    }else if(command==AUTO_TUNE){
      new_auto_calibrate_step_1();
      new_auto_calibrate_step_3(); 
      new_auto_calibrate_step_4(); 
      new_auto_calibrate_step_5(); 
      new_auto_calibrate_step_6(); 
      position_close();
    } 

    if(command!=-1)Serial.println("[ready for next movement]");
    command = -1;
    delay(16);
  }
}


void loop() {

closeState = digitalRead(position_1_sensor);

  // compare the buttonState to its previous state
  if (closeState != lastcloseState) {
    // if the state has changed, increment the counter
    if (closeState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      Serial.println("DEVICE OPENED");
        }
    } else {
      // if the current state is LOW then the button went from on to off:
      XACTUAL=0;
      sendData(0x21+0x80, 0);      // XACTUAL=0
      sendData(0x2D+0x80, 0);      // XTARGET=0
      Serial.println("DEVICE CLOSED");
      move_close_stall=false;
      move_open_stall=false;

    }
  }

lastcloseState = closeState;


openState = digitalRead(position_2_sensor);

  // compare the state to its previous state
  if (openState != lastopenState) {
    // if the state has changed, increment the counter
    if (openState == LOW) {
      if (distance_cal==false){
      XACTUAL=max_steps;
      sendData(0x21+0x80, max_steps);      // XACTUAL=0
      sendData(0x2D+0x80, max_steps);      // XTARGET=0
      }
    }
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
    }else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("CLEARED STALL");
      if (closeState == HIGH) {
      //Blynk.virtualWrite(V46, "OPENED");
     }
    }
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
  }
  
  // save the current state as the last state, for next time through the loop
  lastStallOpenState = stallOpenState;

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
