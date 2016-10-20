#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>

#define AIN0 0
#define AIN1 1
#define AIN2 2
#define AIN3 3
#define AIN4 4
#define AIN5 5
#define AIN6 6


void adcInitSound(void);
void adcInit(uint32_t channel);
uint32_t adcGetState(uint32_t channel);

#endif