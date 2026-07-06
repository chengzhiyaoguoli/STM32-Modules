#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f4xx.h"
#include "Delay.h"

#define MyI2C_GPIO_CLK      RCC_AHB1Periph_GPIOB
#define MyI2C_GPIO_PORT     GPIOB
#define MyI2C_GPIO_SCL      GPIO_Pin_6
#define MyI2C_GPIO_SDA      GPIO_Pin_7

#define ACK                 0
#define NACK                1

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

#endif
