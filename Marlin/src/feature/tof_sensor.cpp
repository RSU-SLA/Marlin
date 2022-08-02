#include "../inc/MarlinConfig.h"

#if ENABLED(I2C_TOF_SENSOR)
#include "tof_sensor.h"
#include <Wire.h>
#include <VL6180X.h>

TofSensor tofSensor;

float TofSensor::range;
float TofSensor::curRange;

VL6180X sensor;

void TofSensor::init()
{
  //Wire.begin();

  SERIAL_ECHO_MSG("TOF SENSOR INIT");
  sensor.init();
  sensor.configureDefault();

  // Reduce range max convergence time and ALS integration
  // time to 30 ms and 50 ms, respectively, to allow 10 Hz
  // operation (as suggested by table "Interleaved mode
  // limits (10 Hz operation)" in the datasheet).
  sensor.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
  sensor.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);

  sensor.setTimeout(500);

   // stop continuous mode if already active
  sensor.stopContinuous();
  // in case stopContinuous() triggered a single-shot
  // measurement, wait for it to complete
  delay(300);
  // start interleaved continuous mode with period of 100 ms
  sensor.startInterleavedContinuous(100);
}

void TofSensor::read()
{
  SERIAL_ECHO_MSG("TOF SENSOR READ");
  Serial.print("Ambient: ");
  Serial.print(sensor.readAmbientContinuous());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.print("\tRange: ");
  Serial.print(sensor.readRangeContinuousMillimeters());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
#endif
