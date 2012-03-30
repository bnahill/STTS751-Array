#ifndef __TEMP_H_
#define __TEMP_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

#define I2CA             I2C1
#define I2CB             I2C3

#define COUNT(arr) (sizeof(arr) / sizeof(*arr))

void temp_init(void);
void temp_read_all(void);

class TemperatureSensor{
public:
	TemperatureSensor(I2C_TypeDef *I2C, uint8_t device);
	/*!
	 @brief Read the temperature value for the provided sensor
	 @return Temperature

	 @post The temperature value will be stored in @ref temperature and the
	 reading count will be incremented
	 */
	float read(void);
	void  read_status(void);

	static const int num_sensors;
	static void hw_init(void);
	static void read_all(void);
	static TemperatureSensor sensors[];
private:
	I2C_TypeDef *const I2C;
	uint8_t      const device;
	uint8_t            status;
	float              temperature;
	uint32_t           num_readings;

	uint8_t read_byte(uint8_t address);
	void init(void);	

	/*!
	 @brief Write a single byte (data) to the address specified using the
	 provided temperature sensor

	 @param sensor  The sensor to use
	 @param address The address within the peripheral's memory to write to
	 @param data    The data to write
	 */
	void write_byte(uint8_t address, uint8_t data);
};

#endif

