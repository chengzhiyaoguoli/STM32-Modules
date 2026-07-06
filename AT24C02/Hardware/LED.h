#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

#define LED_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define LED_GPIO_PORT   GPIOB
#define LED_GPIO_PIN    GPIO_Pin_2

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);

#endif
