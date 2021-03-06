include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include <Preferences.h>

#define STEP_PIN  13
#define DIR_PIN  14
#define ENABLE_PIN 27
#define RXD2 16
#define TXD2 17
#define STALLGUARD 2
#define BUTTON1 23
#define BUTTON2 34
#define SENSOR1 22
#define SENSOR2 32

#define CUSTOM_MOVE 1
#define STOP 2

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
TMC2209Stepper driver2(&Serial2, 0.12f , 0b00);
TaskHandle_t C0; //Dual Core Setup
Preferences preferences_local;

#include "variables.h"
#include "preferences.h"
#include "blynk.h"
#include "motor_control.h"

// STALLGUARD PIN STOP MOTOR
void IRAM_ATTR stalled_position(){ 
  //Serial.println("STALLED");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  stepper.moveTo(stepper.currentPosition());
}

void IRAM_ATTR sensor_1(){ // LIMIT SWITCH 1 STOP MOTOR
  //Serial.println("SENSOR 2");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;
}

void IRAM_ATTR sensor_2(){ // LIMIT SWITCH 2 STOP MOTOR
  //Serial.println("SENSOR 1");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;
}


void setup() {
  
  Serial.begin(115200);
  
  preferences_local.begin("local", false);
  
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STALLGUARD ,INPUT);
  pinMode(BUTTON1,INPUT);
  pinMode(BUTTON2,INPUT);
  pinMode(SENSOR1,INPUT);
  pinMode(SENSOR2,INPUT);

  //Core 0 Setup
  disableCore0WDT();
  xTaskCreatePinnedToCore(
   core0assignments,       // Function that should be called
   "Core_0",               // Name of the task (for debugging)
   8192,                   // Stack size (bytes)
   NULL,                   // Parameter to pass
   1,                      // Task priority
   &C0,                    // Task handle
   0);                     // Core you want to run the task on (0 or 1)

//DRIVER SETUP
  Serial2.begin(115200);
  driver2.begin();
  driver2.toff(4);
  driver2.blank_time(24);
  driver2.rms_current(current_close); 
  driver2.microsteps(64);
  driver2.TCOOLTHRS(TCOOLS);
  driver2.SGTHRS(stall_close);
  driver2.TPWMTHRS(0);
  driver2.semin(0);
  driver2.semax(2);
  driver2.sedn(0b00);
  driver2.en_spreadCycle(false);
  driver2.pdn_disable(true);

  //INTERRUPT SETUP
  attachInterrupt(STALLGUARD, stalled_position, RISING);
  attachInterrupt(SENSOR1, sensor_1, FALLING);
  attachInterrupt(SENSOR2, sensor_2, FALLING);

  //MOTOR SETUP
  stepper.setEnablePin(ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.setCurrentPosition(XACTUAL);
  stepper.disableOutputs();
  
  if(CLOSE_POSITION==1){
    driver2.shaft(true);
  }else{
    driver2.shaft(false);
  }
    
  Serial.println("SETUP COMPLETE");
}

void loop() { //MOTOR FUNCTIONS ONLY

// START BUTTONS
// A press sets the command to open or close the motor.
    
    if(digitalRead(BUTTON1)==LOW){
      stepper.moveTo(max_steps);
      command = CUSTOM_MOVE; 
      Serial.println(command);
    }
    
    if(digitalRead(BUTTON2)==LOW){
      stepper.moveTo(0);
      command = CUSTOM_MOVE;
      Serial.println(command);
    }
    
 //END BUTTTONS
 
    if(command==STOP){
      stepper.setAcceleration(200000);
      stepper.setMaxSpeed(0);
      stepper.moveTo(stepper.currentPosition());
      command = -1; //set back to -1 until next command call
      
    }else if(command==CUSTOM_MOVE){
      motor_running = true;
      move_motor();
      command = -1;
    }
}

void core0assignments( void * pvParameters ) { //WIFI CORE
  
  Blynk.begin(auth, ssid, pass);
    for (;;) {
      Blynk.run();
      blynkTimer.run();

      if(blynk_update == true){
      Blynk.virtualWrite(V2, ((float)XACTUAL/(float)max_steps)*100);
      blynk_update = false;  
      }
      
    }
}
