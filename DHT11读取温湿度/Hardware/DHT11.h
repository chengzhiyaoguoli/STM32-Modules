#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

extern uint8_t DHT11_Data[5];
extern uint8_t Temperature;
extern uint8_t Humidity;

uint8_t DHT11_Read_Data(uint8_t *DHT11_Data, uint8_t *Temperature, uint8_t *Humidity);

/*DHT11宏定义*/
#define DHT11_GPIO_PORT  GPIOA
#define DHT11_GPIO_PIN   GPIO_Pin_2
#define DHT11_GPIO_CLK   RCC_APB2Periph_GPIOA

#endif
