#include <gtest/gtest.h>

#include "lib/conversion_math.h"

TEST(ConversionMathTests, convertToMPS2) {
  EXPECT_NEAR(ConversionMath::convertToMPS2(16400), 9.816f, 1e-3f);
}

TEST(ConversionMathTests, convertToAngularVel) {
  EXPECT_NEAR(ConversionMath::convertToAngularVel(-655), -0.0873f, 1e-4f);
}

TEST(ConversionMathTests, convertToCelsius) {
  EXPECT_NEAR(ConversionMath::convertToCelsius(-4000), 24.77f, 1e-2f);
}
