#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <system_error>

#include "hal/linux_i2c_bus.h"

  LinuxI2CBus::LinuxI2CBus(const char *busPath) : I2CBus(), busPath_(busPath) {

    fd_ = open(busPath_, O_RDWR);
    if (fd_ < 0) {
      throw std::system_error(errno, std::generic_category(), "open");
    }
  }

  LinuxI2CBus::~LinuxI2CBus() {
    if (fd_ >= 0) {
      close(fd_);
    }
  }

  bool LinuxI2CBus::readReg(std::uint8_t deviceAddr, std::uint8_t cursorReg, std::span<std::uint8_t> in) {
    constexpr int msgsCount = 2;
    i2c_msg msgs[msgsCount] = {
      {deviceAddr, 0, 1, &cursorReg}, // cursor write
      {deviceAddr, I2C_M_RD, in.size(), in.data()} // read
    };
    i2c_rdwr_ioctl_data data = {msgs, msgsCount};
    // ioctl returns # of messages sent
    int sentCount = ioctl(fd_, I2C_RDWR, &data);
    if (sentCount != msgsCount) { 
      std::runtime_error("Read & Write Failed.");
      return false;
    }
    return true;
  }

  bool LinuxI2CBus::writeReg(std::uint8_t deviceAddr, std::span<std::uint8_t> out) {
    i2c_msg msg = {deviceAddr, 0, out.size(), out.data()};
    i2c_rdwr_ioctl_data data = {&msg, 1};
    // ioctl returns # of messages sent
    int sentCount = ioctl(fd_, I2C_RDWR, &data);
    if (sentCount != 1) {
      std::runtime_error("Write Failed.");
      return false;
    }
    return true;
  }

