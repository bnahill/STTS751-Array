extern "C" {
	#include "stm32f4xx_conf.h"
}

#include "temp.h"
#include "button.h"
#include "flash.h"

//! A single millisecond tick
#define SYSTICK_MS (((double)SystemCoreClock / 8.0) / 1000.0)

//! @name LED macros
//! @{
#define LED_GPIO GPIOD
#define LED_PIN(x) (GPIO_Pin_12 << x)
#define LED_PIN_ALL (LED_PIN(0) | LED_PIN(1) | LED_PIN(2) | LED_PIN(3))

#define LED_SET(x)    (LED_GPIO->ODR |= LED_PIN(x))
#define LED_CLR(x)    (LED_GPIO->ODR &= ~LED_PIN(x))
#define LED_CLR_ALL() (LED_GPIO->ODR &= ~LED_PIN_ALL)
//! @}

//! A flag to detect each tick
static volatile int tick = 0;

// Symbols to define as C symbols
extern "C"{
	int main(void);
	void SysTick_Handler(void);
};

//! @brief Wait for a tick and reset the ticker
static inline void wait_tick(void){
	while(tick == 0); tick = 0;
}

int main(void){
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure LEDs in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = LED_PIN_ALL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	
	TemperatureSensor::init();

	Button::init();

	// Wait for a button press
	while(!Button::read());

	// Initialize (erase) flash
	Flash::init(TemperatureSensor::num_sensors);
	
	// Configure SysTick for 200ms period	
	if(SysTick_Config(200.0 * SYSTICK_MS)){
		while(1);
	}

	// THEN specify to divide this clock by 8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	while (1){
		LED_SET(0);
		wait_tick();
		LED_SET(1);
		wait_tick();
		LED_SET(2);
		wait_tick();
		LED_SET(3);
		wait_tick();

		LED_CLR_ALL();

		wait_tick();

		TemperatureSensor::read_all();
	}
}

void SysTick_Handler(void){
	tick = 1;
}

