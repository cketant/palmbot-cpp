#pragma once

#include <cstdint>

class I2CBus {

  public:
    /**
     * @param deviceAddr Address of the device on I2C bus
     * @param cursorReg The address of the register where to start the cursor
     * @param len The length of the in buffer
     * @param in The in buffer
     */
    virtual bool readReg(std::uint8_t deviceAddr, std::uint8_t cursorReg, std::uint8_t len, std::uint8_t *in) = 0;
    
    /**
     * @param deviceAddr Address of the device on I2C bus
     * @param len The length of the out buffer
     * @param out The out buffer
     */
    virtual bool writeReg(std::uint8_t deviceAddr, std::uint8_t len, std::uint8_t *out) = 0;

    virtual ~I2CBus() = default;
    
    I2CBus& operator=(const I2CBus&) = delete;

    I2CBus(const I2CBus&) = delete;

  protected:
  
    I2CBus() = default;
    
};