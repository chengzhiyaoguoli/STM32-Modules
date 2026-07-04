#include "DHT11.h"

uint8_t DHT11_Data[5];
uint8_t Temperature;
uint8_t Humidity;

void GPIO_Mode_Input(void)
{
	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

void GPIO_Mode_Output(void)
{
	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

/*产生数据读取信号，主机发送特定时序波形后将引脚控制权交给DHT11*/
void DHT11_Reset(void)
{
	GPIO_Mode_Output();
	GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
	Delay_ms(20);
	GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
	Delay_us(13);
	GPIO_Mode_Input();
}

/*检查引脚交给DHT11后是否存在，能否正常接收到DHT11起始信号*/
uint8_t DHT11_Check(void)
{
	uint8_t retry = 0;
	
	while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	if (retry >= 100)
	{
		return 1;
	}
	else
	{
		retry = 0;
	}
	
	while (!GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	if (retry >= 100)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0;
	
	while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	retry = 0;
	while (!GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
	{
		retry ++;
		Delay_us(1);
	}
	Delay_us(40);
	if (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t DHT11_Read_Byte(void)
{
	uint8_t Byte;
	for (uint8_t i = 0; i < 8; i ++)
	{
		Byte <<= 1;
		Byte |= DHT11_Read_Bit();
	}
	return Byte;
}

uint8_t DHT11_Read_Data(uint8_t *DHT11_Data, uint8_t *Temperature, uint8_t *Humidity)
{
	DHT11_Reset();
	if (DHT11_Check() == 0)
	{
		for (uint8_t i = 0; i < 5; i ++)
		{
			DHT11_Data[i] = DHT11_Read_Byte();
		}
		if (DHT11_Data[4] == DHT11_Data[0] + DHT11_Data[1] + DHT11_Data[2] + DHT11_Data[3])
		{
			*Temperature = DHT11_Data[2];
			*Humidity = DHT11_Data[0];
		}
		return 0;
	}
	else
	{
		return 1;
	}
}
