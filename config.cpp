#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

//---------------------------------------------------------------------------------------
// global instance
//---------------------------------------------------------------------------------------
ConfigClass Config = ConfigClass();

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
ConfigClass::ConfigClass()
{
	this->reset();
}

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
ConfigClass::~ConfigClass()
{
}

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void ConfigClass::begin()
{
    EEPROM.begin(EEPROM_SIZE);
    this->load();
}

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void ConfigClass::save()
{
    this->config->bg = this->bg;
    this->config->fg = this->fg;
    this->config->s = this->s;
    for(int i=0; i<4; i++) this->config->ntpserver[i] = this->ntpserver[i];

    for(int i=0; i<EEPROM_SIZE; i++) EEPROM.write(i, this->eeprom_data[i]);
    EEPROM.commit();
}

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void ConfigClass::reset()
{
    this->config->magic = 0xDEADBEEF;
    this->config->bg = {0, 0, 0};
    this->config->fg = {255, 255, 255};
    this->config->s = {32, 0, 21};
    this->ntpserver[0] = 129;
    this->ntpserver[1] = 6;
    this->ntpserver[2] = 15;
    this->ntpserver[3] = 28;
}

//---------------------------------------------------------------------------------------
//
//
// ...
//
// ->
// <- --
//---------------------------------------------------------------------------------------
void ConfigClass::load()
{
    Serial.println("Reading EEPROM config");
    for(int i=0; i<EEPROM_SIZE; i++) this->eeprom_data[i] = EEPROM.read(i);
    if(this->config->magic != 0xDEADBEEF)
    {
        Serial.println("EEPROM config invalid, writing default values");
        this->reset();
        this->save();
    }
    this->bg = this->config->bg;
    this->fg = this->config->fg;
    this->s = this->config->s;
    for(int i=0; i<4; i++) this->ntpserver[i] = this->config->ntpserver[i];
}
