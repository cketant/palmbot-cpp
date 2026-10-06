#pragma once

#include "hal/i2c_bus.h"

class LinuxI2CBus: public I2CBus {

  public:

    LinuxI2CBus(const char *busPath);

    ~LinuxI2CBus() override;
    
    /**
     * @param deviceAddr Address of the device on I2C bus
     * @param cursorReg The address of the register where to start the cursor
     * @param in The in buffer
     */
    bool readReg(std::uint8_t deviceAddr, std::uint8_t cursorReg, std::span<std::uint8_t> in) override;

    /**
     * The first byte in the out buffer sets the cursor to the register. The remaining bytes
     * are set to the each register at the cursor register then the following registers.
     * @param deviceAddr Address of the device on I2C bus
     * @param out The out buffer
     */
    bool writeReg(std::uint8_t deviceAddr, std::span<std::uint8_t> out) override;


  private:
    /*
    File Descriptor
    */
    int fd_ = -1;
    /*
    The file path of the I2C bus on linux
    */
    const char *busPath_;
};