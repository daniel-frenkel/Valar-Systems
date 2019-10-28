#include "Settings.h"
#include <BlynkSimpleEsp32.h>
#include "BlynkState.h"
#include "ConfigStore.h"
#include "ResetButton.h"
#include "ConfigMode.h"
#include "Indicator.h"
#include "OTA.h"

inline
void BlynkState::set(State m) {
  if (state != m) {
    DEBUG_PRINT(String(StateStr[state]) + " => " + StateStr[m]);
    state = m;
  }
}

long connect_fail_timer = -1;

class Provisioning {

public:
  void begin()
  {
    DEBUG_PRINT("");
    DEBUG_PRINT("Hardware v" + String(BOARD_HARDWARE_VERSION));
    DEBUG_PRINT("Firmware v" + String(BOARD_FIRMWARE_VERSION));

    indicator_init();
    button_init();
    config_init();

    if (configStore.flagConfig) {
      BlynkState::set(MODE_CONNECTING_NET);
    } else {
      BlynkState::set(MODE_WAIT_CONFIG);
    }
  }

  void run() {
    if(connect_fail_timer == -1||Blynk.connected()){
      connect_fail_timer = millis() + 60*60*1000L;
    }
    if(BlynkState::get()==MODE_RUNNING){
      if((WiFi.status() != WL_CONNECTED)) {
        delay(1000);
        BlynkState::set(MODE_CONNECTING_NET);
      }else if(!Blynk.connected()) {
        delay(1000);
        BlynkState::set(MODE_CONNECTING_CLOUD);
      }
    }

    if(connect_fail_timer<millis()){
      DEBUG_PRINT("Restarting after connection error 1-hour timeout.");
      delay(10);
    
      restartMCU();
    }
    
    switch (BlynkState::get()) {
    case MODE_WAIT_CONFIG:       
    case MODE_CONFIGURING:       enterConfigMode();    break;
    case MODE_CONNECTING_NET:    enterConnectNet();    break;
    case MODE_CONNECTING_CLOUD:  enterConnectCloud();  break;
    case MODE_RUNNING:           Blynk.run();          break;
    case MODE_OTA_UPGRADE:       enterOTA();           break;
    case MODE_SWITCH_TO_STA:     enterSwitchToSTA();   break;
    case MODE_RESET_CONFIG:      enterResetConfig();   break;
    default:                     enterError();         break;
    }
  }

};

Provisioning BlynkProvisioning;
