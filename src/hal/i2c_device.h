#pragma once
#include <cstdio>
#include <string.h>

#include "hal/i2c_bus.h"

class I2CDevice {

  public:

    /**
     * The value of the whoami register (0x75) 
     * for the device
     * 
     * @return the WHO_AM_I value
     */
    virtual std::uint8_t whoAmI() = 0;

    /**
     * Read the data from the device include performing
     * the calculations to get the device measurements
     */
    virtual bool readData() = 0;

  protected:
    /**
     * I2C Bus
     */
    I2CBus& bus_;
    /**
     * The address of the device on the bus
     */
    const std::uint8_t addr_;
    /**
     * The name of the device/sensor
     */
    const std::string deviceName_;

    virtual ~I2CDevice() = default;

    I2CDevice& operator=(const I2CDevice&) = delete;

    I2CDevice(const I2CDevice&) = delete;

    I2CDevice(I2CBus& bus, const std::uint8_t deviceAddr, const std::string deviceName)
        : bus_(bus), addr_(deviceAddr), deviceName_(deviceName) {};

};