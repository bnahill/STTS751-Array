#include "tick.h"

//! A single millisecond tick
#define SYSTICK_MS (((double)SystemCoreClock / 8.0) / 1000.0)

//! The actual tick resource
static volatile int tick;

int tick_start(float period_ms){
	if(SysTick_Config((float)period_ms * SYSTICK_MS))
		return 0;
	tick = 0;
	// THEN specify to divide this clock by 8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	return 1;
}

void tick_wait(uint32_t num_ticks){
	while(num_ticks--){
		while(tick == 0);
		tick = 0;
	}
}

void SysTick_Handler(void){
	tick = 1;
}
