#pragma once

#include <cstdint>
#include <numbers>

class ConversionMath {
  public:
    /**
     * m/s² per g 
     */
    static constexpr float GRAVITY = 9.80665F;
    /**
     * Degrees to Rand
     */
    static constexpr float DEG_TO_RAD = std::numbers::pi_v<float> / 180.F;
    /**
     * Convert g to Meters per Second squared
     */
    static float convertToMPS2(int16_t gravity);
    /**
     * Convert Degrees Per Second to Angular Velocity 
     */
    static float convertToAngularVel(int16_t degreesPerSec);
    /**
     * Convert raw value to celsius
     */
    static float convertToCelsius(int16_t raw);
};