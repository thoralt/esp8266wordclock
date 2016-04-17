#include "ledfunctions.h"

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
LEDFunctionsClass LED = LEDFunctionsClass();

//---------------------------------------------------------------------------------------
// variables in PROGMEM (mapping table, images)
//---------------------------------------------------------------------------------------
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

#define MATRIX_GRADIENT_LENGTH 12
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

static const uint8_t PROGMEM hourglass_animation[HOURGLASS_ANIMATION_FRAMES][NUM_PIXELS+2] = {
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 2, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0},
    {   0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 0, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 2, 2, 2, 0, 1, 0, 0,
        0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        2, 2, 2, 2, 0, 0}
};

void LEDFunctionsClass::randomizeStar(int index)
{
	int retryCount = 0;
	bool distanceOK;
	float dx, dy;
	int x=0, y=0;

	do
	{
		distanceOK = true;
    	x = random(11);
    	y = random(10);
		retryCount++;

		for(int j=0; j<NUM_STARS; j++)
		{
			if(j==index) continue;
			dx = x - star_objects[j].x;
			dy = y - star_objects[j].y;
			if(sqrt(dx*dx + dy*dy) < 2.0f)
			{
				distanceOK = false;
				break;
			}
		}
	}
	while(!distanceOK && retryCount < 100);

	this->star_objects[index].x = x;
	this->star_objects[index].y = y;
	this->star_objects[index].speed = 1 + random(3);
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
    for(int i=0; i<NUM_MATRIX_OBJECTS; i++)
    {
        this->matrix_objects[i].x = random(11);
        this->matrix_objects[i].y = random(25) - 25;
        this->matrix_objects[i].speed = MATRIX_SPEED;
        this->matrix_objects[i].count = 0;
    }

    for(int i=0; i<NUM_STARS; i++)
    {
    	this->star_objects[i].x = -10;
    	this->star_objects[i].y = -10;
    }

    for(int i=0; i<NUM_STARS; i++)
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
// Immediately displays an animation step of the hourglass animation
//
// -> animationStep: Number of current frame [0...HOURGLASS_ANIMATION_FRAMES]
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::hourglass(uint8_t animationStep)
{
    palette_entry p[] = {{0, 0, 0}, {255, 255, 255}, {255, 255, 0}};
    if(animationStep < HOURGLASS_ANIMATION_FRAMES)
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
    if(this->doMatrix)
    {
        this->matrix();
    }
    else if(this->doHeart)
    {
        this->heart();
    }
    else if(this->doStars)
    {
        this->stars();
    }
    else
    {
        this->fade();
        this->show();
    }
}

//---------------------------------------------------------------------------------------
// setBrightness
//
// Sets the brightness for the WS2812 values
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
//    palette: Array of 3 palette entries (background, foreground, seconds progress)
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::displayTime(int h, int m, int s, int ms, palette_entry palette[])
{
    uint8_t buf[NUM_PIXELS];
    int pos = s * 1000 + ms;
    pos *= 110;
    pos /= 60000;
    pos++;
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (i < pos) buf[i] = 2; else buf[i] = 0;
    }
    
    buf[0 * 11 + 0] = 1; // E
    buf[0 * 11 + 1] = 1; // S

    buf[0 * 11 + 3] = 1; // I
    buf[0 * 11 + 4] = 1; // S
    buf[0 * 11 + 5] = 1; // T
    
    // minutes 1...4 for the corners
    if ((m % 5) == 1)
    {
        buf[10 * 11 + 0] = 1;
    }
    else if ((m % 5) == 2)
    {
        buf[10 * 11 + 0] = 1;
        buf[10 * 11 + 1] = 1;
    }
    else if ((m % 5) == 3)
    {
        buf[10 * 11 + 0] = 1;
        buf[10 * 11 + 1] = 1;
        buf[10 * 11 + 2] = 1;
    }
    else if ((m % 5) == 4)
    {
        buf[10 * 11 + 0] = 1;
        buf[10 * 11 + 1] = 1;
        buf[10 * 11 + 2] = 1;
        buf[10 * 11 + 3] = 1;
    }
    
    // Minuten
    if (m < 5)
    {
        buf[9 * 11 + 8] = 1;  // U
        buf[9 * 11 + 9] = 1;  // H
        buf[9 * 11 + 10] = 1; // R
    }
    else if (m < 10)
    {
        buf[0 * 11 + 7] = 1;  // F
        buf[0 * 11 + 8] = 1;  // Ü
        buf[0 * 11 + 9] = 1;  // N
        buf[0 * 11 + 10] = 1; // F
        
        buf[3 * 11 + 7] = 1;  // N
        buf[3 * 11 + 8] = 1;  // A
        buf[3 * 11 + 9] = 1;  // C
        buf[3 * 11 + 10] = 1; // H
    }
    else if (m < 15)
    {
        buf[1 * 11 + 0] = 1;  // Z
        buf[1 * 11 + 1] = 1;  // E
        buf[1 * 11 + 2] = 1;  // H
        buf[1 * 11 + 3] = 1;  // N
        
        buf[3 * 11 + 7] = 1;  // N
        buf[3 * 11 + 8] = 1;  // A
        buf[3 * 11 + 9] = 1;  // C
        buf[3 * 11 + 10] = 1; // H
    }
    else if (m < 20)
    {
        buf[2 * 11 + 4] = 1;  // V
        buf[2 * 11 + 5] = 1;  // I
        buf[2 * 11 + 6] = 1;  // E
        buf[2 * 11 + 7] = 1;  // R
        buf[2 * 11 + 8] = 1;  // T
        buf[2 * 11 + 9] = 1;  // E
        buf[2 * 11 + 10] = 1; // L
        h++;
    }
    else if (m < 25)
    {
        buf[1 * 11 + 0] = 1;  // Z
        buf[1 * 11 + 1] = 1;  // E
        buf[1 * 11 + 2] = 1;  // H
        buf[1 * 11 + 3] = 1;  // N

        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        
        buf[3 * 11 + 0] = 1;  // H
        buf[3 * 11 + 1] = 1;  // A
        buf[3 * 11 + 2] = 1;  // L
        buf[3 * 11 + 3] = 1;  // B
        h++;
    }
    else if (m < 30)
    {
        buf[0 * 11 + 7] = 1;  // F
        buf[0 * 11 + 8] = 1;  // Ü
        buf[0 * 11 + 9] = 1;  // N
        buf[0 * 11 + 10] = 1; // F
        
        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        
        buf[3 * 11 + 0] = 1;  // H
        buf[3 * 11 + 1] = 1;  // A
        buf[3 * 11 + 2] = 1;  // L
        buf[3 * 11 + 3] = 1;  // B
        h++;
    }
    else if (m < 35)
    {
        buf[3 * 11 + 0] = 1;  // H
        buf[3 * 11 + 1] = 1;  // A
        buf[3 * 11 + 2] = 1;  // L
        buf[3 * 11 + 3] = 1;  // B
        h++;
    }
    else if (m < 40)
    {
#ifdef USE_BUGFIX
        buf[1 * 11 + 0] = 1;  // Z
        buf[1 * 11 + 1] = 1;  // E
        buf[1 * 11 + 2] = 1;  // H
        buf[1 * 11 + 3] = 1;  // N
        
        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        
        buf[2 * 11 + 0] = 1;  // D
        buf[2 * 11 + 1] = 1;  // R
        buf[2 * 11 + 2] = 1;  // E
        buf[2 * 11 + 3] = 1;  // I
        buf[2 * 11 + 4] = 1;  // V
        buf[2 * 11 + 5] = 1;  // I
        buf[2 * 11 + 6] = 1;  // E
        buf[2 * 11 + 7] = 1;  // R
        buf[2 * 11 + 8] = 1;  // T
        buf[2 * 11 + 9] = 1;  // E
        buf[2 * 11 + 10] = 1; // L
#else
		buf[0 * 11 + 7] = 1;  // F
		buf[0 * 11 + 8] = 1;  // Ü
		buf[0 * 11 + 9] = 1;  // N
		buf[0 * 11 + 10] = 1; // F

		buf[3 * 11 + 7] = 1;  // N
		buf[3 * 11 + 8] = 1;  // A
		buf[3 * 11 + 9] = 1;  // C
		buf[3 * 11 + 10] = 1; // H

		buf[3 * 11 + 0] = 1;  // H
		buf[3 * 11 + 1] = 1;  // A
		buf[3 * 11 + 2] = 1;  // L
		buf[3 * 11 + 3] = 1;  // B
#endif
        h++;
    }
    else if (m < 45)
    {
#ifdef USE_BUGFIX
        buf[0 * 11 + 7] = 1;  // F
        buf[0 * 11 + 8] = 1;  // Ü
        buf[0 * 11 + 9] = 1;  // N
        buf[0 * 11 + 10] = 1; // F
        
        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        
        buf[2 * 11 + 0] = 1;  // D
        buf[2 * 11 + 1] = 1;  // R
        buf[2 * 11 + 2] = 1;  // E
        buf[2 * 11 + 3] = 1;  // I
        buf[2 * 11 + 4] = 1;  // V
        buf[2 * 11 + 5] = 1;  // I
        buf[2 * 11 + 6] = 1;  // E
        buf[2 * 11 + 7] = 1;  // R
        buf[2 * 11 + 8] = 1;  // T
        buf[2 * 11 + 9] = 1;  // E
        buf[2 * 11 + 10] = 1; // L
#else
		buf[1 * 11 + 0] = 1;  // Z
		buf[1 * 11 + 1] = 1;  // E
		buf[1 * 11 + 2] = 1;  // H
		buf[1 * 11 + 3] = 1;  // N

		buf[3 * 11 + 7] = 1;  // N
		buf[3 * 11 + 8] = 1;  // A
		buf[3 * 11 + 9] = 1;  // C
		buf[3 * 11 + 10] = 1; // H

		buf[3 * 11 + 0] = 1;  // H
		buf[3 * 11 + 1] = 1;  // A
		buf[3 * 11 + 2] = 1;  // L
		buf[3 * 11 + 3] = 1;  // B
#endif
        h++;
    }
    else if (m < 50)
    {
        buf[2 * 11 + 0] = 1;  // D
        buf[2 * 11 + 1] = 1;  // R
        buf[2 * 11 + 2] = 1;  // E
        buf[2 * 11 + 3] = 1;  // I
        buf[2 * 11 + 4] = 1;  // V
        buf[2 * 11 + 5] = 1;  // I
        buf[2 * 11 + 6] = 1;  // E
        buf[2 * 11 + 7] = 1;  // R
        buf[2 * 11 + 8] = 1;  // T
        buf[2 * 11 + 9] = 1;  // E
        buf[2 * 11 + 10] = 1; // L
        h++;
    }
    else if (m < 55)
    {
        buf[1 * 11 + 0] = 1;  // Z
        buf[1 * 11 + 1] = 1;  // E
        buf[1 * 11 + 2] = 1;  // H
        buf[1 * 11 + 3] = 1;  // N
        
        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        h++;
    }
    else
    {
        buf[0 * 11 + 7] = 1;  // F
        buf[0 * 11 + 8] = 1;  // Ü
        buf[0 * 11 + 9] = 1;  // N
        buf[0 * 11 + 10] = 1; // F
        
        buf[1 * 11 + 7] = 1;  // V
        buf[1 * 11 + 8] = 1;  // O
        buf[1 * 11 + 9] = 1;  // R
        h++;
    }

    // StundenÃ¼berlauf wegen ErhÃ¶hung auf nÃ¤chste Stunde berÃ¼cksichtigen
    if (h > 23) h = 0;
    
    // Stunden
    if (h == 0 || h == 12)
    {
        buf[9 * 11 + 0] = 1;  // Z
        buf[9 * 11 + 1] = 1;  // W
        buf[9 * 11 + 2] = 1;  // Ö
        buf[9 * 11 + 3] = 1;  // L
        buf[9 * 11 + 4] = 1;  // F
    }
    else if (h == 1 || h == 13)
    {
        buf[4 * 11 + 0] = 1;  // E
        buf[4 * 11 + 1] = 1;  // I
        buf[4 * 11 + 2] = 1;  // N
        if (m > 4) buf[4 * 11 + 3] = 1; // S
    }
    else if (h == 2 || h == 14)
    {
        buf[4 * 11 + 7] = 1;  // Z
        buf[4 * 11 + 8] = 1;  // W
        buf[4 * 11 + 9] = 1;  // E
        buf[4 * 11 + 10] = 1; // I
    }
    else if (h == 3 || h == 15)
    {
        buf[5 * 11 + 0] = 1;  // D
        buf[5 * 11 + 1] = 1;  // R
        buf[5 * 11 + 2] = 1;  // E
        buf[5 * 11 + 3] = 1;  // I
    }
    else if (h == 4 || h == 16)
    {
        buf[5 * 11 + 7] = 1;  // V
        buf[5 * 11 + 8] = 1;  // I
        buf[5 * 11 + 9] = 1;  // E
        buf[5 * 11 + 10] = 1; // R
    }
    else if (h == 5 || h == 17)
    {
        buf[6 * 11 + 0] = 1;  // F
        buf[6 * 11 + 1] = 1;  // Ü
        buf[6 * 11 + 2] = 1;  // N
        buf[6 * 11 + 3] = 1;  // F
    }
    else if (h == 6 || h == 18)
    {
        buf[6 * 11 + 6] = 1;  // S
        buf[6 * 11 + 7] = 1;  // E
        buf[6 * 11 + 8] = 1;  // C
        buf[6 * 11 + 9] = 1;  // H
        buf[6 * 11 + 10] = 1; // S
    }
    else if (h == 7 || h == 19)
    {
        buf[7 * 11 + 0] = 1;  // S
        buf[7 * 11 + 1] = 1;  // I
        buf[7 * 11 + 2] = 1;  // E
        buf[7 * 11 + 3] = 1;  // B
        buf[7 * 11 + 4] = 1;  // E
        buf[7 * 11 + 5] = 1;  // N
    }
    else if (h == 8 || h == 20)
    {
        buf[7 * 11 + 7] = 1;  // A
        buf[7 * 11 + 8] = 1;  // C
        buf[7 * 11 + 9] = 1;  // H
        buf[7 * 11 + 10] = 1; // T
    }
    else if (h == 9 || h == 21)
    {
        buf[8 * 11 + 0] = 1;  // N
        buf[8 * 11 + 1] = 1;  // E
        buf[8 * 11 + 2] = 1;  // U
        buf[8 * 11 + 3] = 1;  // N
    }
    else if (h == 10 || h == 22)
    {
        buf[8 * 11 + 4] = 1;  // Z
        buf[8 * 11 + 5] = 1;  // E
        buf[8 * 11 + 6] = 1;  // H
        buf[8 * 11 + 7] = 1;  // N
    }
    else if (h == 11 || h == 23)
    {
        buf[8 * 11 + 8] = 1;  // E
        buf[8 * 11 + 9] = 1;  // L
        buf[8 * 11 + 10] = 1; // F
    }
    
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
//    palette: color definition for source buffer
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
//    palette: color definition for source buffer
//    immediately: if true, display buffer immediately; fade to new colors if false
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::set(const uint8_t *buf, palette_entry palette[], bool immediately)
{
    this->setBuffer(this->targetValues, buf, palette);

    if(immediately) 
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
//    source: buffer with color indexes
//    palette: colors for indexed source buffer
// <- --
//---------------------------------------------------------------------------------------
void LEDFunctionsClass::setBuffer(uint8_t *target, const uint8_t *source, palette_entry palette[])
{
    uint32_t mapping, palette_index;
    
    // cast source to 32 bit pointer to ensure 32 bit aligned access
    uint32_t *buf = (uint32_t*)source;
    // this holds the current 4 bytes
    uint32_t currentDWord;
    // this is a pointer to the current 4 bytes for access as single bytes
    uint8_t *currentBytes = (uint8_t*)&currentDWord;
    // this counts bytes from 0...3
    uint32_t byteCounter = 0;
    for(int i = 0; i < NUM_PIXELS; i++)
    {
        // get next 4 bytes
        if(byteCounter == 0) currentDWord = buf[i>>2];

        mapping = led_mapping[i] * 3;
        palette_index = currentBytes[byteCounter];
        target[mapping + 0] = palette[palette_index].r;
        target[mapping + 1] = palette[palette_index].g;
        target[mapping + 2] = palette[palette_index].b;

        if(++byteCounter>3) byteCounter = 0;
    }
}

//---------------------------------------------------------------------------------------
// fade
//
// Fade one step of the color values from this->currentValues[i] to 
// this->targetValues[i].
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
    	if(delta > 64) this->currentValues[i] += 8;
    	else if(delta > 16) this->currentValues[i] += 4;
    	else if(delta > 0) this->currentValues[i]++;
    	else if(delta < -64) this->currentValues[i] -= 8;
    	else if(delta < -16) this->currentValues[i] -= 4;
    	else if(delta < 0) this->currentValues[i]--;
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
        this->pixels->setPixelColor(i, pixels->Color(
            ((int)data[i * 3 + 0] * this->brightness) >> 8,
            ((int)data[i * 3 + 1] * this->brightness) >> 8,
            ((int)data[i * 3 + 2] * this->brightness) >> 8));
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
    for(int i=0; i<NUM_MATRIX_OBJECTS; i++)
    {
        for(int j=i+1; j<NUM_MATRIX_OBJECTS; j++)
        {
            if(matrix_objects[i].y < matrix_objects[j].y)
            {
                temp = matrix_objects[i];
                matrix_objects[i] = matrix_objects[j];
                matrix_objects[j] = temp;
            }
        }
    }

    // clear buffer    
    for(int i=0; i<NUM_PIXELS*3; i++) buf[i] = 0;
    
    for(int i=0; i<NUM_MATRIX_OBJECTS; i++)
    {
    	// check if current matrix object has left the screen
        if(matrix_objects[i].y - MATRIX_GRADIENT_LENGTH > 10)
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
            if(matrix_objects[i].count > 30000)
            {
                matrix_objects[i].count -= 30000;
                matrix_objects[i].y++;
            }
        }

        // draw the gradient of one matrix object
        int y = matrix_objects[i].y;
        int x = matrix_objects[i].x;
        for(int j=0; j<MATRIX_GRADIENT_LENGTH; j++)
        {
            int ofs = led_mapping[x + y*11] * 3;
            if(y >= 0 && y < 10)
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
    for(int i=0; i<NUM_PIXELS*3; i++) buf[i] = 0;

    int x, y, b, offset;
	for(int i=0; i<NUM_STARS; i++)
	{
		x = this->star_objects[i].x;
		y = this->star_objects[i].y;
		b = this->star_objects[i].brightness;
		offset = led_mapping[x + y*11] * 3;
		buf[offset + 0] = b;
		buf[offset + 1] = b;
		buf[offset + 2] = b;

		if(this->star_objects[i].state == 0)
		{
			this->star_objects[i].brightness += this->star_objects[i].speed;
			if(this->star_objects[i].brightness >= 255)
			{
				this->star_objects[i].brightness = 255;
				this->star_objects[i].state = 1;
			}
		}
		else
		{
			this->star_objects[i].brightness -= this->star_objects[i].speed;
			if(this->star_objects[i].brightness <= 0)
			{
				this->star_objects[i].brightness = 0;
				this->star_objects[i].state = 0;
				this->randomizeStar(i);
			}
		}
	}
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
    
    switch(this->heartState)
    {
        case 0:
            if(this->heartBrightness >= 255) this->heartState = 1;
            else this->heartBrightness += 32;
            break;
            
        case 1:
            if(this->heartBrightness < 128) this->heartState = 2;
            else this->heartBrightness -= 32;
            break;

        case 2:
            if(this->heartBrightness >= 255) this->heartState = 3;
            else this->heartBrightness += 32;
            break;

        case 3:
        default:
            if(this->heartBrightness <= 0) this->heartState = 0;
            else this->heartBrightness -= 4;
            break;
    }

    if(this->heartBrightness > 255) this->heartBrightness = 255;
    if(this->heartBrightness < 0) this->heartBrightness = 0;
}
