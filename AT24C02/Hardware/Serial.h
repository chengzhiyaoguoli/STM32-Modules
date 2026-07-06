#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f4xx.h"

#define RINGBUFFER_SIZE   2048                      //接收数据缓冲区大小

typedef struct {
    uint8_t Buffer[RINGBUFFER_SIZE];
    volatile uint16_t Write_Index;                  //写入数据位置
    volatile uint16_t Read_Index;                   //读取数据位置
} RingBuffer_t;

extern RingBuffer_t Serial_RxRB;

/*
 *初始化相关宏定义
 */
#define Serial_GPIO_PORT            GPIOA
#define Serial_GPIO_TX_PIN          GPIO_Pin_9
#define Serial_GPIO_RX_PIN          GPIO_Pin_10
#define Serial_GPIO_TX_PINSOURCE    GPIO_PinSource9
#define Serial_GPIO_RX_PINSOURCE    GPIO_PinSource10

void Serial_Init(uint32_t Baudrate);
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char *String);
void Serial_Printf(char *format, ...);

#endif
