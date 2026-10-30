#include "motor_gpiod.h"

MotorGpiod::MotorGpiod(const pins::Motor& motor, const std::filesystem::path& chipPath) :
  motorInsReqs_(
    gpiod::chip(chipPath)
      .prepare_request()
      .set_consumer("set-in-line-values")
      .add_line_settings(
        {motor.in1, motor.in2},
        gpiod::line_settings().set_direction(
          gpiod::line::direction::OUTPUT
        ))
      .set_output_values({gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE })
      .do_request()
  ), motorEncsReqs_(
    gpiod::chip(chipPath)
      .prepare_request()
      .set_consumer("get-encoder-line-values")
      .add_line_settings(
        {motor.enc1, motor.enc2},
        gpiod::line_settings().set_direction(
          gpiod::line::direction::INPUT
        ).set_edge_detection(
          gpiod::line::edge::BOTH)
        )
      .do_request()
  ), motor_(motor) {};

  void MotorGpiod::forward(float speed) {
    motorInsReqs_.set_values({gpiod::line::value::ACTIVE, gpiod::line::value::INACTIVE});
  }

  void MotorGpiod::reverse(float speed) {
    motorInsReqs_.set_values({gpiod::line::value::INACTIVE, gpiod::line::value::ACTIVE});
  }

  void MotorGpiod::brake(bool isCoast) {
    if (isCoast) {
      motorInsReqs_.set_values({gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
    } else {
      motorInsReqs_.set_values({gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
    }
  }

  std::int64_t MotorGpiod::readEncoderTicksBlocking(std::chrono::nanoseconds ns) {
    if (!motorEncsReqs_.wait_edge_events(ns)) return 0;
    return static_cast<int64_t>(motorEncsReqs_.read_edge_events(edgeEventBuffer_));
  }