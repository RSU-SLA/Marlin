#include "dac_stm32f4.h"

dac_stm32f4::dac_stm32f4() {}

DAC_HandleTypeDef dac_stm32f4::DACx;

void dac_stm32f4::setValue(const uint8_t channel, const uint8_t value)
{
    uint8_t val = std::min<uint8_t>(value, 116);
    
    //HAL_DAC_Start(&DACx, DAC_CHANNEL_2);
    HAL_DAC_SetValue(&DACx,DAC_CHANNEL_2, DAC_ALIGN_8B_R, val);

    //SERIAL_ERROR_MSG("DAC: ", value, " : ", val);
    //HAL_DAC_Stop(&DACx, DAC_CHANNEL_2);
}

void dac_stm32f4::init(uint8_t dacTrigger)
{
    DAC_TypeDef *dacInstance;

    DAC_ChannelConfTypeDef sConfig;

    dacInstance      = (DAC_TypeDef *)pinmap_peripheral(digitalPinToPinName(PA4),  PinMap_DAC);
    if (dacInstance != (DAC_TypeDef *)pinmap_peripheral(digitalPinToPinName(PA4), PinMap_DAC)) dacInstance = NP;

    DACx.Instance = dacInstance;

    if (DACx.Instance)
    {
        DACx.State = HAL_DAC_STATE_RESET;

    }

    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    HAL_DAC_Init(&DACx);
    HAL_DAC_ConfigChannel(&DACx, &sConfig, DAC_CHANNEL_2);

    HAL_DAC_Start(&DACx, DAC_CHANNEL_2);

    HAL_DAC_SetValue(&DACx,DAC_CHANNEL_2,DAC_ALIGN_8B_R, 0);
    //HAL_DAC_Stop(&DACx, DAC_CHANNEL_2);
    //(&DACx, DAC_CHANNEL_1);
}