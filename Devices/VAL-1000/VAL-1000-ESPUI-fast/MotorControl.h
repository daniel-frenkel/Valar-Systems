uint16_t positionLabel;

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define STEP_PIN  13
#define DIR_PIN  14
#define ENABLE_PIN 27
#define BUTTON1 23
#define BUTTON2 34
#define RXD2 16
#define TXD2 17
#define STALLGUARD 2 
#define SENSOR1 32
#define SENSOR2 22

#define SERIAL_PORT_2    Serial2    // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE          0.10f      // R_SENSE for current calc.  


FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;
TMC2209Stepper driver(&SERIAL_PORT_2, R_SENSE , DRIVER_ADDRESS);

void feedTheDog()
{
  // feed dog 0
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed = 1;                       // feed dog
  TIMERG0.wdt_wprotect = 0;                   // write protect
}

void IRAM_ATTR button1pressed() 
{
    move_to_step = 0;
    run_motor = true;
}

void IRAM_ATTR button2pressed() 
{
    move_to_step = max_steps;
    run_motor = true;
}

void IRAM_ATTR stalled_position()
{
  stalled_motor = true;
  stop_motor = true;
}

void IRAM_ATTR sensor_short()
{
  sensor2_trip = true;
}

void IRAM_ATTR sensor_long()
{
  sensor1_trip = true;
}

void IRAM_ATTR wifi_button_press()
{
  wifi_button = true;
}

void setZero()
{
      current_position=0;
      stepper->setCurrentPosition(0);
      Serial.print("current_position: ");
      Serial.println(current_position);
}

void goHome()
{
      current_position = 0;
      move_to_step = -10000;
      run_motor = true;
}

void move_motor() {
  Serial.print("Current Position: ");
  Serial.println(current_position);
  
  Serial.print("Moving to Position: ");
  Serial.println(move_to_step);

  stepper->setCurrentPosition(current_position);
    
  stalled_motor = false;
  sensor1_trip = false;
  sensor2_trip = false;
  
  stepper->setAcceleration(accel);;
  stepper->setSpeedInHz(max_speed);
  driver.rms_current(current);
  driver.SGTHRS(stall);
  driver.TCOOLTHRS(tcools);

if(current_position == move_to_step)
{
  Serial.println("ALREADY THERE!");
}
else if(move_to_step > current_position) // Open
{

      Serial.println("Opening");
      stepper->moveTo(move_to_step); 
    
      while (stepper->getCurrentPosition() != stepper->targetPos()) 
      {

      if (stalled_motor == true)
      {
        printf("Stalled\n");
        stepper->forceStop();
        break;
        //stepper->setCurrentPosition(move_to_step);
        //stepper->moveTo(move_to_step);
      }

      vTaskDelay(1);
      
      }

}

else if(move_to_step < current_position) //close
{
  
      Serial.println("Closing");
      stepper->moveTo(move_to_step);
      //stepper.enableOutputs();
    
      while (stepper->getCurrentPosition() != stepper->targetPos())  
      {
                
          if (stalled_motor == true) //we assume it's in the center
          {
            printf("Stalled\n");
            stepper->forceStop();
            break;
            stepper->setCurrentPosition(0);
            stepper->moveTo(0);
          }
      vTaskDelay(1);
      }
}else
{
  Serial.println("DO NOTHING!");
}
      current_position = stepper->getCurrentPosition();
      //stepper.disableOutputs();
      printf("Motor Function Complete\n");
}

// put your setup code here, to run once:
void setup_motors(){ 
  
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(STALLGUARD ,INPUT);
  pinMode(WIFI_PIN ,INPUT);
  pinMode(BUTTON1,INPUT);
  pinMode(BUTTON2,INPUT);
  pinMode(SENSOR1,INPUT);
  pinMode(SENSOR2,INPUT);
  SERIAL_PORT_2.begin(115200);
  
  driver.begin();
  driver.toff(4);
  driver.blank_time(24);
  driver.I_scale_analog(false);
  driver.internal_Rsense(false);
  driver.mstep_reg_select(true);
  driver.rms_current(current); 
  driver.SGTHRS(stall);
  driver.microsteps(0);
  driver.TCOOLTHRS(tcools); // 
  driver.TPWMTHRS(0);
  driver.semin(0);
  
  if (open_direction == 1)
  {
    driver.shaft(true);  
  }else{
    driver.shaft(false);  
  }
  
  driver.en_spreadCycle(false);
  driver.pdn_disable(true);

  engine.init();
  stepper = engine.stepperConnectToPin(STEP_PIN);
  
  stepper->setDirectionPin(DIR_PIN);
  stepper->setEnablePin(ENABLE_PIN);
  stepper->setAutoEnable(true);

    
  //stepper.setEnablePin(ENABLE_PIN);
  //stepper.setPinsInverted(false, false, true);
  //stepper.disableOutputs();
  
  attachInterrupt(STALLGUARD, stalled_position, RISING);
  attachInterrupt(WIFI_PIN, wifi_button_press, FALLING);
  attachInterrupt(BUTTON1, button1pressed, FALLING);
  attachInterrupt(BUTTON2, button2pressed, FALLING);
  attachInterrupt(SENSOR1, sensor_long, FALLING);
  attachInterrupt(SENSOR2, sensor_short, FALLING);

}
