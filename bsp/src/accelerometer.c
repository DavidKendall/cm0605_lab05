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
 **************************************************************************/
#include <assert.h>
#include <stdint.h>
#include <board.h>
#include <adc.h>
#include <accelerometer.h>



/* accelerometerInit()
 *
 * Initialise the ADC associated with the accelerometer on the LPC2378
 *
 * Must be called prior to accelerometerRead()
 */
void accelerometerInit( void ) {
   adcInit(X_CHANNEL);
   adcInit(Y_CHANNEL);
   adcInit(Z_CHANNEL);
}

/* Read the ADC to determine an accelerometer X-axis value
 * Returns a 10-bit value in {0..1023} or -1 for a parameter error
 */
uint32_t accelerometerRead(uint32_t channel ) {
  uint32_t value;
   
  assert((channel == X_CHANNEL) || (channel == Y_CHANNEL) || (channel == Z_CHANNEL));
   
  switch ( channel ) {
  case X_CHANNEL:
    value = adcGetState(AIN1);
    break;
  case Y_CHANNEL:
    value = adcGetState(AIN0);
    break;
  case Z_CHANNEL:
    value = adcGetState(AIN6);
    break;
  default:
    value = 0;
  }
  return value; 
}
