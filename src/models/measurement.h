#pragma once
#include <chrono>
#include <variant>

/**
 * Measurements from the sensor
 */
struct IMUMeasurement {
  /**
   * Acceleration measured as m/s^2
   */
  float accelX{0};
  /**
   * Acceleration measured as m/s^2
   */
  float accelY{0};
  /**
   * Acceleration measured as m/s^2
   */
  float accelZ{0};
  /**
   * Radians per second
   */
  float angularVelX{0};
  /**
   * Radians per second
   */
  float angularVelY{0};
  /**
   * Radians per second
   */
  float angularVelZ{0};
  /**
   * Temperature in Celsius
   */
  float tempC{0};
};

struct Measurement {
  std::chrono::steady_clock::time_point timestamp{};
  std::variant<IMUMeasurement> data;
};