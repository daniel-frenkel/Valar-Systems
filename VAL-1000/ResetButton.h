#define WIFI_PIN 4 

volatile bool     g_buttonPressed = false;
volatile uint32_t g_buttonPressTime = -1;



void button_action(void)
{
  //wifi_set = false;
  //preferences.putBool ("wifi_set", false);
  preferences.putInt ("wifi_set", 0);
  preferences.end();
  delay(1000);
  ESP.restart();
}

void button_change(void)
{

  bool buttonState = !digitalRead(WIFI_PIN);

  if (buttonState && !g_buttonPressed) {
    g_buttonPressTime = millis();
    g_buttonPressed = true;
    //Serial.println("Hold the button to reset configuration...");
  } else if (!buttonState && g_buttonPressed) {
    g_buttonPressed = false;
    uint32_t buttonHoldTime = millis() - g_buttonPressTime;
    if (buttonHoldTime >= 3000) {
    button_action();
    }
    g_buttonPressTime = -1;
  }
}
