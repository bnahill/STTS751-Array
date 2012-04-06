#ifndef __FLASH_H_
#define __FLASH_H_

#include "stm32f4xx_conf.h"

//! @addtogroup flash Flash Interface
//! @{

/*!
 @brief Initialize flash and write the number of sensors to the header
 @param num_sensors The number of sensors
 */
void flash_init(uint32_t num_sensors);

/*!
 @brief Write a float to the next available space in flash
 @param data The value to write
 */
void flash_write_float(float data);

/*!
 @brief Is flash initialized?
 @return True if it's ready
 */
int flash_is_ready(void);

//! @}

#endif

