#ifndef __TICK_H_
#define __TICK_H_

extern "C" {
	#include "stm32f4xx_conf.h"
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
	static bool start(float period_ms);

	/*!
	 @brief Wait for some number of ticks
	 @param num_ticks Number of ticks to wait
	 */
	static void wait(uint32_t num_ticks = 1);

	friend void SysTick_Handler(void);
private:
	//! The actual tick resource
	static volatile int tick;
};

//! @}

#endif

