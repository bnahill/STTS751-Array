#ifndef __FLASH_H_
#define __FLASH_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

//! @ingroup util
//! @{

/*!
 @brief An abstraction of the flash interface for storing TemperatureSensor data
 */
class Flash {
public:
	/*!
	 @brief Initialize flash and write the number of sensors to the header
	 @param num_sensors The number of sensors
	 */
	static void init(uint32_t num_sensors);
	
	/*!
	 @brief Write a float to the next available space in flash
	 @param data The value to write
	 */
	static void write_float(float data);

	/*!
	 @brief Is flash initialized?
	 @return True if it's ready
	 */
	static bool is_ready(){return ready;}

private:
	//! The current offset from the begining of #sensor_data to be writing
	static uint32_t offset;

	//! @name In flash
	//! @{
	//! The following are actually located in flash and are not directly writable
	
	//! The number of sensors in the system; useful for data decoding
	static uint32_t num_sensors;
	//! The actual sensor data
	static float    sensor_data[];
	//! @}

	//! The (constant) size of FP storage in 32-bit words
	static const uint32_t avail_words;

	//! Flag indicatiing that device is ready to write	
	static bool ready;
	
	/*!
	 @brief Convert an address to a sector

	 @param addr The address in flash
	 @return The sector number
	 */
	static uint32_t get_sector(uint32_t addr);
};

//! @}

#endif

