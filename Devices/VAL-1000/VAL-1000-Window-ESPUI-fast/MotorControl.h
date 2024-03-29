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

#define SHAFT true

#define SERIAL_PORT_2    Serial2    // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE          0.10f      // E_SENSE for current calc.  


//AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
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


void move_motor() {
  Serial.print("Current Position: ");
  Serial.println(current_position);
  
  Serial.print("Moving to Position: ");
  Serial.println(move_to_step);

  stepper->setCurrentPosition(current_position);
  //stepper->moveTo(move_to_step);
  
  stalled_motor = false;
  sensor1_trip = false;
  sensor2_trip = false;
  
  stepper->setAcceleration(accel);
  stepper->setSpeedInHz(max_speed);
  driver.rms_current(current);
  driver.SGTHRS(stall);
  driver.TCOOLTHRS(tcools);

if(move_to_step == 0)
{
  
  Serial.println("Closing until Trip");
  stepper->moveTo(-10000);

      while (stepper->getCurrentPosition() != stepper->targetPos()) 
      {
          if (sensor1_trip == true || (digitalRead(SENSOR1) == LOW)) 
          {
            printf("TRIPPED ON 1\n");
            stepper->forceStop(); // Stop as fast as possible: sets new target
            stepper->setCurrentPosition(0);
            stepper->moveTo(0);
          }
          
          if (digitalRead(STALLGUARD) == HIGH)
          {
            printf("Stalled\n");
            stepper->forceStop(); // Stop as fast as possible: sets new target
            stepper->moveTo(stepper->getCurrentPosition());
          }

      vTaskDelay(1);
      //stepper.run();
      //feedTheDog();

      }
  }
  else if(move_to_step > current_position) // Open
  {

      Serial.println("Opening");
      stepper->moveTo(move_to_step);
      //stepper.enableOutputs();
    
      while (stepper->getCurrentPosition() != stepper->targetPos()) {

      if (sensor2_trip == true || digitalRead(SENSOR2) == LOW)
      {
        printf("TRIPPED ON 2\n");
        stepper->forceStop(); // Stop as fast as possible: sets new target
        stepper->setCurrentPosition(max_steps);
        stepper->moveTo(max_steps);
      }
      
      if (stalled_motor == true)
      {
        printf("Stalled\n");
        stepper->forceStop(); // Stop as fast as possible: sets new target
        stepper->moveTo(stepper->getCurrentPosition());
      }

      vTaskDelay(1);
      //stepper.run();
      //feedTheDog();
      }
  }
  else if(move_to_step < current_position)
  {
  
      Serial.println("Closing");
      stepper->moveTo(move_to_step);
      //stepper.enableOutputs();
    
      while (stepper->getCurrentPosition() != stepper->targetPos()) 
      {
        
          if (sensor1_trip == true ||(digitalRead(SENSOR1) == LOW)) //(sensor1_trip == true) || 
          {
            printf("TRIPPED ON 1\n");
            stepper->forceStop(); // Stop as fast as possible: sets new target
            stepper->setCurrentPosition(0);
            stepper->moveTo(0);
          }
          
          if (stalled_motor == true)
          {
            printf("Stalled\n");
            stepper->forceStop(); // Stop as fast as possible: sets new target
            stepper->moveTo(stepper->getCurrentPosition());
          }

      vTaskDelay(1);
      //stepper.run();
      //feedTheDog();

      }
}
else if(current_position == move_to_step)
{
  Serial.println("ALREADY THERE!");
}
else
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
  driver.shaft(SHAFT);  
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
