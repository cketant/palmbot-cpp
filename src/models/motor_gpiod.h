#include "config/pins.h"
#include <gpiod.hpp>

class MotorGpiod {

  public:
    MotorGpiod(pins::Motor& motor, const std::filesystem::path& chipPath);

  private:
    /**
     * Motor In input pin line requests to write
     */
    gpiod::line_request motorInsReqs_;
    /**
     * Motor Encoder pin line requests to read 
     */
    gpiod::line_request motorEncsReqs_;

};