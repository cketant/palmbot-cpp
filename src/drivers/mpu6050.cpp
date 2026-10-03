#include "drivers/mpu6050.h"

#include <iostream>
#include <system_error>


MPU6050::MPU6050(I2CBus& bus, 
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {}

void MPU6050::whoAmI() {
  std::uint8_t whoamiBuff[1];
  bool result = bus_.readReg(addr_, 0x75, 1, whoamiBuff);
  if (result) {
    printf("%s - WHO_AM_I: 0x%02X\n", deviceName_.c_str(), whoamiBuff[0]);
  } else {
    throw std::runtime_error("Unable to print the WHO_AMI_I register");
  }

}