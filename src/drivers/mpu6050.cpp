#include "drivers/mpu6050.h"
#include <iostream>
#include <system_error>
#include <array>

MPU6050::MPU6050(I2CBus& bus,
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {
    configure();
    wake();
  }

MPU6050::~MPU6050() {
  std::array<uint8_t, 2> turnOff = {0x6B, 0x40};
  bus_.writeReg(addr_, turnOff);
}

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

bool MPU6050::configure() {
  return true;
}

bool MPU6050::readData() {
  bool result = bus_.readReg(addr_, 0x3B, data_);
  if (!result) return false;

  // ACCEL X [0,1]
  std::int16_t accelX = static_cast<std::int16_t>((data_[0]<<8) | data_[1]); 

  // ACCEL Y [2,3]
  std::int16_t accelY = static_cast<std::int16_t>((data_[2]<<8) | data_[3]); 

  // ACCEL Z [4,5]
  std::int16_t accelZ = static_cast<std::int16_t>((data_[4]<<8) | data_[5]); 

  // TEMP [6,7]
  std::int16_t temp = static_cast<std::int16_t>((data_[6]<<8) | data_[7]); 

  // GYRO X [8,9]
  std::int16_t gyroX = static_cast<std::int16_t>((data_[8]<<8) | data_[9]); 

  // GYRO Y [10,11]
  std::int16_t gyroY = static_cast<std::int16_t>((data_[10]<<8) | data_[11]); 

  // GYRO Z [12,13]
  std::int16_t gyroZ = static_cast<std::int16_t>((data_[12]<<8) | data_[13]); 

  m_.accelX = convertToMPS2(accelX);
  m_.accelX = convertToMPS2(accelY);
  m_.accelX = convertToMPS2(accelZ);

  m_.tempC = convertToCelsius(temp);

  m_.angularVelX = convertToAngularVel(gyroX);
  m_.angularVelY = convertToAngularVel(gyroY);
  m_.angularVelZ = convertToAngularVel(gyroZ);

  m_.timestamp = std::chrono::steady_clock::now();

  return true;
}

float MPU6050::convertToMPS2(int16_t gravity) const {
  return (gravity / MPU6050::ACCEL_LSB) * MPU6050::GRAVITY;
}

float MPU6050::convertToAngularVel(int16_t degreesPerSec) const {
  return (degreesPerSec / MPU6050::GYRO_LSB) * MPU6050::DEG_TO_RAD;
}

float MPU6050::convertToCelsius(int16_t raw) const {
  return (raw / 340) + 36.53;
}

bool MPU6050::wake() {
  std::array<uint8_t, 3> wakeData{0x6B, 0x21, 0x00};
  return bus_.writeReg(addr_, wakeData);
}

const MPU6050::Measurement MPU6050::latest() const {
  return m_;
}