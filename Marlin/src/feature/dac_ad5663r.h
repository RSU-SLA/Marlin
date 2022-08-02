#pragma once

#include "../MarlinCore.h"
#include "../inc/MarlinConfig.h"
#include <stdint.h>

#include HAL_PATH(../HAL, MarlinSPI.h)

class ad5663r
{
  static MarlinSPI mySPI;
  static SPI_HandleTypeDef SPIx;
public:
    ad5663r();
    static void asdf();
    static void tick();
    static inline void DataTransferBegin() { if (SPIx.Instance) { HAL_SPI_Init(&SPIx); } WRITE(DAC_CS_PIN, LOW); };
    static inline void DataTransferEnd() { WRITE(DAC_CS_PIN, HIGH); };
    static void init(uint8_t spiRate);
    static void init2(uint8_t spiRate);
    static void setValue(const uint8_t channel, const uint16_t value);
    static void setValue2(const uint8_t channel, const uint8_t value);
    static uint8_t HardwareIO(uint8_t data);
    static uint8_t HardwareIO(uint8_t *data);
    static uint8_t SoftwareIO(uint8_t data);
  static uint8_t IO(uint8_t data = 0) { return SPIx.Instance ? HardwareIO(data) : SoftwareIO(data); }
};