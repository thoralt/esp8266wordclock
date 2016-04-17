#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <IPAddress.h>

typedef struct _palette_entry
{
    uint8_t r, g, b;
} palette_entry;

typedef struct _config_struct
{
    uint32_t magic;
    palette_entry bg;
    palette_entry fg;
    palette_entry s;
    uint8_t ntpserver[4];
} config_struct;

#define EEPROM_SIZE 512

class ConfigClass
{
public:
	ConfigClass();
	virtual ~ConfigClass();
	void begin();
	void save();
	void load();
	void reset();

	palette_entry fg;
	palette_entry bg;
	palette_entry s;
	IPAddress ntpserver = IPAddress(0, 0, 0, 0);

private:
	config_struct *config = (config_struct*) eeprom_data;;
	uint8_t eeprom_data[EEPROM_SIZE];
};

extern ConfigClass Config;

#endif /* CONFIG_H_ */
