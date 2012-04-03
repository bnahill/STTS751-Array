#ifndef __BUTTON_H_
#define __BUTTON_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

//! @ingroup util
//! @{

/*!
 @brief Class to access button
 */
class Button {
public:
	/*!
	 @brief Initialize button hardware
	 */
	static void init();

	/*!
	 @brief Read the value
	 @return True if pressed
	 */
	static bool read();
};

//! @}

#endif

