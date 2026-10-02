#pragma once

#include "hal/i2c_bus.h"

class LinuxI2CBus: public I2CBus {

  public:

    LinuxI2CBus(const char *busPath);

    ~LinuxI2CBus() override;

    bool readReg(std::uint8_t deviceAddr, std::uint8_t cursorReg, std::uint8_t len, std::uint8_t *in) override;
    
    bool writeReg(std::uint8_t deviceAddr, std::uint8_t len, std::uint8_t *out) override;


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