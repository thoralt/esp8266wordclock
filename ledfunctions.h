// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  See ledfunctions.cpp for description.
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
#ifndef _LEDFUNCTIONS_H_
#define _LEDFUNCTIONS_H_

#include <Adafruit_NeoPixel.h>
#include <stdint.h>
#include <vector>

#include "config.h"
#include "matrixobject.h"
#include "starobject.h"


typedef struct _leds_template_t
{
	int param0, param1, param2;
	const std::vector<int> LEDs;
} leds_template_t;

#define NUM_MATRIX_OBJECTS 25
#define NUM_STARS 10

class LEDFunctionsClass
{
public:
	LEDFunctionsClass();
	~LEDFunctionsClass();
	void begin(int pin);
	void process();
	void displayTime(int h, int m, int s, int ms, palette_entry palette[]);
	void set(const uint8_t *buf, palette_entry palette[]);
	void set(const uint8_t *buf, palette_entry palette[], bool immediately);
	void showHeart(bool show);
	void showStars(bool show);
	void showMatrix(bool show);
	void setBrightness(uint8_t brightness);
	void hourglass(uint8_t animationStep, bool green);

	// this mapping table maps the linear memory buffer structure used throughout the
	// project to the physical layout of the LEDs
	static const uint32_t PROGMEM mapping[NUM_PIXELS];

private:
	static const std::vector<leds_template_t> hoursTemplate;
	static const std::vector<leds_template_t> minutesTemplate;
	std::vector<MatrixObject> matrix;
	std::vector<StarObject> stars;
	uint8_t currentValues[NUM_PIXELS * 3];
	uint8_t targetValues[NUM_PIXELS * 3];
	Adafruit_NeoPixel *pixels = NULL;
	int heartBrightness = 0;
	int heartState = 0;
	int brightness = 96;
	bool doMatrix = false;
	bool doHeart = false;
	bool doStars = false;

	void show();
	void renderMatrix();
	void renderHeart();
	void renderStars();
	void fade();
	void setBuffer(uint8_t *target, const uint8_t *source, palette_entry palette[]);
};

extern LEDFunctionsClass LED;

#endif
