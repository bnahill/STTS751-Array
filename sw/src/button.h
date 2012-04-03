#ifndef __BUTTON_H_
#define __BUTTON_H_

extern "C" {
	#include "stm32f4xx_conf.h"
};

class Button {
public:
	static void init();
	static int read();
};

#endif

