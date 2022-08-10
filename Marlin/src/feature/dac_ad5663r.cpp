#include "dac_ad5663r.h"


ad5663r::ad5663r() {}
#define GALVO_XY_DMA

MarlinSPI ad5663r::mySPI(DAC_MOSI_PIN, NC, DAC_SCK_PIN, NC);
#ifdef GALVO_XY_DMA
void ad5663r::setValue(const uint8_t channel, const uint16_t value)
{
  
  OUT_WRITE(DAC_CS_PIN, LOW);
  static uint8_t tbuf[2][3];
  
    uint8_t datax[3];
    tbuf[channel][0] = 0b00011000;
    tbuf[channel][0] |= channel;
    tbuf[channel][1] = (value >> 8) & 0xFF;
    tbuf[channel][2] = value & 0xFF;
  mySPI.dmaTransfer(&tbuf[channel][0], nullptr, 3);
  OUT_WRITE(DAC_CS_PIN, HIGH);
}

void ad5663r::tick() 
{}

void ad5663r::init(uint8_t) {

  OUT_WRITE(DAC_CS_PIN, HIGH);
  
  /**
   * STM32F1 APB2 = 72MHz, APB1 = 36MHz, max SPI speed of this MCU if 18Mhz
   * STM32F1 has 3 SPI ports, SPI1 in APB2, SPI2/SPI3 in APB1
   * so the minimum prescale of SPI1 is DIV4, SPI2/SPI3 is DIV2
   */
  #if SPI_DEVICE == 1
    #define SPI_CLOCK_MAX SPI_CLOCK_DIV4
  #else
    #define SPI_CLOCK_MAX SPI_CLOCK_DIV2
  #endif
  uint8_t clock;
  uint8_t spiRate = SPI_SPEED_5;
  switch (spiRate) {
    case SPI_FULL_SPEED:    clock = SPI_CLOCK_MAX;  break;
    case SPI_HALF_SPEED:    clock = SPI_CLOCK_DIV4; break;
    case SPI_QUARTER_SPEED: clock = SPI_CLOCK_DIV8; break;
    case SPI_EIGHTH_SPEED:  clock = SPI_CLOCK_DIV16; break;
    case SPI_SPEED_5:       clock = SPI_CLOCK_DIV32; break;
    case SPI_SPEED_6:       clock = SPI_CLOCK_DIV64; break;
    default:                clock = SPI_CLOCK_DIV2;// Default from the SPI library
  }

  mySPI.setClockDivider(clock);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(SPI_MODE2);
  mySPI.begin();
  return;
  SPI_TypeDef *spiInstance;

  OUT_WRITE(DAC_CS_PIN, HIGH);

  spiInstance      = (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(DAC_SCK_PIN),  PinMap_SPI_SCLK);
  if (spiInstance != (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(DAC_MOSI_PIN), PinMap_SPI_MOSI)) spiInstance = NP;

  SPIx.Instance                = spiInstance;

  if (SPIx.Instance) {
    SPIx.State                   = HAL_SPI_STATE_RESET;
    SPIx.Init.NSS                = SPI_NSS_SOFT;
    SPIx.Init.Mode               = SPI_MODE_MASTER;
    SPIx.Init.Direction          = SPI_DIRECTION_2LINES;
    SPIx.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_4;
    SPIx.Init.CLKPhase           = SPI_PHASE_1EDGE;
    SPIx.Init.CLKPolarity        = SPI_POLARITY_HIGH;
    SPIx.Init.DataSize           = SPI_DATASIZE_8BIT;
    SPIx.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    SPIx.Init.TIMode             = SPI_TIMODE_DISABLE;
    SPIx.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
    SPIx.Init.CRCPolynomial      = 10;

    pinmap_pinout(digitalPinToPinName(DAC_SCK_PIN), PinMap_SPI_SCLK);
    pinmap_pinout(digitalPinToPinName(DAC_MOSI_PIN), PinMap_SPI_MOSI);

    #ifdef SPI1_BASE
      if (SPIx.Instance == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        SERIAL_ECHO_MSG("SPI 1 IO");
      }
    #endif
    #ifdef SPI2_BASE
      if (SPIx.Instance == SPI2) {
        
        SERIAL_ECHO_MSG("SPI 2 IO");
        __HAL_RCC_SPI2_CLK_ENABLE();
      }
    #endif
    #ifdef SPI3_BASE
      if (SPIx.Instance == SPI3) {
        SERIAL_ECHO_MSG("SPI 3 IO");
        __HAL_RCC_SPI3_CLK_ENABLE();
      }
    #endif
  }
  else {
        SERIAL_ECHO_MSG("No SPI instance");
    SPIx.Instance = nullptr;
    SET_OUTPUT(DAC_MOSI_PIN);
    SET_OUTPUT(DAC_SCK_PIN);
  }

  DataTransferBegin();

  uint8_t data[3];
  data[0] = 0b00111111;
  data[1] = 0x0F;
  data[2] = 0x01;
  HardwareIO(data);
/*
  IO(0b00111111);
  IO(0x0F);
  IO(0x01);
  */
  DataTransferEnd();

}
#endif
#ifdef GALVO_XY_NO_DMA
#include "pinconfig.h"

SPI_HandleTypeDef ad5663r::SPIx;


uint8_t ad5663r::HardwareIO2(uint8_t data) {
  SERIAL_ECHO_MSG("Hardware IO");
  SPI_1LINE_TX(&SPIx);
  __HAL_SPI_ENABLE(&SPIx);
  while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}
  SPIx.Instance->DR = data;
  while ((SPIx.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE) {}
  __HAL_SPI_DISABLE(&SPIx);

  return SPIx.Instance->DR;
}

uint8_t ad5663r::HardwareIO(uint8_t *data)
{
  SPI_1LINE_TX(&SPIx);
  __HAL_SPI_ENABLE(&SPIx);
  for (int i = 0; i < 3; ++i)
  {
    while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}
    if (data == nullptr)
      {
        //SERIAL_ERROR_MSG("NULL DATA idx", i);
        break;
      }
    SPIx.Instance->DR = data[i];
    //while ((SPIx.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE) {}
  }
  while (__HAL_SPI_GET_FLAG(&SPIx, SPI_FLAG_BSY)) {}
  __HAL_SPI_DISABLE(&SPIx);

  return SPIx.Instance->DR;
  
}

void ad5663r::setValue(const uint8_t channel, const uint16_t value)
{
  if (SPIx.Instance == nullptr)
    return;
    
  SERIAL_ERROR_MSG("setValue channel: ", channel, " val : ", value);
    static uint16_t lastData[2] = {0,0};
    if (channel < 2)
    {
        if (lastData[channel] == value)
        {
          SERIAL_ERROR_MSG("SAME");
          return;
        }
    }else if (lastData[0] == value || lastData[1] == value)
    {
      SERIAL_ERROR_MSG("SAME");
      return;
    }

    
    while (__HAL_SPI_GET_FLAG(&SPIx, SPI_FLAG_BSY)) {}    
    if (channel == 7)
    {
      lastData[0] = value;
      lastData[1] = value;
    }
    else
      lastData[channel] = value;
    DataTransferBegin();
    static uint8_t datax[3];
    datax[0] = 0b00011000;
    datax[0] |= channel;
    datax[1] = (value >> 8) & 0xFF;
    datax[2] = value & 0xFF;
    HardwareIO(datax);
    
    DataTransferEnd();
}


uint8_t ad5663r::SoftwareIO(uint8_t data) {
    
  SERIAL_ECHO_MSG("Software IO");
  uint8_t result = 0;

  return result;
}

void ad5663r::initDMA(uint8_t) {
}

void ad5663r::init(uint8_t) {
  SPI_TypeDef *spiInstance;

  SetCS(true);

  spiInstance      = (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(DAC_SCK_PIN),  PinMap_SPI_SCLK);
  if (spiInstance != (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(DAC_MOSI_PIN), PinMap_SPI_MOSI)) spiInstance = NP;

  SPIx.Instance                = spiInstance;

  if (SPIx.Instance) {
    SPIx.State                   = HAL_SPI_STATE_RESET;
    SPIx.Init.NSS                = SPI_NSS_SOFT;
    SPIx.Init.Mode               = SPI_MODE_MASTER;
    SPIx.Init.Direction          = SPI_DIRECTION_2LINES;
    SPIx.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_2;
    SPIx.Init.CLKPhase           = SPI_PHASE_1EDGE;
    SPIx.Init.CLKPolarity        = SPI_POLARITY_HIGH;
    SPIx.Init.DataSize           = SPI_DATASIZE_8BIT;
    SPIx.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    SPIx.Init.TIMode             = SPI_TIMODE_DISABLE;
    SPIx.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
    SPIx.Init.CRCPolynomial      = 10;

    pinmap_pinout(digitalPinToPinName(DAC_SCK_PIN), PinMap_SPI_SCLK);
    pinmap_pinout(digitalPinToPinName(DAC_MOSI_PIN), PinMap_SPI_MOSI);
    //pin_PullConfig(get_GPIO_Port(STM_PORT(digitalPinToPinName(DAC_SCK_PIN))), STM_LL_GPIO_PIN(digitalPinToPinName(DAC_MOSI_PIN)), GPIO_PULLDOWN);
    pin_SetPinSpeed(get_GPIO_Port(STM_PORT(digitalPinToPinName(DAC_MOSI_PIN))), STM_LL_GPIO_PIN(digitalPinToPinName(DAC_MOSI_PIN)), LL_GPIO_SPEED_FREQ_VERY_HIGH);
    pin_SetPinSpeed(get_GPIO_Port(STM_PORT(digitalPinToPinName(DAC_CS_PIN))), STM_LL_GPIO_PIN(digitalPinToPinName(DAC_CS_PIN)), LL_GPIO_SPEED_FREQ_VERY_HIGH);
    pin_SetPinSpeed(get_GPIO_Port(STM_PORT(digitalPinToPinName(DAC_SCK_PIN))), STM_LL_GPIO_PIN(digitalPinToPinName(DAC_SCK_PIN)), LL_GPIO_SPEED_FREQ_VERY_HIGH);

    #ifdef SPI1_BASE
      if (SPIx.Instance == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        SERIAL_ECHO_MSG("SPI 1 IO");
      }
    #endif
    #ifdef SPI2_BASE
      if (SPIx.Instance == SPI2) {
        
        SERIAL_ECHO_MSG("SPI 2 IO");
        __HAL_RCC_SPI2_CLK_ENABLE();
      }
    #endif
    #ifdef SPI3_BASE
      if (SPIx.Instance == SPI3) {
        SERIAL_ECHO_MSG("SPI 3 IO");
        __HAL_RCC_SPI3_CLK_ENABLE();
      }
    #endif
  }
  else {
        SERIAL_ECHO_MSG("No SPI instance");
    SPIx.Instance = nullptr;
    SET_OUTPUT(DAC_MOSI_PIN);
    SET_OUTPUT(DAC_SCK_PIN);
  }

  DataTransferBegin();

  uint8_t data[3];
  data[0] = 0b111000;
  data[1] = 0x00;
  data[2] = 0x01;
  HardwareIO(data);
  DataTransferEnd();


  //while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}

  // power up
  /*
  DataTransferBegin();

  data[0] = 0b00100000;
  data[1] = 0x00;
  data[2] = 0b000011;
  HardwareIO(data);
  DataTransferEnd();
  */
  //while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}
  
  setValue(7, 0x8012); // mid

}
float frequencies[] = {10e1, 50e1, 1e1, 1e1};
float phases[] = {0, 0, 180, 270};
float offsets[] = {32768, 32768, 32768, 32768};
float amplitudes[] = {10000, 10000, 32600, 32600};

void ad5663r::tick()
{
  return;
  float t = 1e-6 * float(micros());

    int i = 0;
  float x = offsets[i] + amplitudes[i]
            * sin(2.0*M_PI*(t*frequencies[i] + phases[i]/360.0));
  i = 1;
  float y = offsets[i] + amplitudes[i]
            * sin(2.0*M_PI*(t*frequencies[i] + phases[i]/360.0));
  uint16_t output_y = (uint16_t)y;
  uint16_t output_x = (uint16_t)x;
  if (y > 0xFFFF)
        output_y = 0xFFFF;
      else if (y < 0)
        output_y = 0;
        
  if (x > 0xFFFF)
        output_x = 0xFFFF;
      else if (x < 0)
        output_x = 0;


setValue(0, output_x);
setValue(1, output_y);
return;
//SERIAL_ERROR_MSG("Tick");
  DataTransferBegin();
  
  uint8_t datax[3];
  datax[0] = 0b00011000;
  datax[1] = (output_x >> 8) & 0xFF;
  datax[2] = output_x & 0xFF;
  HardwareIO(datax);
  
  DataTransferEnd();

  
  DataTransferBegin();
  uint8_t datay[3];
  datay[0] = 0b00011001;
  datay[1] = (output_y >> 8) & 0xFF;
  datay[2] = output_y & 0xFF;
  HardwareIO(datay);

  DataTransferEnd();
  /*
  IO(0b00011000);
  IO((output_x >> 8) & 0xFF);
  IO(output_x & 0xFF);
  DataTransferEnd();

  DataTransferBegin();
  IO(0b00011001);
  IO((output_y >> 8) & 0xFF);
  IO(output_y & 0xFF);
  DataTransferEnd();
  */
}

#endif