#define LED1 19
#define LED2 22
#define BUTTON1 4
#define BUTTON2 2

int brightness0 = 0;    // how bright the LED is
int brightness1 = 0;    // how bright the LED is
int brightness2 = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

int btn1Press;
int btn2Press;
int allowButtons;

int allowButtonTime;

void IRAM_ATTR button1pressed() 
{
    btn1Press = 1;
}

void IRAM_ATTR button2pressed() 
{
    btn2Press = 1;
}

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(115200);
  ledcAttachPin(LED1, 0); // assign a led pins to a channel
  ledcAttachPin(LED2, 1); // assign a led pins to a channel
  ledcAttachPin(LED1, 2); // assign a led pins to a channel
  ledcAttachPin(LED2, 2); // assign a led pins to a channel
  
  ledcSetup(0, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(1, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 5000, 8); // 12 kHz PWM, 8-bit resolution

  pinMode(BUTTON1,INPUT_PULLUP );
  pinMode(BUTTON2,INPUT_PULLUP );
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  attachInterrupt(BUTTON1, button1pressed, FALLING);
  attachInterrupt(BUTTON2, button2pressed, FALLING);
  
}

void led1On() {
    
    while (brightness0 < 255) { //Blocks
      ledcWrite(0, brightness0); // set the brightness of the LED
      brightness0 = brightness0 + fadeAmount;
      vTaskDelay(30);
    }
    brightness0 = 0;
    ledcWrite(0, brightness0); // set the brightness of the LED
}

void led2On() {
    
    while (brightness1 < 255) { //Blocks
      ledcWrite(1, brightness1); // set the brightness of the LED
      brightness1 = brightness1 + fadeAmount;
      vTaskDelay(30);
    }
    brightness1 = 0;
    ledcWrite(1, brightness1); // set the brightness of the LED
}

void ledBothOn() {
    Serial.println("Turn both LEDS on");
    brightness2 = 0;
    
    while (brightness2 < 255) { //Blocks
      ledcWrite(2, brightness2); // set the brightness of the LED
      brightness2 = brightness2 + fadeAmount;
      vTaskDelay(30);
    }
    

}

void loop() {

  Serial.println(digitalRead(BUTTON2));
  
  if(btn1Press == 1) {
    Serial.println("BUTTON 1 PRESS");
    led1On();
    int timeMax1 = millis() + 1000;
    while (millis() < timeMax1){ 
      if(btn2Press == 1){
             Serial.println("CLOSE DEVICE");
             ledBothOn();
             
             }
    }
    btn1Press = 0;
    btn2Press = 0;
  }

  /*
  if (btn2Press == 1) {
    
    Serial.println("START BUTTON 2 PRESS");
    int timeMax2 = millis() + 1000;
    
    while (millis() < timeMax2){   
      if(btn1Press == 1){
             Serial.println("OPEN DEVICE");
             ledBothOn();
    
             } 
     }
     
     btn1Press = 0;
     btn2Press = 0;
     Serial.println("END BUTTON 2 PRESS");
   }

  */
    
}
