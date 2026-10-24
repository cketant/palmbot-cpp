#include "drv8833.h"

DRV8833::DRV8833(
  pins::Motor& motor1, 
  pins::Motor& motor2, 
  pins::Gpio& standby,
  std::string gpioChipPath = "/dev/gpiochip0") {
    configure();
  }

DRV8833::~DRV8833(){}

bool DRV8833::configure() {
  
}