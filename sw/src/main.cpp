extern "C" {
	#include "stm32f4xx_conf.h"
}

#include "temp.h"

#define SYSTICK_MS ((SystemCoreClock / 8) / 1000)

static volatile int go = 0;

// Symbols to define as C symbols
extern "C"{
	void ClkOutCfg(void);
	int main(void);
	void SysTick_Handler(void);
};


void ClkOutCfg(void){
	RCC->CFGR &= ~(RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE);
	RCC->CFGR |= RCC_CFGR_MCO2_1 | RCC_CFGR_MCO2PRE_2 | 
	             RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_0;
}

int main(void){
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure PD12, PD13, PD14 and PD15 in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TemperatureSensor::hw_init();

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	                        
	if(SysTick_Config(100 * SYSTICK_MS)){
		while(1);
	}

	while (1){
		/* PD12 to be toggled */
		GPIO_SetBits(GPIOD, GPIO_Pin_12);

		while(go == 0); go = 0;

		/* PD13 to be toggled */
		GPIO_SetBits(GPIOD, GPIO_Pin_13);

		while(go == 0); go = 0;

		/* PD14 to be toggled */
		GPIO_SetBits(GPIOD, GPIO_Pin_14);

		/* Insert delay */
		while(go == 0); go = 0;

		/* PD15 to be toggled */
		GPIO_SetBits(GPIOD, GPIO_Pin_15);

		/* Insert delay */
		while(go == 0); go = 0;

		GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

		/* Insert delay */
		while(go == 0); go = 0;

		TemperatureSensor::read_all();
		go = TemperatureSensor::num_sensors;
	}
}

void SysTick_Handler(void){
	go = 1;
	GPIOC->ODR ^= GPIO_Pin_7;
}

