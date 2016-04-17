#include <stdint.h>

#ifndef _BRIGHTNESS_H_
#define _BRIGHTNESS_H_

#define NUM_ADC_VALUES 256

class BrightnessClass
{
public:
    BrightnessClass();
    uint32_t value();

    uint32_t adcAvg = 0;
    
private:
    int adcValues[NUM_ADC_VALUES];
    uint32_t getBrightnessForADCValue(uint32_t adcValue);
};

extern BrightnessClass Brightness;

#endif


