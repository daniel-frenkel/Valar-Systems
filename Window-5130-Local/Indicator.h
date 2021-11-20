void indicator_run();

#if !defined(BOARD_LED_BRIGHTNESS)
#define BOARD_LED_BRIGHTNESS 255
#endif

#define DIMM(x)    ((x)*(BOARD_LED_BRIGHTNESS)/255)
#define RGB(r,g,b) (DIMM(r) << 16 | DIMM(g) << 8 | DIMM(b) << 0)

class Indicator {
public:

  enum Colors {
    COLOR_BLACK   = RGB(0x00, 0x00, 0x00),
    COLOR_WHITE   = RGB(0xFF, 0xFF, 0xE7),
    COLOR_BLUE    = RGB(0x0D, 0x36, 0xFF),
    COLOR_BLYNK   = RGB(0x2E, 0xFF, 0xB9),
    COLOR_RED     = RGB(0xFF, 0x10, 0x08),
    COLOR_MAGENTA = RGB(0xA7, 0x00, 0xFF),
  };

  Indicator() {
    m_Counter = 0;
    initLED();
  }

  uint32_t run() {
    State currState = BlynkState::get();

    // Reset counter if indicator state changes
    if (m_PrevState != currState) {
      m_PrevState = currState;
      m_Counter = 0;
    }

    if (g_buttonPressed) {
      if (millis() - g_buttonPressTime > BUTTON_HOLD_TIME_ACTION)     { return beatLED(COLOR_WHITE,   (int[]){ 100, 100 }); }
      if (millis() - g_buttonPressTime > BUTTON_HOLD_TIME_INDICATION) { return waveLED(COLOR_WHITE,   1000); }
    }
    switch (currState) {
    case MODE_RESET_CONFIG:
    case MODE_WAIT_CONFIG:       return beatLED(COLOR_BLUE,    (int[]){ 50, 500 });
    case MODE_CONFIGURING:       return beatLED(COLOR_BLUE,    (int[]){ 200, 200 });
    case MODE_CONNECTING_NET:    return beatLED(COLOR_BLYNK,   (int[]){ 50, 500 });
    case MODE_CONNECTING_CLOUD:  return beatLED(COLOR_BLYNK,   (int[]){ 100, 100 });
    //case MODE_RUNNING:           return waveLED(COLOR_BLYNK,   5000); //Original
    //case MODE_RUNNING:           digitalWrite(BOARD_LED_PIN, LOW);
    case MODE_RUNNING:           return offLED(COLOR_BLYNK,   5000);
    case MODE_OTA_UPGRADE:       return beatLED(COLOR_MAGENTA, (int[]){ 50, 50 });
    default:                     return beatLED(COLOR_RED,     (int[]){ 80, 100, 80, 1000 } );
    }
  }

protected:

  /*
   * LED drivers
   */

// Single color LED

  void initLED() {
    ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_BITS);
    ledcAttachPin(BOARD_LED_PIN, LEDC_CHANNEL_1);
  }

  void setLED(uint32_t color) {
    #if BOARD_LED_INVERSE
    ledcWrite(LEDC_CHANNEL_1, BOARD_PWM_MAX - color);
    #else
    ledcWrite(LEDC_CHANNEL_1, color);
    #endif
  }


  /*
   * Animations
   */

  uint32_t skipLED() {
    return 20;
  }


  template<typename T>
  uint32_t beatLED(uint32_t, const T& beat) {
    const uint8_t cnt = sizeof(beat)/sizeof(beat[0]);
    setLED((m_Counter % 2 == 0) ? BOARD_PWM_MAX : 0);
    uint32_t next = beat[m_Counter % cnt];
    m_Counter = (m_Counter+1) % cnt;
    return next;
  }

  uint32_t waveLED(uint32_t, unsigned breathePeriod) {
    uint8_t brightness = (m_Counter < 128) ? m_Counter : 255 - m_Counter;

    setLED(BOARD_PWM_MAX * ((float)brightness / (BOARD_PWM_MAX/2)));

    // This function relies on the 8-bit, unsigned m_Counter rolling over.
    m_Counter = (m_Counter+1) % 256;
    return breathePeriod / 256;
  }
  

    uint32_t offLED(uint32_t, unsigned breathePeriod) {
    uint8_t brightness = 0;

    setLED(BOARD_PWM_MAX * ((float)brightness / (BOARD_PWM_MAX/2)));

    // This function relies on the 8-bit, unsigned m_Counter rolling over.
    m_Counter = (m_Counter+1) % 256;
    return breathePeriod / 256;
  }


private:
  uint8_t m_Counter;
  State   m_PrevState;
};

Indicator indicator;

/*
 * Animation timers
 */

  #include <Ticker.h>

  Ticker blinker;

  void indicator_run() {
    uint32_t returnTime = indicator.run();
    if (returnTime) {
      blinker.attach_ms(returnTime, indicator_run);
    }
  }

  void indicator_init() {
    blinker.attach_ms(100, indicator_run);
  }
