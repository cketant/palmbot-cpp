#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <system_error>

#include "drivers/linux_i2c_bus.h"

  LinuxI2CBus::LinuxI2CBus(const char *busPath, std::uint8_t imuAddress) {
    busPath_ = busPath;
    imuAddress_ = imuAddress;

    fd_ = open(busPath_, O_RDWR);
    if (fd_ < 0) {
      throw std::system_error(errno, std::generic_category(), "open");
    }
    if (ioctl(fd_, I2C_SLAVE, imuAddress_) < 0) {
      int e = errno;
      close(fd_);
      throw std::system_error(e, std::generic_category(), "ioctl");
    }
  }

  bool LinuxI2CBus::readReg(std::uint8_t reg, std::uint8_t &value) {
    if (write(fd_, &reg, 1) < 0) return false;
    if (read(fd_, &value, 1) < 0) return false;
    return true;
  }

  bool LinuxI2CBus::writeReg(std::uint8_t reg, std::uint8_t value) {
    std::uint8_t out[2] = {reg, value};
    return write(fd_, out, 2) == 2;
  }

  void LinuxI2CBus::whoAmI(std::uint8_t reg) {
    std::uint8_t whoami = 0;
    readReg(0x75, whoami);
    printf("WHO_AM_I 0x%02X\n", whoami);
  }

  LinuxI2CBus::~LinuxI2CBus() {
    if (fd_ >= 0) {
      close(fd_);
    }
  }

