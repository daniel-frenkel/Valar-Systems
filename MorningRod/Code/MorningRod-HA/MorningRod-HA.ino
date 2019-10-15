#include "esphome.h"
#include "pins.h"
#include "motor_control.h"

class MyCustomCover : public Component, public Cover {
 public:
  void setup() override {
    // This will be called by App.setup()
    pinMode(5, INPUT);
    pinMode(btn1,INPUT_PULLUP);
    pinMode(btn2,INPUT_PULLUP);


    clockout_setup();
    setup_motors();
  }
  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(false);
    traits.set_supports_position(true);
    traits.set_supports_tilt(false);
    return traits;
  }
  void control(const CoverCall &call) override {
    // This will be called every time the user requests a state change.
    if (call.get_position().has_value()) {
      float pos = *call.get_position();
      // Write pos (range 0-1) to cover
      if (pos == 0) {
          move_open;
      }

      if (pos == 1) {
          move_close;
      }

      // Publish new state
      this->position = pos;
      this->publish_state();
    }
    if (call.get_stop()) {
      // User requested cover stop
      //Can't use this until there is an interrupt
    }
  }
};
