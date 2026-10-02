#pragma once

#include "i2c_bus.h"

class LinuxI2CBus: public I2CBus {

  public:

    LinuxI2CBus(const char *busPath, std::uint8_t imuAddress);

    LinuxI2CBus& operator=(const LinuxI2CBus&) = delete;

    LinuxI2CBus(const LinuxI2CBus&) = delete;

    ~LinuxI2CBus() override;

    bool readReg(std::uint8_t reg, std::uint8_t &value) override;
    
    bool writeReg(std::uint8_t reg, std::uint8_t value) override;

    void whoAmI(std::uint8_t reg) override;


  private:
    /*
    File Descriptor
    */
    int fd_ = -1;
    /*
    Address of the sensor on the I2C
    */
    u_int8_t imuAddress_;
    /*
    The file path of the I2C bus on linux
    */
    const char *busPath_;

    
};