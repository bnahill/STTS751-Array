#ifndef __BUTTON_H_
#define __BUTTON_H_

#include "stm32f4xx_conf.h"

//! @addtogroup button Button
//! @{

/*!
 @brief Initialize button hardware
 */
void button_init(void);

/*!
 @brief Read the value
 @return True if pressed
 */
int button_read(void);

//! @}

#endif

