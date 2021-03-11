void IRAM_ATTR stalled_position(){
  Serial.println("Stalled");
  
  Serial.println(driver2.TSTEP());
  
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition());
  stalled_motor=true;
  stop_motor=true;
}

void IRAM_ATTR current_calibration(){
  Serial.println("Current Set");
  current_set=true;
}

void IRAM_ATTR digital_stop(){ //NOT USED
  Serial.println("Stalled");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;
}

void IRAM_ATTR button_stop(){ //NOT USED

 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 if (interrupt_time - last_interrupt_time > 50){
  
  Serial.println("Button Stop");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;
 }
}

void IRAM_ATTR sensor_short(){
  Serial.println("SENSOR 2");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;

  if(distance_cal==true){
  trip = trip + 1;
  }
}

void IRAM_ATTR sensor_long(){
  Serial.println("SENSOR 1");
  stepper.setAcceleration(200000);
  stepper.setMaxSpeed(0);
  delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition());
  stop_motor=true;
  
  if(distance_cal==true){
  trip = trip + 1;
  }
}




void setZero(){
      XACTUAL=0;
      stepper.setCurrentPosition(XACTUAL);
      preferences_local.putInt("XACTUAL", XACTUAL);
      Blynk.virtualWrite(V46, "CLOSED");
      Blynk.virtualWrite(V23, 0);
      Serial.print("XACTUAL: ");
      Serial.println(XACTUAL);
}
