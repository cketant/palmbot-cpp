#include "vl53l0x.h"

VL53L0X::VL53L0X(I2CBus& bus, 
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {
    reset();
    wake();
    configure();
};

VL53L0X::~VL53L0X() {};

std::uint8_t VL53L0X::whoAmI() {
  std::array<std::uint8_t, 1> whoAmIBuff{};
  if (!bus_.readReg(addr_, 0xC0, whoAmIBuff)) return -1;
  std::cout << deviceName_ << " - " << "WHO_AM_I: " << std::hex << std::showbase << (int)whoAmIBuff[0] << std::endl;
  return whoAmIBuff[0];
};

bool VL53L0X::configure() {
  return true;
};

bool VL53L0X::wake() {
  return true;
};

bool VL53L0X::reset() {
  return true;
};

std::optional<Measurement> VL53L0X::measurement() {
  return {};
};