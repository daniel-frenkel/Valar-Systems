#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "AUTH";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SSID";
char pass[] = "PASSWORD";


BlynkTimer blynkTimer;



BLYNK_CONNECTED() {
  // Synchronize time on connection
  Blynk.syncAll();
}


BLYNK_WRITE(V1) {
    switch (param.asInt()){
      
    case 1: // Item 1 //Close
      Serial.print("Closing to position: ");
      stepper.moveTo(0);
      Serial.println(stepper.targetPosition());
      command = MOVE;
      break;

    case 2: // Item 4 //Open 100%
      Serial.print("Opening to position: ");
      stepper.moveTo(max_steps);
      Serial.println(stepper.targetPosition());
      command = MOVE;
      break;

    case 3: // Item 5 //"STOP"
      Serial.println("STOPPING");
      command = STOP;

      stepper.setAcceleration(200000);
      stepper.setMaxSpeed(0);
      stepper.moveTo(stepper.currentPosition());

      
      break;
      
  }
}

BLYNK_WRITE(V2) { // set position slider
  Serial.print("Move to: ");
  int q=(max_steps/100)*param.asInt();
  stepper.moveTo(q);
  Serial.print(q);
  command = MOVE;
}

BLYNK_WRITE(V22) { // set max distance
  Serial.print("set distance: ");
  float q=param.asFloat()*one_inch; 
  max_steps = (int)q;
  Serial.println(max_steps);
  preferences_local.putInt("max_steps", max_steps);
}

BLYNK_WRITE(V21) { // set acceleration
  Serial.print("set accel: ");
  int q=param.asFloat()*10;
  MOVE_ACCEL = q;
  Serial.println(MOVE_ACCEL);
  preferences_local.putInt("move_accel", MOVE_ACCEL);
  stepper.setAcceleration(MOVE_ACCEL);
}

BLYNK_WRITE(V34) { // set velocity
  Serial.print("set velocity: ");
  long q=param.asInt()*10;
  MOVE_VELOCITY = q;
  Serial.println(MOVE_VELOCITY);
  preferences_local.putInt("move_velocity", MOVE_VELOCITY);
  stepper.setMaxSpeed(MOVE_VELOCITY);
  
  TCOOLS = (3089838.00*pow(float(MOVE_VELOCITY),-1.00161534))*1.5;
  
  Blynk.virtualWrite(V125, TCOOLS/10);
  Serial.print("TCOOLS: ");
  Serial.println(TCOOLS);
}


//STALLGUARD
/*
A higher value gives a higher sensitivity. 
A higher value makes StallGuard4 more sensitive and requires less torque to indicate a stall.
*/
BLYNK_WRITE(V123) { // set stallguard OPEN value
  Serial.print("set OPEN stall: ");
  int q = param.asInt();
  if(q > 255) q = 255;
  if(q < 0) q = 0;
  stall_open = q;
  preferences_local.putInt("stall_open", stall_open); 
  Serial.println(stall_open);
}

BLYNK_WRITE(V124) { // set stallguard CLOSE value
  Serial.print("set CLOSE stall: ");
  int q=param.asInt();
  if(q>255)q=255;
  if(q<0)q=0;
  stall_close = q;
  preferences_local.putInt("stall_close", stall_close); 
  Serial.println(stall_close);
}

BLYNK_WRITE(V25) { // set Current OPEN value
  Serial.print("set open current: ");
  int q=param.asInt();
  current_open=q;
  preferences_local.putInt("current_open", current_open);
  driver.rms_current(current_open);
  Serial.println(current_open);
}


BLYNK_WRITE(V26) { // set Current CLOSE value
  Serial.print("set close current: ");
  int q=param.asInt();
  current_close=q;
  driver.rms_current(current_close);
  preferences_local.putInt("current_close", current_close);
  Serial.println(current_close);
}


BLYNK_WRITE(V36) { // Reverse Motor Turn
  if(param.asInt()==1){
    Serial.println("Reverse Turn ON ");
    CLOSE_POSITION = 1; // tell control loop what to do
    driver.shaft(true);
    preferences_local.putInt("close_pos", CLOSE_POSITION);
  }
  else{
    Serial.println("Reverse Turn OFF ");
    CLOSE_POSITION = 2;
    driver.shaft(false);
    preferences_local.putInt("close_pos", CLOSE_POSITION);
  }
}

BLYNK_WRITE(V50) { //Set Zero
  if(param.asInt()!=0){
      XACTUAL=0;
      stepper.setCurrentPosition(XACTUAL);
      preferences_local.putInt("XACTUAL", XACTUAL);
      Blynk.virtualWrite(V2, 0);
      Serial.print("XACTUAL: ");
      Serial.println(XACTUAL);

  }
}

/* set TCOOLS
*  This is the lower velocity threshold to turn on stallguard.
*  Stallguard is not reliable at low velocities. This turns it off until the TCOOLS value is reached.
*/

BLYNK_WRITE(V125) { 
  Serial.print("set TCOOLS: ");
  int q=param.asFloat(); 
  TCOOLS = q*10;
  Serial.println(TCOOLS);
  driver.TCOOLTHRS(TCOOLS); // 
  preferences_local.putInt("tcools", TCOOLS);
}
