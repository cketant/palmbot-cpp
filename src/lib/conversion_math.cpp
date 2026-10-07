#include "conversion_math.h"
#include "drivers/mpu6050.h"

float ConversionMath::convertToMPS2(int16_t gravity) {
  return (gravity / MPU6050::ACCEL_LSB) * ConversionMath::GRAVITY;
}

float ConversionMath::convertToAngularVel(int16_t degreesPerSec) {
  return (degreesPerSec / MPU6050::GYRO_LSB) * ConversionMath::DEG_TO_RAD;
}

float ConversionMath::convertToCelsius(int16_t raw) {
  return (raw / 340.0f) + 36.53f;
}