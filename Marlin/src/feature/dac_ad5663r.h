#pragma once

#include "../MarlinCore.h"
#include "../inc/MarlinConfig.h"
#include <stdint.h>

#include HAL_PATH(../HAL, MarlinSPI.h)

typedef struct {
  uint32_t min_val;
  uint32_t mid_val;
  uint32_t max_val;
  float scaling; // DAC_XY_SCALE
} dac_settings_t;


static constexpr dac_settings_t dac_default = {
  22768,
  32768,
  42768,
  100.0f
};

class ad5663r
{
  static MarlinSPI mySPI;
  static SPI_HandleTypeDef SPIx;
public:
    ad5663r();
    static dac_settings_t settings[2];
    static void asdf();
    static void tick();
    static void SetCS(bool val) { OUT_WRITE(DAC_CS_PIN, val); }
    static inline void DataTransferBegin() {  if (SPIx.Instance) { HAL_SPI_Init(&SPIx); SetCS(false); }  };
    static inline void DataTransferEnd() { SetCS(true); };
    static void init(uint8_t spiRate);
    static void initDMA(uint8_t spiRate);
    static void setValue(const uint8_t channel, const uint16_t value);
    static void setValueDMA(const uint8_t channel, const uint16_t value);
    static uint8_t HardwareIO2(uint8_t data);
    static uint8_t HardwareIO(uint8_t *data);
    static uint8_t SoftwareIO(uint8_t data);
  static uint8_t IO(uint8_t data = 0) { return SPIx.Instance ? HardwareIO2(data) : SoftwareIO(data); }
};