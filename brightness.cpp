// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  This class reads the analog input with an attached light dependent resistor
//  (pullup to VCC, LDR to ground), filters the resulting ADC values with an
//  moving average filter and calculates a brightness value using an interpolation
//  on a curve defined by several points.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	this->avg = analogRead(A0);
}

//---------------------------------------------------------------------------------------
// filter
//
// Exponential moving average filter using integer arithmetics. Implements the
// following behaviour:
//
//   output = (1-coeff)*last_output + coeff*input
//
// FILTER_COEFFICIENT is defined as fixed point number with 16.16 bits resolution.
//
// -> input: filter input [0...65535]
// <- --
//---------------------------------------------------------------------------------------
uint32_t BrightnessClass::filter(uint16_t input)
{
	uint32_t tmp = (65536 - FILTER_COEFFICIENT)
			* this->avg + (uint32_t)input*FILTER_COEFFICIENT;
	return (tmp + 32768) >> 16;
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
	while (lightTable[i] != -1)
	{
		if (adcValue >= lightTable[i] && adcValue < lightTable[i + 2])
		{
			float x1 = lightTable[i + 0];
			float y1 = lightTable[i + 1];
			float x2 = lightTable[i + 2];
			float y2 = lightTable[i + 3];
			result = y1 + (value - x1) * (y2 - y1) / (x2 - x1);
			return (int) result;
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
	// read next ADC value and apply filter
	this->avg = this->filter(analogRead(A0));

	// calculate brightness value for filtered ADC value
	return this->getBrightnessForADCValue(this->avg);
}
