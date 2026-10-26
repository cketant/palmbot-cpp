#include "drv8833.h"

DRV8833::DRV8833(
  const pins::Motor& motor1, 
  const pins::Motor& motor2, 
  const pins::Gpio& standby,
  std::string gpioChipPath) : motor1_(motor1, std::filesystem::path{gpioChipPath}), 
  motor2_(motor2, std::filesystem::path{gpioChipPath}), 
  nSleep_(standby), 
  nSleepReq_(
    gpiod::chip(gpioChipPath)
      .prepare_request()
      .set_consumer("set-nsleep")
      .add_line_settings(
        standby, 
        gpiod::line_settings().set_direction(
          gpiod::line::direction::OUTPUT
        ))
      .set_output_values({gpiod::line::value::INACTIVE})
      .do_request()
    ) {
      configure();
    }

DRV8833::~DRV8833(){
  nSleepReq_.set_value(nSleep_, gpiod::line::value::INACTIVE);
}

bool DRV8833::configure() {
  nSleepReq_.set_value(nSleep_, gpiod::line::value::ACTIVE);
  return true;
}

bool DRV8833::forward(float speed) {
  motor1_.forward(speed);
  motor2_.forward(speed);
  return true;
}

bool DRV8833::reverse(float speed) {
  motor1_.reverse(speed);
  motor2_.reverse(speed);
  return true;
}

bool DRV8833::turnLeft() {
  motor1_.brake(false);
  motor2_.forward(0.3);
  return true;
}

bool DRV8833::turnRight() {
  motor1_.forward(0.3);
  motor2_.brake(false);
  return true;
}

bool DRV8833::brake(bool isCoast) {
  motor1_.brake(isCoast);
  motor2_.brake(isCoast);
  return true;
}

void DRV8833::logLeftMotorEncoderTicks(std::chrono::nanoseconds remaining) {
  motor1_.getEncoderTicksBlocking(remaining);
}