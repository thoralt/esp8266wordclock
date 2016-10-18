// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  See brightness.cpp for description.
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
#ifndef _BRIGHTNESS_H_
#define _BRIGHTNESS_H_

#include <stdint.h>

#define ALPHA(x) ((uint16_t)(x * 65535.0f))
#define FILTER_COEFFICIENT ALPHA(0.01)

class BrightnessClass
{
public:
	BrightnessClass();
	uint32_t value();

	uint32_t avg = 0;
	uint32_t brightnessOverride = 256;

private:
	uint32_t getBrightnessForADCValue(uint32_t adcValue);
	uint32_t filter(uint16_t input);
};

extern BrightnessClass Brightness;

#endif

