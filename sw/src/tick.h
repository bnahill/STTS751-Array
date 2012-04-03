#ifndef __TICK_H_
#define __TICK_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

// Symbols to define as C symbols
extern "C"{
	void SysTick_Handler(void);
};

//! @ingroup util
//! @{

/*!
 @brief Abstraction for a tick created by the SysTick device
 */
class Tick{
public:
	/*!
	 @brief Start the timer with specified period
	 @param period_ms Number of milliseconds in period
	 @return True if successful
	 */
	static bool start(uint32_t period_ms);

	/*!
	 @brief Wait for the next tick
	 */
	static void wait();

	friend void SysTick_Handler(void);
private:
	//! The actual tick resource
	static volatile int tick;
};

//! @}

#endif

