void IRAM_ATTR button_stop()
{ //btn2  
  stepper.setAcceleration(20000);
  //stepper.setMaxSpeed(0);
  //delayMicroseconds(100000);
  stepper.moveTo(stepper.currentPosition()+20000);
}


void IRAM_ATTR button_start()
{ //btn1
  if(motor_running == false){
    stepper.setCurrentPosition(0);
    move_to_position = max_steps*100;
    command = CUSTOM_MOVE;
  }
}


void setZero()
{
      XACTUAL=0;
      stepper.setCurrentPosition(XACTUAL);
      preferences_local.putInt("XACTUAL", XACTUAL);
      Blynk.virtualWrite(V46, "CLOSED");
      Blynk.virtualWrite(V23, 0);
      Serial.print("XACTUAL: ");
      Serial.println(XACTUAL);
}
