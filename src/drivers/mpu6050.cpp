#include <iostream>
#include <system_error>
#include <array>

#include "drivers/mpu6050.h"
#include "lib/conversion_math.h"

MPU6050::MPU6050(I2CBus& bus,
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {
    configure();
    wake();
  }

MPU6050::~MPU6050() {
  // Turn off the FIFO Buffer and Reset the FIFO Buffer
  std::array<std::uint8_t, 2> fifoBuff{FIFO_EN_REGISTER, 0x04};
  bus_.writeReg(addr_, fifoBuff);

  // Sleep the sensor
  std::array<uint8_t, 2> turnOff{PWR_MGMT_1_REG, 0x41};
  bus_.writeReg(addr_, turnOff);
}

std::uint8_t MPU6050::whoAmI() {
  std::array<std::uint8_t, 1> whoamiBuff{};
  bool result = bus_.readReg(addr_, WHO_AM_I_REG, whoamiBuff);
  if (result) {
    printf("%s - WHO_AM_I: 0x%02X\n", deviceName_.c_str(), whoamiBuff[0]);
    return whoamiBuff[0];
  } else {
    throw std::runtime_error("Unable to print the WHO_AMI_I register");
    return -1;
  }
}

bool MPU6050::configure() {
  // Configure the DLPF to 1kHz, 2.8ms delay, 98 bandwidth hz
  // See Register 26 docs
  std::array<std::uint8_t, 2> dlpfBuff{DLPF_CONFIG_REG, 0x02};
  if (!bus_.writeReg(addr_, dlpfBuff)) return false;

  // Set the sample rate by setting the Sample Rate Divider = 100Hz
  // See Register 25 docs
  std::array<std::uint8_t, 2> sampleRateBuff{SMPLRT_DIV_REG, 0x10};
  if (!bus_.writeReg(addr_, sampleRateBuff)) return false;

  // Enable the FIFO
  // See register 106 docs
  std::array<std::uint8_t, 2> enableFifoBuff{USR_CTRL_REGISTER, 0x40};
  if (!bus_.writeReg(addr_, enableFifoBuff)) return false;
  
  // Set which sensor values get thrown in the FIFO buffer
  // See register 35
  std::array<std::uint8_t, 2> fifoBuff{FIFO_EN_REGISTER, 0xF8};
  if (!bus_.writeReg(addr_, fifoBuff)) return false;
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

  m_.accelX = ConversionMath::convertToMPS2(accelX);
  m_.accelX = ConversionMath::convertToMPS2(accelY);
  m_.accelX = ConversionMath::convertToMPS2(accelZ);

  m_.tempC = ConversionMath::convertToCelsius(temp);

  m_.angularVelX = ConversionMath::convertToAngularVel(gyroX);
  m_.angularVelY = ConversionMath::convertToAngularVel(gyroY);
  m_.angularVelZ = ConversionMath::convertToAngularVel(gyroZ);

  m_.timestamp = std::chrono::steady_clock::now();

  return true;
}

bool MPU6050::wake() {
  std::array<uint8_t, 3> wakeData{PWR_MGMT_1_REG, 0x21, 0x01};
  return bus_.writeReg(addr_, wakeData);
}