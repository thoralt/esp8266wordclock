// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  This module implements functions to manage the WS2812B LEDs. Two buffers contain
//  color information with current state and fade target state and are updated by
//  either simple set operations or integrated screensavers (matrix, stars, heart).
//  Also contains part of the data and logic for the hourglass animation.
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
#include "ledfunctions.h"

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
LEDFunctionsClass LED = LEDFunctionsClass();

//---------------------------------------------------------------------------------------
// variables in PROGMEM (mapping table, images)
//---------------------------------------------------------------------------------------
#include "hourglass_animation.inc"

// this mapping table maps the linear memory buffer structure used throughout the
// project to the physical layout of the LEDs
#if 1 // to allow code folding
static const uint32_t PROGMEM led_mapping[NUM_PIXELS] =
{
	10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
	11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
	32,  31,  30,  29,  28,  27,  26,  25,  24,  23,  22,
	33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,
	54,  53,  52,  51,  50,  49,  48,  47,  46,  45,  44,
	55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,
	76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,
	77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
	98,  97,  96,  95,  94,  93,  92,  91,  90,  89,  88,
	99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
	112, 111, 110, 113
};
#endif

// This defines the LED output for different minutes
// param0 controls whether the hour has to be incremented for the given minutes
// param1 is the matching minimum minute count (inclusive)
// param2 is the matching maximum minute count (inclusive)
#if 1 // to allow code folding
const std::vector<leds_template_t> minutes_template =
{
	{0,  0,  4, {107, 108, 109}},                                  // UHR
	{0,  5,  9, {7, 8, 9, 10, 40, 41, 42, 43}},                    // FÜNF NACH
	{0, 10, 14, {11, 12, 13, 14, 40, 41, 42, 43}},                 // ZEHN NACH
	{1, 15, 19, {26, 27, 28, 29, 30, 31, 32}},                     // VIERTEL
	{1, 20, 24, {11, 12, 13, 14, 18, 19, 20, 33, 34, 35, 36}},     // ZEHN VOR HALB
	{1, 25, 29, {7, 8, 9, 10, 18, 19, 20, 33, 34, 35, 36}},        // FÜNF VOR HALB
	{1, 30, 34, {33, 34, 35, 36}},                                 // HALB
	{1, 35, 39, {7, 8, 9, 10, 40, 41, 42, 43, 33, 34, 35, 36}},    // FÜNF NACH HALB
	{1, 35, 39, {7, 8, 9, 10, 40, 41, 42, 43, 33, 34, 35, 36}},    // FÜNF NACH HALB
/*	{1, 35, 39, {11, 12, 13, 14, 18, 19, 20, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 }}, // ZEHN VOR DREIVIERTEL */
	{1, 40, 44, {11, 12, 13, 14, 40, 41, 42, 43, 33, 34, 35, 36}}, // ZEHN NACH HALB
/*	{1, 40, 44, {7, 8, 9, 10, 18, 19, 20, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 }}, // FÜNF VOR DREIVIERTEL */
	{1, 45, 49, {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 }},    // DREIVIERTEL
	{1, 50, 54, {11, 12, 13, 14, 18, 19, 20}},                     // ZEHN VOR
	{1, 55, 59, {7, 8, 9, 10, 18, 19, 20}}                         // FÜNF VOR
};
#endif

// This defines the LED output for different hours
// param0 deals with special cases:
//     = 0: matches hour in param1 and param2
//     = 1: matches hour in param1 and param2 whenever minute is < 5
//     = 2: matches hour in param1 and param2 whenever minute is >= 5
// param1: hour to match
// param2: alternative hour to match
#if 1 // to allow code folding
const std::vector<leds_template_t> hours_template =
{
	{0,  0, 12, {99, 100, 101, 102, 103}}, // ZWÖLF
	{1,  1, 13, {44, 45, 46}},             // EIN
	{2,  1, 13, {44, 45, 46, 47}},         // EINS
	{0,  2, 14, {51, 52, 53, 54}},         // ZWEI
	{0,  3, 15, {55, 56, 57, 58}},         // DREI
	{0,  4, 16, {62, 63, 64, 65}},         // VIER
	{0,  5, 17, {66, 67, 68, 69}},         // FÜNF
	{0,  6, 18, {72, 73, 74, 75, 76}},     // SECHS
	{0,  7, 19, {77, 78, 79, 80, 81, 82}}, // SIEBEN
	{0,  8, 20, {84, 85, 86, 87}},         // ACHT
	{0,  9, 21, {88, 89, 90, 91}},         // NEUN
	{0, 10, 22, {92, 93, 94, 95}},         // ZEHN
	{0, 11, 23, {96, 97, 98}},             // ELF
};
#endif

// color gradient from white to green to black for matrix screen saver
#define MATRIX_GRADIENT_LENGTH 12
#if 1 // to allow code folding
static const palette_entry matrix_gradient[MATRIX_GRADIENT_LENGTH] = {
	{255, 255, 255},
	{128, 255, 128},
	{ 64, 255,  64},
	{  0, 255,   0},
	{  0, 128,   0},
	{  0,  64,   0},
	{  0,  32,   0},
	{  0,  16,   0},
	{  0,   8,   0},
	{  0,   2,   0},
	{  0,   1,   0},
	{  0,   0,   0}
};
#endif

//---------------------------------------------------------------------------------------
// randomizeStar
//
// Assigns new random coordinates and speed to a star structure. Retries until new
// coordinates have a distance of minimum 2 LEDs.
//
// -> index: Number of star to be randomized [0...NUM_STARS-1]
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::randomizeStar(int index)
{
	int retryCount = 0;
	bool distanceOK;
	int x = 0, y = 0, dx, dy;

	do
	{
		// assume distance is OK
		distanceOK = true;

		// create new random pair
		x = random(11);
		y = random(10);
		retryCount++;

		// iterate over all other stars
		for (int j = 0; j < NUM_STARS; j++)
		{
			// skip the star which is currently being randomized
			if (j == index)
				continue;

			// calculate distance
			dx = x - star_objects[j].x;
			dy = y - star_objects[j].y;
			if (dx * dx + dy * dy < 5)
			{
				// retry if distance to any star is below limit
				distanceOK = false;
				break;
			}
		}
	} while (!distanceOK && retryCount < 100);

	this->star_objects[index].x = x;
	this->star_objects[index].y = y;
	this->star_objects[index].speed = 15 + random(15);
}

//---------------------------------------------------------------------------------------
// LEDFunctionsClass
//
// Constructor, initializes data structures
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
LEDFunctionsClass::LEDFunctionsClass()
{
	// initialize matrix objects with random coordinates
	for (int i = 0; i < NUM_MATRIX_OBJECTS; i++)
	{
		this->matrix_objects[i].x = random(11);
		this->matrix_objects[i].y = random(25) - 25;
		this->matrix_objects[i].speed = MATRIX_SPEED;
		this->matrix_objects[i].count = 0;
	}

	// initialize star objects with coordinates [-10, -10] to prepare for
	// calls to randomizeStar()
	for (int i = 0; i < NUM_STARS; i++)
	{
		this->star_objects[i].x = -10;
		this->star_objects[i].y = -10;
	}

	// initialize stars with random coordinates
	for (int i = 0; i < NUM_STARS; i++)
	{
		this->randomizeStar(i);
		this->star_objects[i].count = 0;
		this->star_objects[i].state = 0;
		this->star_objects[i].brightness = random(250);
	}
}

//---------------------------------------------------------------------------------------
// begin
//
// Initializes the LED driver
//
// -> pin: hardware pin to use for WS2812B data output
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::begin(int pin)
{
	this->pixels = new Adafruit_NeoPixel(NUM_PIXELS, pin, NEO_GRB + NEO_KHZ800);
	this->pixels->begin();
}

//---------------------------------------------------------------------------------------
// hourglass
//
// Immediately displays an animation step of the hourglass animation.
// ATTENTION: Animation frames must start at 32 bit boundary each!
//
// -> animationStep: Number of current frame [0...HOURGLASS_ANIMATION_FRAMES]
//    green: Flag to switch the palette color 3 to green instead of yellow (used in the
//           second half of the hourglass animation to indicate the short wait-for-OTA
//           window)
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::hourglass(uint8_t animationStep, bool green)
{
	// colors in palette: black, white, yellow
	palette_entry p[] = {{0, 0, 0}, {255, 255, 255}, {255, 255, 0}, {255, 255, 0}};

	if(green) p[3].r = 0;

	// safety check
	if (animationStep < HOURGLASS_ANIMATION_FRAMES)
	{
		this->set(hourglass_animation[animationStep], p, true);
	}
	else
	{
		this->set(hourglass_animation[0], p, true);
	}
}

//---------------------------------------------------------------------------------------
// process
//
// Drives internal data flow, should be called repeatedly from main loop()
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::process()
{
	// check for flags to display matrix, heart or stars
	if (this->doMatrix)
	{
		this->matrix();
	}
	else if (this->doHeart)
	{
		this->heart();
	}
	else if (this->doStars)
	{
		this->stars();
	}
	else
	{
		// no special fx -> do normal fading and display current state
		this->fade();
		this->show();
	}
}

//---------------------------------------------------------------------------------------
// setBrightness
//
// Sets the brightness for the WS2812 values. Will be multiplied with each color
// component when sending data to WS2812.
//
// -> brightness: [0...255]
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::setBrightness(uint8_t brightness)
{
	this->brightness = brightness;
}

//#define USE_BUGFIX

//---------------------------------------------------------------------------------------
// displayTime
//
// Loads internal buffers with the given time representation, starts fading to this new
// value
//
// -> h, m, s, ms: Current time
//	palette: Array of 3 palette entries (background, foreground, seconds progress)
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::displayTime(int h, int m, int s, int ms,
		palette_entry palette[])
{
	// buffer to hold pixel states as palette indexes
	uint8_t buf[NUM_PIXELS];

	// initialize the buffer with either background (=0) or seconds progress (=2)
	// part of the background will be illuminated with color 2 depending on current
	// seconds/milliseconds value, whole screen is backlit when seconds = 59
	int pos = s * 1000 + ms;
	pos *= 110;
	pos /= 60000;
	pos++;
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		if (i < pos)
			buf[i] = 2;
		else
			buf[i] = 0;
	}

	// set static LEDs
	buf[0 * 11 + 0] = 1; // E
	buf[0 * 11 + 1] = 1; // S

	buf[0 * 11 + 3] = 1; // I
	buf[0 * 11 + 4] = 1; // S
	buf[0 * 11 + 5] = 1; // T

	// minutes 1...4 for the corners
	for(int i=0; i<=((m%5)-1); i++) buf[10 * 11 + i] = 1;

	int adjust_hour = 0;
	for(leds_template_t t : minutes_template)
	{
		if(m >= t.param1 && m <= t.param2)
		{
			for(int i: t.LEDs) buf[i] = 1;
			adjust_hour = t.param0;
			break;
		}
	}

	h += adjust_hour;
	if (h > 23)	h -= 24;

	for(leds_template_t t : hours_template)
	{
		if((t.param1 == h || t.param2 == h) &&
		   ((t.param0 == 1 && m < 5) ||  // special case full hour
			(t.param0 == 2 && m >= 5) || // special case hour + minutes
			(t.param0 == 0)))            // normal case
		{
			for(int i: t.LEDs) buf[i] = 1;
			break;
		}
	}

	// set the new values as target for fade operation
	this->set(buf, palette);
}

//---------------------------------------------------------------------------------------
// set
//
// Sets the internal LED buffer to new values based on an indexed source buffer and an
// associated palette. Does not display colors immediately, fades to new colors instead.
//
// Attention: If buf is PROGMEM, make sure it is aligned at 32 bit and its size is
// a multiple of 4 bytes!
//
// -> buf: indexed source buffer
//	palette: color definition for source buffer
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::set(const uint8_t *buf, palette_entry palette[])
{
	this->set(buf, palette, false);
}

//---------------------------------------------------------------------------------------
// set
//
// Sets the internal LED buffer to new values based on an indexed source buffer and an
// associated palette.
//
// Attention: If buf is PROGMEM, make sure it is aligned at 32 bit and its size is
// a multiple of 4 bytes!
//
// -> buf: indexed source buffer
//	palette: color definition for source buffer
//	immediately: if true, display buffer immediately; fade to new colors if false
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::set(const uint8_t *buf, palette_entry palette[],
		bool immediately)
{
	this->setBuffer(this->targetValues, buf, palette);

	if (immediately)
	{
		this->setBuffer(this->currentValues, buf, palette);
		this->show();
	}
}

//---------------------------------------------------------------------------------------
// setBuffer
//
// Fills a buffer (e. g. this->targetValues) with color data based on indexed source
// pixels and a palette. Pays attention to 32 bit boundaries, so use with PROGMEM is
// safe.
//
// -> target: color buffer, e. g. this->targetValues or this->currentValues
//	source: buffer with color indexes
//	palette: colors for indexed source buffer
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::setBuffer(uint8_t *target, const uint8_t *source,
		palette_entry palette[])
{
	uint32_t mapping, palette_index;

	// cast source to 32 bit pointer to ensure 32 bit aligned access
	uint32_t *buf = (uint32_t*) source;
	// this holds the current 4 bytes
	uint32_t currentDWord;
	// this is a pointer to the current 4 bytes for access as single bytes
	uint8_t *currentBytes = (uint8_t*) &currentDWord;
	// this counts bytes from 0...3
	uint32_t byteCounter = 0;
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		// get next 4 bytes
		if (byteCounter == 0)
			currentDWord = buf[i >> 2];

		mapping = led_mapping[i] * 3;
		palette_index = currentBytes[byteCounter];
		target[mapping + 0] = palette[palette_index].r;
		target[mapping + 1] = palette[palette_index].g;
		target[mapping + 2] = palette[palette_index].b;

		if (++byteCounter > 3)
			byteCounter = 0;
	}
}

//---------------------------------------------------------------------------------------
// fade
//
// Fade one step of the color values from this->currentValues[i] to 
// this->targetValues[i]. Uses non-linear fade speed depending on distance to target
// value.
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::fade()
{
	int delta;
	for (int i = 0; i < NUM_PIXELS * 3; i++)
	{
		delta = this->targetValues[i] - this->currentValues[i];
		if (delta > 64)
			this->currentValues[i] += 8;
		else if (delta > 16)
			this->currentValues[i] += 4;
		else if (delta > 0)
			this->currentValues[i]++;
		else if (delta < -64)
			this->currentValues[i] -= 8;
		else if (delta < -16)
			this->currentValues[i] -= 4;
		else if (delta < 0)
			this->currentValues[i]--;
	}
}

//---------------------------------------------------------------------------------------
// show
//
// Internal method, copies this->currentValues to WS2812 object while applying brightness
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::show()
{
	uint8_t *data = this->currentValues;

	// copy current color values to LED object and display it
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		this->pixels->setPixelColor(i,
				pixels->Color(((int) data[i * 3 + 0] * this->brightness) >> 8,
						((int) data[i * 3 + 1] * this->brightness) >> 8,
						((int) data[i * 3 + 2] * this->brightness) >> 8));
	}
	this->pixels->show();
}

//---------------------------------------------------------------------------------------
// showHeart
//
// Sets internal flag for showing the heart animation
//
// -> 
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::showHeart(bool show)
{
	this->doHeart = show;
}

//---------------------------------------------------------------------------------------
// showStars
//
// Sets internal flag for showing the stars animation
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::showStars(bool show)
{
	this->doStars = show;
}

//---------------------------------------------------------------------------------------
// showMatrix
//
// Sets internal flag for showing the matrix animation
//
// -> 
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::showMatrix(bool show)
{
	this->doMatrix = show;
}

//---------------------------------------------------------------------------------------
// matrix
//
// Renders one frame of the matrix animation and displays it immediately
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::matrix()
{
	uint8_t *buf = this->currentValues;

	// sort by y coordinate for correct overlapping
	matrix_object temp;
	for (int i = 0; i < NUM_MATRIX_OBJECTS; i++)
	{
		for (int j = i + 1; j < NUM_MATRIX_OBJECTS; j++)
		{
			if (matrix_objects[i].y < matrix_objects[j].y)
			{
				temp = matrix_objects[i];
				matrix_objects[i] = matrix_objects[j];
				matrix_objects[j] = temp;
			}
		}
	}

	// clear buffer
	for (int i = 0; i < NUM_PIXELS * 3; i++)
		buf[i] = 0;

	// iterate over all matrix objects
	for (int i = 0; i < NUM_MATRIX_OBJECTS; i++)
	{
		// check if current matrix object has left the screen
		if (matrix_objects[i].y - MATRIX_GRADIENT_LENGTH > 10)
		{
			// recreate object with new x, y and speed
			matrix_objects[i].x = random(11);
			matrix_objects[i].y = random(25) - 25;
			matrix_objects[i].speed = MATRIX_SPEED;
			matrix_objects[i].count = 0;
		}
		else
		{
			// update position
			matrix_objects[i].count += matrix_objects[i].speed;
			if (matrix_objects[i].count > 30000)
			{
				matrix_objects[i].count -= 30000;
				matrix_objects[i].y++;
			}
		}

		// draw the gradient of one matrix object
		int y = matrix_objects[i].y;
		int x = matrix_objects[i].x;
		for (int j = 0; j < MATRIX_GRADIENT_LENGTH; j++)
		{
			int ofs = led_mapping[x + y * 11] * 3;
			if (y >= 0 && y < 10)
			{
				buf[ofs + 0] = matrix_gradient[j].r;
				buf[ofs + 1] = matrix_gradient[j].g;
				buf[ofs + 2] = matrix_gradient[j].b;
			}
			y--;
		}
	}
	this->show();
}

//---------------------------------------------------------------------------------------
// stars
//
// Renders one frame of the stars animation and displays it immediately
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::stars()
{
	uint8_t *buf = this->currentValues;

	// clear buffer
	for (int i = 0; i < NUM_PIXELS * 3; i++)
		buf[i] = 0;

	int x, y, b, offset;
	for (int i = 0; i < NUM_STARS; i++)
	{
		// fetch coordinates and brightness of current star
		x = this->star_objects[i].x;
		y = this->star_objects[i].y;
		b = this->star_objects[i].brightness;
		if(b < 0) b = 0;
		if(b > 255) b = 255;

		// write brightness to target buffer
		offset = led_mapping[x + y * 11] * 3;
		buf[offset + 0] = b;
		buf[offset + 1] = b;
		buf[offset + 2] = b;

		// increase or decrease brightness depending on current state
		if (this->star_objects[i].state == 0)
		{
			this->star_objects[i].brightness += this->star_objects[i].speed;
			if (this->star_objects[i].brightness >= 255)
			{
				// switch to decreasing mode
				this->star_objects[i].brightness = 255;
				this->star_objects[i].state = 1;
			}
		}
		else
		{
			this->star_objects[i].brightness -= this->star_objects[i].speed;
			if (this->star_objects[i].brightness < -2000)
			{
				// switch to increasing mode and get new random coordinates
				this->star_objects[i].brightness = 0;
				this->star_objects[i].state = 0;
				this->randomizeStar(i);
			}
		}
	}

	// send current values to LEDs
	this->show();
}

//---------------------------------------------------------------------------------------
// heart
//
// Renders one frame of the heart animation and displays it immediately
//
// -> --
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::heart()
{
	palette_entry palette[2];
	uint8_t heart[] = {
		0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1
	};
	palette[0] = {0, 0, 0};
	palette[1] = {(uint8_t)this->heartBrightness, 0, 0};
	this->set(heart, palette, true);

	switch (this->heartState)
	{
	case 0:
		if (this->heartBrightness >= 255) this->heartState = 1;
		else this->heartBrightness += 32;
		break;

	case 1:
		if (this->heartBrightness < 128) this->heartState = 2;
		else this->heartBrightness -= 32;
		break;

	case 2:
		if (this->heartBrightness >= 255) this->heartState = 3;
		else this->heartBrightness += 32;
		break;

	case 3:
	default:
		if (this->heartBrightness <= 0) this->heartState = 0;
		else this->heartBrightness -= 4;
		break;
	}

	if (this->heartBrightness > 255) this->heartBrightness = 255;
	if (this->heartBrightness < 0) this->heartBrightness = 0;
}
