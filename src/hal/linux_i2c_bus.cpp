#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <system_error>

#include "hal/linux_i2c_bus.h"

  LinuxI2CBus::LinuxI2CBus(const char *busPath) :  busPath_(busPath) {

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
      {.addr=deviceAddr, .flags=0, .len=1, .buf=&cursorReg}, // cursor write
      {.addr=deviceAddr, .flags=I2C_M_RD, .len=static_cast<std::uint16_t>(in.size()), .buf=in.data()} // read
    };
    i2c_rdwr_ioctl_data data = {.msgs=msgs, .nmsgs=msgsCount};
    // ioctl returns # of messages sent
    int const sentCount = ioctl(fd_, I2C_RDWR, &data);
    if (sentCount != msgsCount) { 
      throw std::runtime_error("Read & Write Failed.");
      return false;
    }
    return true;
  }

  bool LinuxI2CBus::writeReg(std::uint8_t deviceAddr, std::span<std::uint8_t> out) {
    // 1st byte in the out buffer sets the cursor register
    i2c_msg msgs[1] = {
      {.addr=deviceAddr, .flags=0, .len=static_cast<std::uint16_t>(out.size()), .buf=out.data()}
    };
    i2c_rdwr_ioctl_data data = {.msgs=msgs, .nmsgs=1};
    // ioctl returns # of messages sent
    int const sentCount = ioctl(fd_, I2C_RDWR, &data);
    if (sentCount != 1) {
      throw std::runtime_error("Write Failed.");
      return false;
    }
    return true;
  }

