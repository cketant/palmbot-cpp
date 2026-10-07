#include <gtest/gtest.h>

#include "drivers/mpu6050.h"
#include "mocks/hal/i2c_bus.h"

TEST(MPU6050Test, WhoAmIReadsIdentityRegisterAndReturnsValue) {
  MockI2CBus bus;
  MPU6050 sensor(bus);

  EXPECT_CALL(bus, readReg(
      std::uint8_t{0x68},
      std::uint8_t{0x75},
      testing::Truly([](std::span<std::uint8_t> out) {
        return out.size() == 1;
      })))
      .WillOnce([](std::uint8_t, std::uint8_t, std::span<std::uint8_t> out) {
        out[0] = 0x68;
        return true;
      });

  EXPECT_EQ(sensor.whoAmI(), 0x68);
}

TEST(MPU6050Test, WhoAmIThrowsWhenBusReadFails) {
  MockI2CBus bus;
  MPU6050 sensor(bus);

  EXPECT_CALL(bus, readReg(
      std::uint8_t{0x68},
      std::uint8_t{0x75},
      testing::_))
      .WillOnce(testing::Return(false));

  EXPECT_THROW(sensor.whoAmI(), std::runtime_error);
}
