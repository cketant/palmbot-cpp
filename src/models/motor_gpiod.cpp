#include "motor_gpiod.h"

MotorGpiod::MotorGpiod(pins::Motor& motor, const std::filesystem::path& chipPath) : 
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
  ) {};