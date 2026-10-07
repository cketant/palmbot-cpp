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

    ~MPU6050() override;
    /**
     * The LSB Sensitivity for Acceleration
     */
    static constexpr float ACCEL_LSB = 16384;
    /**
     * The LSB Sensitivity for Gyroscope
     */
    static constexpr float GYRO_LSB = 131;
    /**
     * Take the sensor out of sleep mode (it powers up asleep, and
     * the data registers read zero until this is called).
     * Call once before readData().
     */
    bool wake();

    /**
    Read the data registers from the sensor
    */
    bool readData() override;
    
    /**
     * Configure the sensor
     */
    bool configure() override;
    /**
     * Who Am I register value
     */
    std::uint8_t whoAmI() override;

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

  private:
    /**
    The buffer for all the data registers of the sensor
    */
    std::array<std::uint8_t, 14> data_{};
    /**
     * The latest measurement taken
     */
    Measurement m_{};

};