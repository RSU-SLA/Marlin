#include "dac_ad5663r.h"


ad5663r::ad5663r() {}

MarlinSPI ad5663r::mySPI(DAC_MOSI_PIN, NC, DAC_SCK_PIN, PB10);
void ad5663r::setValue2(const uint8_t channel, const uint8_t value)
{
    uint8_t packet[3];
    packet[0] = 0b00000111;
    packet[1] = 0xF;
    packet[2] = 0xF;

  SERIAL_ECHO_MSG(">>>>>ad5663r DAC setValue");
  OUT_WRITE(DAC_CS_PIN, LOW);
    for (uint8_t data : packet)
        mySPI.transfer(data);
  OUT_WRITE(DAC_CS_PIN, HIGH);
    //const uint8_t returnByte = mySPI.transfer(value);
  //return returnByte; 
}
void ad5663r::init2(uint8_t spiRate) {

  SERIAL_ECHO_MSG(">>>>>ad5663r DAC init");
  SET_OUTPUT(DAC_CS_PIN);
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
  switch (spiRate) {
    case SPI_FULL_SPEED:    clock = SPI_CLOCK_MAX;  break;
    case SPI_HALF_SPEED:    clock = SPI_CLOCK_DIV4; break;
    case SPI_QUARTER_SPEED: clock = SPI_CLOCK_DIV8; break;
    case SPI_EIGHTH_SPEED:  clock = SPI_CLOCK_DIV16; break;
    case SPI_SPEED_5:       clock = SPI_CLOCK_DIV32; break;
    case SPI_SPEED_6:       clock = SPI_CLOCK_DIV64; break;
    default:                clock = SPI_CLOCK_DIV2;// Default from the SPI library
  }

  mySPI.setClockDivider(SPI_CLOCK_DIV64);
  mySPI.setBitOrder(MSBFIRST);
  mySPI.setDataMode(SPI_MODE2);
  mySPI.begin();
__HAL_RCC_SPI2_CLK_ENABLE();

  OUT_WRITE(DAC_CS_PIN, LOW);
    uint8_t packet[3];
    packet[0] = 0b00111111;
    packet[1] = 0xF;
    packet[2] = 0xF;
  mySPI.transfer(0b00111111);
  mySPI.transfer(0x0F);
  mySPI.transfer(0x01);
  OUT_WRITE(DAC_CS_PIN, HIGH);
  SERIAL_ECHO_MSG(">>>>>ad5663r DAC init end");
}

#include "pinconfig.h"

uint16_t delta(uint16_t a, uint16_t b) { return a > b ? a - b : b - a; }

SPI_HandleTypeDef ad5663r::SPIx;


uint8_t ad5663r::HardwareIO(uint8_t data) {
  //SERIAL_ECHO_MSG("Hardware IO");
  __HAL_SPI_ENABLE(&SPIx);
  while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}
  SPIx.Instance->DR = data;
  while ((SPIx.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE) {}
  __HAL_SPI_DISABLE(&SPIx);

  return SPIx.Instance->DR;
}

uint8_t ad5663r::HardwareIO(uint8_t *data)
{
  __HAL_SPI_ENABLE(&SPIx);
  for (int i = 0; i < 3; ++i)
  {
    while ((SPIx.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE) {}
    SPIx.Instance->DR = data[i];
    //while ((SPIx.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE) {}
  }
  while (__HAL_SPI_GET_FLAG(&SPIx, SPI_FLAG_BSY)) {}
    __HAL_SPI_DISABLE(&SPIx);

  return SPIx.Instance->DR;
  
}

uint8_t ad5663r::SoftwareIO(uint8_t data) {
    
  SERIAL_ECHO_MSG("Software IO");
  uint8_t result = 0;

  return result;
}

void ad5663r::init(uint8_t) {
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
float frequencies[] = {1e2, 1e3, 1e1, 1e1};
float phases[] = {0, 90, 180, 270};
float offsets[] = {32768, 32768, 32768, 32768};
float amplitudes[] = {12500, 25000, 32600, 32600};

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

void ad5663r::setValue(const uint8_t channel, const uint16_t value)
{
    static uint16_t lastData[2] = {0,0};
        if (lastData[channel] == value)
        {
          //SERIAL_ERROR_MSG("SAME");
          return;
        }
        //SERIAL_ERROR_MSG("setValue channel: ", channel, " val : ", value);
        

    lastData[channel] = value;
    DataTransferBegin();
    uint8_t datax[3];
    datax[0] = 0b00011000;
    datax[0] |= channel;
    datax[1] = (value >> 8) & 0xFF;
    datax[2] = value & 0xFF;
    HardwareIO(datax);
    
    DataTransferEnd();
/*

  DataTransferBegin();
  IO(0x18);
  IO(0xFB);
  IO(0x16);
  IO(0x19);
  IO(0xDB);
  IO(0xA1);
  DataTransferEnd();
  */

}