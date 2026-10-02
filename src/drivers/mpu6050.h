#pragma once

#include <cstdint>
#include <system_error>

#include "hal/i2c_bus.h"
#include "hal/i2c_device.h"

class MPU6050: public I2CDevice {

  public:
    MPU6050(I2CBus& bus, const std::uint8_t deviceAddr = 0x68, const std::string deviceName = "MPU6050");

    void whoAmI() override;

};