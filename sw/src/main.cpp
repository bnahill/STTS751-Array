extern "C" {
	#include "stm32f4xx_conf.h"
}

#include "temp.h"
#include "button.h"
#include "flash.h"
#include "tick.h"

//! @defgroup util Utilities

//! @name LED macros
//! @{
#define LED_GPIO GPIOD
#define LED_PIN(x) (GPIO_Pin_12 << x)
#define LED_PIN_ALL (LED_PIN(0) | LED_PIN(1) | LED_PIN(2) | LED_PIN(3))

#define LED_SET(x)    (LED_GPIO->ODR |= LED_PIN(x))
#define LED_CLR(x)    (LED_GPIO->ODR &= ~LED_PIN(x))
#define LED_CLR_ALL() (LED_GPIO->ODR &= ~LED_PIN_ALL)
//! @}

// Symbols to define as C symbols
extern "C"{
	int main(void);
	void SysTick_Handler(void);
};


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
	if(!Tick::start(200)){
		while(1);
	}

	while (1){
		LED_SET(0);
		Tick::wait();
		LED_SET(1);
		Tick::wait();
		LED_SET(2);
		Tick::wait();
		LED_SET(3);
		Tick::wait();

		LED_CLR_ALL();

		Tick::wait();

		TemperatureSensor::read_all();
	}
}

