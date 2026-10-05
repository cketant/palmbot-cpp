#include <cstdint>
#include <span>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "drivers/mpu6050.h"
#include "hal/i2c_bus.h"

class MockI2CBus final : public I2CBus {
public:
  MOCK_METHOD(bool, readReg,
              (std::uint8_t deviceAddr,
               std::uint8_t cursorReg,
               std::span<std::uint8_t> in),
              (override));

  MOCK_METHOD(bool, writeReg,
              (std::uint8_t deviceAddr,
               std::span<std::uint8_t> out),
              (override));

  ~MockI2CBus() override = default;
};