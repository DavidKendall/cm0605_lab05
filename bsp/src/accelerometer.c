/*************************************************************************
 *
 *    File name   : accelerometer.c
 *    Description : Configures and LPC2378 ADC to read accelerometer
 *                  values
 *
 *    History :
 *    1. Date        : November 24, 2009
 *       Author      : WDH
 *       Description : Create
 *
 *       Modified    : DK, 25-11-11
*                    : DK, 30-05-13; added median filter
 *
 **************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <median_filter.h>
#include <adc.h>
#include <accelerometer.h>

static medianFilter_t fx;
static medianFilter_t fy;
static medianFilter_t fz;

static uint32_t oldx;
static uint32_t oldy;
static uint32_t oldz;

/* accelerometerInit()
 *
 * Initialise the ADC associated with the accelerometer on the LPC2378
 *
 * Must be called prior to accelerometerRead()
 */
void accelerometerInit( void ) {
  adcChannelInit((adcChannel_t)ACCEL_X);
  adcChannelInit((adcChannel_t)ACCEL_Y);
  adcChannelInit((adcChannel_t)ACCEL_Z);
  
  oldx = accelerometerReadRaw(ACCEL_X);
  oldy = accelerometerReadRaw(ACCEL_Y);
  oldz = accelerometerReadRaw(ACCEL_Z);
  medianFilterInit(&fx, oldx);
  medianFilterInit(&fy, oldy);
  medianFilterInit(&fz, oldz);

}

/* Read the ADC to determine an accelerometer X-axis value
 * Returns a 10-bit value in {0..1023}
 */
uint32_t accelerometerReadRaw(accelerometerChannel_t channel ) {
  return adcGetState((adcChannel_t)channel);
}

/* Read the ADC to determine an accelerometer X-axis value
 * Returns a 10-bit value in {0..1023}. 
 * Median-filter, followed by exponential decay low-pass filter with
 * alpha = 0.25
 */
uint32_t accelerometerRead(accelerometerChannel_t channel ) {
  uint32_t result;
  switch (channel) {
    case ACCEL_X: {
      result = (oldx >> 2) + (3 * (medianFilterAdd(&fx, accelerometerReadRaw(ACCEL_X)) >> 2)); 
      oldx = result;
      break;
    }
    case ACCEL_Y: {
      result = (oldy >> 2) + (3 * (medianFilterAdd(&fy, accelerometerReadRaw(ACCEL_Y)) >> 2)); 
      oldy = result;
      break;
    }
    case ACCEL_Z: {
      result = (oldz >> 2) + (3 * (medianFilterAdd(&fz, accelerometerReadRaw(ACCEL_Z)) >> 2)); 
      oldz = result;
      break;
    }
    default: {
      while (true) { /* should not be here */ }
      break;
    }
  }
  return result;
}
