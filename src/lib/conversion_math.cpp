#include "conversion_math.h"
#include "drivers/mpu6050.h"

float ConversionMath::convertToMPS2(int16_t gravity) {
  return (static_cast<float>(gravity) / MPU6050::ACCEL_LSB) * ConversionMath::GRAVITY;
}

float ConversionMath::convertToAngularVel(int16_t degreesPerSec) {
  return (static_cast<float>(degreesPerSec) / MPU6050::GYRO_LSB) * ConversionMath::DEG_TO_RAD;
}

float ConversionMath::convertToCelsius(int16_t raw) {
  return (static_cast<float>(raw) / 340.0F) + 36.53F;
}