#include "drivers/mpu6050.h"
#include <iostream>
#include <system_error>
#include <array>

MPU6050::MPU6050(I2CBus& bus, 
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {}

std::uint8_t MPU6050::whoAmI() {
  std::array<std::uint8_t, 1> whoamiBuff{};
  bool result = bus_.readReg(addr_, 0x75, whoamiBuff);
  if (result) {
    printf("%s - WHO_AM_I: 0x%02X\n", deviceName_.c_str(), whoamiBuff[0]);
    return whoamiBuff[0];
  } else {
    throw std::runtime_error("Unable to print the WHO_AMI_I register");
    return -1;
  }
}

bool MPU6050::readData() {
  bool result = bus_.readReg(addr_, 0x3B, data_);
  return result;
}

const MPU6050::Measurement MPU6050::latest() const {
  return m_;
}