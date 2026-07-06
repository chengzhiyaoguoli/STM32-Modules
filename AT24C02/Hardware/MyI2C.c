#include "MyI2C.h"

void MyI2C_Init(void)
{
    RCC_AHB1PeriphClockCmd(MyI2C_GPIO_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = MyI2C_GPIO_SCL | MyI2C_GPIO_SDA ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(MyI2C_GPIO_PORT, &GPIO_InitStructure);
}

void MyI2C_W_SCL(uint8_t BitValue)
{
    GPIO_WriteBit(MyI2C_GPIO_PORT, MyI2C_GPIO_SCL, (BitAction)BitValue);
    Delay_us(2);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
    GPIO_WriteBit(MyI2C_GPIO_PORT, MyI2C_GPIO_SDA, (BitAction)BitValue);
    Delay_us(2);
}

uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(MyI2C_GPIO_PORT, MyI2C_GPIO_SDA);
    Delay_us(2);
    return BitValue;
}

void MyI2C_Start(void)
{
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
