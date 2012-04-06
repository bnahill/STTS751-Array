#ifndef __TEMP_H_
#define __TEMP_H_

#include "stm32f4xx_conf.h"

#include "toolchain.h"

//! @addtogroup temp STTS751 Temperature Sensor
//! @{

typedef struct {
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
} temp_sensor_t;

/*!
 @brief Read the temperature value for the provided sensor
 @param sensor The sensor
 @return Temperature

 @post The temperature value will be stored in @a sensor and the 
 reading count will be incremented.
 */
float temp_read(temp_sensor_t *RESTRICT sensor);

/*!
 @brief Read the status register from the sensor and save it
 @param sensor The sensor
 @return The status
 @post @a Status is set
 */
uint8_t  temp_read_status(temp_sensor_t *RESTRICT sensor);

/*!
 @brief The number of sensors; Compile-time constant determined by values 
 in temp.cpp
 */
extern const int temp_num_sensors;

/*!
 @brief Initialize the hardware associated with the sensors
 @pre None
 @post The devices may now be used
 */
void temp_init(void);

/*!
 @brief Read the values of all sensors. If flash has been initialized,
 the value will be written to flash as well.
 */
void temp_read_all(void);

//! The temperature sensor devices
extern temp_sensor_t temp_sensors[];

//! @}

#endif

