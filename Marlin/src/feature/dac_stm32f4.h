#pragma once

#include "../MarlinCore.h"
#include "../inc/MarlinConfig.h"
#include <stdint.h>

#include HAL_PATH(../HAL, MarlinSPI.h)

class dac_stm32f4
{
  static DAC_HandleTypeDef DACx;
public:
    dac_stm32f4();
    static void init(uint8_t spiRate);
    static void setValue(const uint8_t channel, const uint8_t value);
};