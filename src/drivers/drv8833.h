#include <gpiod.hpp>
#include <string.h>
#include <filesystem>

#include "config/pins.h"
#include "models/motor_gpiod.h"

class DRV8833 {

  public:

    DRV8833(const pins::Motor& motor1, 
      const pins::Motor& motor2, 
      const pins::Gpio& standby,
      std::string gpioChipPath = "/dev/gpiochip0");

    /**
     * Turn off the H-Bridge
     */
    ~DRV8833();

    bool configure();

    bool forward();

    bool reverse();

    /**
     * coast to a halt or immediately halt
     */
    bool brake(bool isCoast);
    
  private:
    /**
     * NSleep Pin
     */
    pins::Gpio nSleep_;
    MotorGpiod motor1_;
    MotorGpiod motor2_;
    /**
     * The NSleep / StandBy Pin. Low means off
     * High means on.
     */
    gpiod::line_request nSleepReq_;
};