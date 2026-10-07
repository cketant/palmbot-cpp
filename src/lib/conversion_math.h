#pragma once

#include <cstdint>

class ConversionMath {
  public:
    /**
     * m/s² per g 
     */
    static constexpr float GRAVITY = 9.80665f;
    /**
     * Degrees to Rand
     */
    static constexpr float DEG_TO_RAD = 3.14159265f / 180.f;
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