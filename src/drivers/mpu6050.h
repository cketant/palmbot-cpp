#pragma once

#include <cstdint>
#include <system_error>
#include <array>
#include <chrono>

#include "hal/i2c_bus.h"
#include "hal/i2c_device.h"
#include "models/SPSCQueue.h"

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
     * Register for setting the Sample Rate Divider - Register 25
     */
    static constexpr std::uint8_t SMPLRT_DIV_REG = 0x19;
    /**
     * Register for setting the Power Management for wake - Register 107
     */
    static constexpr std::uint8_t PWR_MGMT_1_REG = 0x6B;
    /**
     * Register for setting Data Low Pass Filter - Register 26
     */
    static constexpr std::uint8_t DLPF_CONFIG_REG = 0x1A;
    /**
     * Register configure which sensor values are loaded into FIFO - Register 35
     */
    static constexpr std::uint8_t FIFO_EN_REGISTER = 0x23;
    /**
     * Enable/Disable the FIFO buffer - Register 106
     */
    static constexpr std::uint8_t USR_CTRL_REGISTER = 0x6A;
    /**
     * FIFO H count number of samples - Register 114
     */
    static constexpr std::uint8_t FIFO_COUNT_H_REG = 0x72;
    /**
     * FIFO L count number of samples - Register 115
     */
    static constexpr std::uint8_t FIFO_COUNT_L_REG = 0x73;
    /**
     * FIFO buffer of samples - Register 116
     */
    static constexpr std::uint8_t FIFO_BUFF_REG = 0x74;
    /**
     * Take the sensor out of sleep mode (it powers up asleep, and
     * the data registers read zero until this is called).
     * Call once before readData().
     */
    bool wake() override;

    /**
     * Reset the device to default values
     */
    bool reset() override;

    /**
    Read the data registers from the sensor
    */
    std::optional<Measurement> measurement() override;
    
    /**
     * Configure the sensor
     */
    bool configure() override;
    /**
     * Who Am I register value
     */
    std::uint8_t whoAmI() override;
    
  private:
    /**
    The buffer for all the data registers of the sensor
    */
    std::array<std::uint8_t, 14> data_{};
    /**
     * The latest measurement taken
     */
    IMUMeasurement m_{};

};