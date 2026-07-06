#include "Serial.h"
#include <stdio.h>
#include <stdarg.h>

//串口接收环形队列缓冲区
RingBuffer_t Serial_RxRB = 
{
	.Buffer = {0},
	.Write_Index = 0,
	.Read_Index = 0,
};

void Serial_Init(uint32_t Baudrate)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/*选用GPIO_Mode_AF模式时，引脚的控制权（包括输入路径和输出路径）移交给片上外设，
	由AF Mux（复用功能选择器）控制，外设知道哪是输出引脚，哪是输入引脚*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = Serial_GPIO_TX_PIN | Serial_GPIO_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(Serial_GPIO_PORT, &GPIO_InitStructure);

	/*同一个引脚，它的内部电路连接了多个外设，明确告诉硬件：请把某引脚连到指定外设的线路上，
	不要连到该引脚可复用的其他外设上。*/
    GPIO_PinAFConfig(Serial_GPIO_PORT, Serial_GPIO_TX_PINSOURCE, GPIO_AF_USART1);
    GPIO_PinAFConfig(Serial_GPIO_PORT, Serial_GPIO_RX_PINSOURCE, GPIO_AF_USART1);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = Baudrate;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    
	/*
	 * DMA写的内存大小为RINGBUFFER_SIZE，循环模式，永远不会停止，
	 * 则只要串口接收到数据，DMA就会自动将数据写入到Serial_RxRB.Buffer中，没有数据则等待
	 */
    DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_BufferSize = RINGBUFFER_SIZE;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Serial_RxRB.Buffer;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1->DR;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	DMA_Cmd(DMA2_Stream2, ENABLE);
    USART_Cmd(USART1, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		volatile uint32_t temp;
		temp = USART1->SR;
		temp = USART1->DR;
		(void)temp; //防止编译器优化
        
        //记录缓冲区中最新写入的数据的索引
		Serial_RxRB.Write_Index = RINGBUFFER_SIZE - DMA_GetCurrDataCounter(DMA2_Stream2);
	}
}

/*
 * 功能函数，发送字节和字符串
 */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}
