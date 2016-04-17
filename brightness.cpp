#include <Arduino.h>
#include "brightness.h"

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
BrightnessClass Brightness = BrightnessClass();

//---------------------------------------------------------------------------------------
// BrightnessClass
//
// Constructor, initializes filter with current ADC value
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
BrightnessClass::BrightnessClass()
{
    int adc = analogRead(A0);
    for(int i=0; i<NUM_ADC_VALUES; i++) adcValues[i] = adc;
    this->adcAvg = adc;
}

//---------------------------------------------------------------------------------------
// getBrightnessForADCValue
//
// Calculates a brightness value for a given ADC value using a table and interpolation.
//
// -> adcValue: input value [0...1023]
// <- brightness [0...255]
//---------------------------------------------------------------------------------------
uint32_t BrightnessClass::getBrightnessForADCValue(uint32_t adcValue)
{
    float lightTable[] = {
        0, 160, 
        300, 96,
        680, 64, 
        800, 32, 
        900, 15, 
        1023, 15, 
        -1, -1
    };
    
    int i = 0;
    float result;
    float value = adcValue;
    while(lightTable[i] != -1)
    {
        if(adcValue >= lightTable[i] && adcValue < lightTable[i+2])
        {
            float x1 = lightTable[i+0];
            float y1 = lightTable[i+1];
            float x2 = lightTable[i+2];
            float y2 = lightTable[i+3];
            result = y1 + (value - x1) * (y2 - y1) / (x2 - x1);
            return (int)result;
        }
        i += 2;
    }
    
    return 0;
}

//---------------------------------------------------------------------------------------
// value
//
// Triggers a new sample to be read from light sensor, returns value from average filter
//
// -> --
// <- low pass filtered brightness value
//---------------------------------------------------------------------------------------
uint32_t BrightnessClass::value()
{
    int adc = analogRead(A0);
    for(int i=1; i<NUM_ADC_VALUES; i++) adcValues[i-1] = adcValues[i];
    adcValues[NUM_ADC_VALUES-1] = adc;
    
    int avg = 0;
    for(int i=0; i<NUM_ADC_VALUES; i++) avg += adcValues[i];
    avg /= NUM_ADC_VALUES;
    this->adcAvg = avg;
    return this->getBrightnessForADCValue(avg);
}
