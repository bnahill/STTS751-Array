#ifndef __FLASH_H_
#define __FLASH_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

class Flash {
public:
	static void init(uint32_t num_sensors);
	static void write_float(float data);

	static bool is_ready(){return ready;}

private:
	static uint32_t offset;
	static uint32_t num_sensors;
	static float    sensor_data[];
	static const uint32_t avail_words;
	
	static bool ready;
	
	static uint32_t get_sector(uint32_t addr);
};

#endif

