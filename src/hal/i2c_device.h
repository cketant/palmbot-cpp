#pragma once
#include <cstdio>
#include <string.h>

#include "hal/i2c_bus.h"

class I2CDevice {

  public:

    virtual ~I2CDevice() = default;

    I2CDevice& operator=(const I2CDevice&) = delete;

    I2CDevice(const I2CDevice&) = delete;

    /**
     * The value of the whoami register (0x75) 
     * for the device
     */
    virtual void whoAmI() = 0;

  protected:

    I2CDevice(I2CBus& bus, const std::uint8_t deviceAddr, const std::string deviceName)
        : bus_(bus), addr_(deviceAddr), deviceName_(deviceName) {};

    I2CBus& bus_;
    const std::uint8_t addr_;
    const std::string deviceName_;

};