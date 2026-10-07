#pragma once

#include <cstdint>
#include <system_error>
#include <array>
#include <chrono>

#include "hal/i2c_bus.h"
#include "hal/i2c_device.h"

class MPU6050: public I2CDevice {

  public:
    MPU6050(I2CBus& bus, 
      const std::uint8_t deviceAddr = 0x68, 
      const std::string deviceName = "MPU6050");

    std::uint8_t whoAmI() override;

    /**
     * Take the sensor out of sleep mode (it powers up asleep, and
     * the data registers read zero until this is called).
     * Call once before readData().
     */
    bool wake();

    /**
    Read the data registers from the sensor
      3B ACCEL_XOUT_H
      3C ACCEL_XOUT_L
      3D ACCEL_YOUT_H
      3E ACCEL_YOUT_L
      3F ACCEL_ZOUT_H
      40 ACCEL_ZOUT_L
      41 TEMP_OUT_H
      42 TEMP_OUT_L
      43 GYRO_XOUT_H 
      44 GYRO_XOUT_L 
      45 GYRO_YOUT_H 
      46 GYRO_YOUT_L 
      47 GYRO_ZOUT_H 
      48 GYRO_ZOUT_L 
    */
    bool readData() override;
    
    /**
     * Configure the sensor
     */
    bool configure() override;

    struct Measurement {
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
      /**
       * Timestamp when measurement taken
       */
      std::chrono::steady_clock::time_point timestamp{};
    };

    /**
     * The latest measurements from the sensor. Return
     * a read-only copy. 
     * @return Measurement 
     */
    const Measurement latest() const;

    ~MPU6050() override;

  private:
    /**
     * The LSB Sensitivity for Acceleration
     */
    static constexpr float ACCEL_LSB = 16384;
    /**
     * The LSB Sensitivity for Gyroscope
     */
    static constexpr float GYRO_LSB = 131;
    /**
     * m/s² per g 
     */
    static constexpr float GRAVITY = 9.80665f;
    /**
     * Degrees to Rand
     */
    static constexpr float DEG_TO_RAD = 3.14159265f / 180.f;
    /**
    The buffer for all the data registers of the sensor
    */
    std::array<std::uint8_t, 14> data_{};
    /**
     * The latest measurement taken
     */
    Measurement m_{};
    /**
     * Convert g to Meters per Second squared
     */
    float convertToMPS2(int16_t gravity) const;
    /**
     * Convert Degrees Per Second to Angular Velocity 
     */
    float convertToAngularVel(int16_t degreesPerSec) const;
    /**
     * Convert raw value to celsius
     */
    float convertToCelsius(int16_t raw) const;
};