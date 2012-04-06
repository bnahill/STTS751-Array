#include "button.h"

#define BUTTON_GPIO  GPIOA
#define BUTTON_PIN   GPIO_Pin_0

void button_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
}


int button_read(){
	if((BUTTON_GPIO->IDR & BUTTON_PIN) != 0)
		return 1;
	return 0;
}

