#ifndef __TEMP_H_
#define __TEMP_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

//! @addtogroup temp STTS751 Temperature Sensor
//! @{

/*!
 @brief An interface to the temperature sensors
 */
class TemperatureSensor {
public:
	/*!
	 @brief Read the temperature value for the provided sensor
	 @return Temperature

	 @post The temperature value will be stored in #temperature and the
	 reading count will be incremented
	 */
	float read(void);

	/*!
	 @brief Read the status register from the sensor and save it
	 @return The status
	 @post #status is set
	 */
	uint8_t  read_status(void);

	/*!
	 @brief The number of sensors; Compile-time constant determined by values 
	 in temp.cpp
	 */
	static const int num_sensors;
	
	/*!
	 @brief Initialize the hardware associated with the sensors
	 @pre None
	 @post The devices may now be used
	 */
	static void init(void);
	
	//! @brief Read the values of all sensors
	static void read_all(void);
	
	//! The temperature sensor devices
	static TemperatureSensor sensors[];
private:
	//! The I2C peripheral used
	I2C_TypeDef        *I2C;
	//! The device address
	uint8_t            device;
	//! The status as returned by the device
	uint8_t            status;
	//! The most recent temperature reading -- Invalid if num_readings < 1
	float              temperature;
	//! The number of readings taken since startup
	uint32_t           num_readings;

	/*!
	 @brief Read a single byte from the provided address
	 @param address The address in the peripheral memory to read from
	 @return The value at that address
	 */
	uint8_t read_byte(uint8_t address);

	/*!
	 @brief Initialize the device
	 @param I2C The I2C peripheral to be used
	 @param device The I2C address on the provided bus
	 
	 @pre init() has run
	 @post The device is configured for 12-bit precision
	 
	 This is intended to be done by init() and involves I2C write
	 */
	void sensor_init(I2C_TypeDef *I2C, uint8_t device);

	/*!
	 @brief Write a single byte to the address specified

	 @param address The address within the peripheral's memory to write to
	 @param data    The data to write
	 */
	void write_byte(uint8_t address, uint8_t data);
};

//! @}

#endif

