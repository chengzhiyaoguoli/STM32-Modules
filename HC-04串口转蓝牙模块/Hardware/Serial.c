#include "Serial.h"

char DMA_TxBuffer[DMA_TX_BUF_SIZE];

void Serial_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;				//传入USART3发送寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;			//数据长度为字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//地址不自增
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_TxBuffer;					//传入数据存储的地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//数据长度为字节
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//地址自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;								//外设为数据转运的终点
	DMA_InitStructure.DMA_BufferSize = 8;											//8个ADC序列，一个循环转运8次
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;									//单次传输模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									//硬件触发（USART）使能，软件触发失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//DMA优先级高
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART3, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);									//USART3发送请求使能
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i <Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}
	
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + 0x30);
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

void DMA1_Channel2_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC2) == SET)
	{
		DMA_Cmd(DMA1_Channel2, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC2); 
	}
}
