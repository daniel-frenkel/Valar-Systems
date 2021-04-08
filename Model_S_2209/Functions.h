void IRAM_ATTR stalled_position()
{
  Serial.println("Stalled INTERRUPT");
  //stepper.setAcceleration(200000);
  //stepper.moveTo(stepper.currentPosition());
  stalled_motor = true;
  stop_motor = true;
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

void IRAM_ATTR sensor_short()
{
  sensor2_trip = true;
}

void IRAM_ATTR sensor_long()
{
  sensor1_trip = true;
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
