#define LED1 33
#define LED2 18
#define BUTTON1 23
#define BUTTON2 34

int brightness0 = 0;    // how bright the LED is
int brightness1 = 0;    // how bright the LED is
int fade0Amount = 15;    // how many points to fade the LED by
int fade1Amount = 15; 

int btn1Press;
int btn2Press;


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
  ledcAttachPin(LED1, 1); // assign a led pins to a channel
  ledcAttachPin(LED2, 0); // assign a led pins to a channel

  ledcSetup(0, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(1, 5000, 8); // 12 kHz PWM, 8-bit resolution


  pinMode(BUTTON1,INPUT_PULLUP );
  pinMode(BUTTON2,INPUT_PULLUP );
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  attachInterrupt(BUTTON1, button1pressed, FALLING);
  attachInterrupt(BUTTON2, button2pressed, FALLING);

  ledcWrite(0, 0); // turn off LED 
  ledcWrite(1, 0); // turn off LED 

}


int button1Timer;
int button2Timer;
int waitButton1Timer;
int waitButton2Timer;
bool motorRunning;


void loop() {

  if(btn1Press == 1) {
    //Serial.println("BUTTON 1 PRESS");
    button1Timer = millis();
    waitButton2Timer = millis() + 1000;
    
    if (millis() < waitButton2Timer && btn2Press == 1 && motorRunning == false) {
      Serial.println("START MOTOR CLOSE");
      motorRunning = true;
      }
    
    if (brightness0 <= 255 && brightness0 >= 0) { 
      ledcWrite(0, brightness0); // set the brightness of the LED
      brightness0 = brightness0 + fade0Amount;
      vTaskDelay(30);
      }

    //Fade instead of turn off
    if (brightness0 > 255) {
      brightness0 = 255;
      fade0Amount = -fade0Amount;
      }

     if (brightness0 < 0){
      
      brightness0 = 0;
      ledcWrite(0, brightness0);
      btn1Press = 0;
      fade0Amount = 15;
      motorRunning = false;
     }
    }
    
  if (btn2Press == 1) {
    //Serial.println(" BUTTON 2 PRESS");
    button2Timer = millis();
    waitButton1Timer = millis() + 1000;
    
    if (millis() < waitButton1Timer && btn1Press == 1 && motorRunning == false ) {
      Serial.println("START MOTOR OPEN");
      motorRunning = true;
      }
      
    if (brightness1 <= 255 && brightness1 >= 0) { 
      ledcWrite(1, brightness1); // set the brightness of the LED
      brightness1 = brightness1 + fade1Amount;
      vTaskDelay(30);
      }

    //Fade instead of turn off
    if (brightness1 > 255) {
      brightness1 = 255;
      fade1Amount = -fade1Amount;
      }

     if (brightness1 < 0){
      
      brightness1 = 0;
      ledcWrite(1, brightness1);
      btn2Press = 0;
      fade1Amount = 15;
      motorRunning = false;
     }

  }
   
}
