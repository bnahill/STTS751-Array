#include "tick.h"

//! A single millisecond tick
#define SYSTICK_MS (((double)SystemCoreClock / 8.0) / 1000.0)

volatile int Tick::tick;

bool Tick::start(uint32_t period_ms){
	bool ret = SysTick_Config((float)period_ms * SYSTICK_MS);
	tick = 0;
	// THEN specify to divide this clock by 8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	return ret;
}

void Tick::wait(){
	while(tick == 0);
	tick = 0;
}

void SysTick_Handler(void){
	Tick::tick = 1;
}
