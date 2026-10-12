#pragma once

#include <iostream>
#include <cstdio>
#include <iomanip>

#include "hal/i2c_device.h"


class VL53L0X: public I2CDevice {

  public:

    VL53L0X(I2CBus& bus, 
      std::uint8_t deviceAddr = 0x29, 
      std::string deviceName = "VL530X");

    ~VL53L0X();

    /**
     * Print the WHO AM I value
     */
    std::uint8_t whoAmI() override;

    /**
     * The measurement for the sensor
     */
    std::optional<Measurement> measurement() override;

    /**
     * Configure the sensor after wake
     */
    bool configure() override;

    /**
     * Wake the sesnsor
     */
    bool wake() override;

    /**
     * Reset the sensor
     */
    bool reset() override;

  private:
    
    TOFMeasurement m_{};

    std::array<std::uint8_t, 1> data_{};

};