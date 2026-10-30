#pragma once

#include <gpiod.hpp>
#include <string>
#include <filesystem>
#include <chrono>

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

    bool forward(float speed=1.0);

    bool reverse(float speed=1.0);

    bool turnLeft();

    bool turnRight();

    bool brake(bool isCoast);

    std::int64_t readLeftMotorEncoderTicks(std::chrono::nanoseconds timeout);
    
  private:
    MotorGpiod motor1_;
    MotorGpiod motor2_;
    /**
     * NSleep Pin
     */
    pins::Gpio nSleep_;
    /**
     * The NSleep / StandBy Pin. Low means off
     * High means on.
     */
    gpiod::line_request nSleepReq_;
};