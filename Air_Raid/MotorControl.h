void move_motor() {

  Serial.println(CS_Check);

   
  Serial.print("Moving to Position: ");
  Serial.println(move_to_position);
  
  motor_running = true;         // mark that the track motor is running
  stop_motor = false;
  
  stepper.setAcceleration(MOVE_ACCEL);
  stepper.setMaxSpeed(MOVE_VELOCITY);
  stepper.setSpeed(MOVE_VELOCITY);
  
  stepper.enableOutputs();
  driver2.rms_current(current_open);
  stepper.setCurrentPosition(0); 
  stepper.moveTo(100000); 
  

    
  while ((stepper.speed() < MOVE_VELOCITY) && stop_motor == false) { // wait for position_reached flag 
  stepper.run();
  }

  int run_time = millis() + 2000;
  
  while (millis() < run_time && stop_motor == false) { // wait for position_reached flag 
  stepper.runSpeed();
  }
  
  stepper.disableOutputs();
  motor_running = false;   
  
}




/*  ====================================
 * 
 *    BEGIN MOTOR DRIVER BACKEND
 * 
 *  ====================================
 */



// put your setup code here, to run once:
void setup_motors(){ 
  
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STALLGUARD ,INPUT);
  SERIAL_PORT_2.begin(115200);

  driver2.begin();
  driver2.toff(4);
  driver2.blank_time(24);
  driver2.microsteps(motor_microsteps);
  driver2.TCOOLTHRS(300); // 
  driver2.TPWMTHRS(0);
  driver2.semin(0);
  driver2.semax(2);
  driver2.sedn(0b00);
  
  driver2.en_spreadCycle(true);
  driver2.pdn_disable(true);

  stepper.setEnablePin(ENABLE_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.disableOutputs();

  attachInterrupt(btn1, button_start, FALLING);
  attachInterrupt(btn2, button_stop, FALLING);
  
  
  if(CLOSE_POSITION==1){
    driver2.shaft(true);
  }else{
    driver2.shaft(false);
    }
}
