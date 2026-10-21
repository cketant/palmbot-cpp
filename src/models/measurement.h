#pragma once
#include <chrono>
#include <variant>

/**
 * Measurements from the MPU6050
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

/**
 * Measurement from the VL53L0X
 */
struct TOFMeasurement {
  /**
   * Distance to the nearest object
   */
  float distance {0};
};

struct Measurement {
  std::chrono::steady_clock::time_point timestamp;
  std::variant<IMUMeasurement, TOFMeasurement> data;
};