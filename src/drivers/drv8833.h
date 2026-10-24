#include <gpiod.hpp>
#include <string.h>
#include "config/pins.h"
#include "models/motor_gpiod.h"

class DRV8833 {

  public:

    DRV8833(pins::Motor& motor1, 
      pins::Motor& motor2, 
      pins::Gpio& standby,
      std::string gpioChipPath = "/dev/gpiochip0");

    ~DRV8833();
    
    bool isOn(bool isOn);

    bool configure();
    
    struct MotorGpiod {};
    
  private:
    MotorGpiod& motor1_;
    MotorGpiod& motor2_;
    gpiod::line_values nSleepReq_;
};