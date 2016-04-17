#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "config.h"

#ifndef _LEDFUNCTIONS_H_
#define _LEDFUNCTIONS_H_

#define NUM_PIXELS 114

#define HOURGLASS_ANIMATION_FRAMES 8
#define MATRIX_SPEED 3500
#define NUM_MATRIX_OBJECTS 25
typedef struct _matrix_object
{
    int32_t x;
    int32_t y;
    int32_t speed, count;
} matrix_object;

#define NUM_STARS 10
typedef struct _stars_object
{
	int32_t x;
	int32_t y;
	int32_t speed, count;
	int32_t brightness;
	int32_t state;
} stars_object;

class LEDFunctionsClass
{
public:
    LEDFunctionsClass();
    void begin(int pin);
    void process();
    void displayTime(int h, int m, int s, int ms, palette_entry palette[]);
    void set(const uint8_t *buf, palette_entry palette[]);
    void set(const uint8_t *buf, palette_entry palette[], bool immediately);
    void showHeart(bool show);
    void showStars(bool show);
    void showMatrix(bool show);
    void setBrightness(uint8_t brightness);
    void hourglass(uint8_t animationStep);
    
private:
    matrix_object matrix_objects[NUM_MATRIX_OBJECTS];
    stars_object star_objects[NUM_STARS];
    uint8_t currentValues[NUM_PIXELS * 3];
    uint8_t targetValues[NUM_PIXELS * 3];
    Adafruit_NeoPixel *pixels = NULL;
    int heartBrightness = 0;
    int heartState = 0;
    int brightness = 96;
    bool doMatrix = false;
    bool doHeart = false;
    bool doStars = false;

    void randomizeStar(int index);
    void show();
    void matrix();
    void heart();
    void stars();
    void fade();
    void setBuffer(uint8_t *target, const uint8_t *source, palette_entry palette[]);
};

extern LEDFunctionsClass LED;

#endif


