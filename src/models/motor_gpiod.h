#pragma once

#include <gpiod.hpp>
#include <filesystem>
#include <chrono>

#include "config/pins.h"

class MotorGpiod {

  public:
    MotorGpiod(const pins::Motor& motor, const std::filesystem::path& chipPath);

    void forward(float speed);

    void reverse(float speed);

    void brake(bool isCoast);

    gpiod::edge_event_buffer& getEncoderTicksBlocking(std::chrono::nanoseconds ns);

  private:
    /**
     * Motor In input pin line requests to write
     */
    gpiod::line_request motorInsReqs_;
    /**
     * Motor Encoder pin line requests to read 
     */
    gpiod::line_request motorEncsReqs_;
    /**
     * Motor and Encoder pin values
     */
    const pins::Motor& motor_;
    /**
     * Counting ticks of the encoders. Call this blocks 
     */
    gpiod::edge_event_buffer edgeEventBuffer_ {1024};

};