#include "AT24C02.h"
/*
 * EEPROM二维数组缓冲区，完美对应页，可通过for循环写入，哈哈，太妙了！
 * 不需要考虑页内回卷和换页，要写入时直接一次全部写入即可！
 */
uint8_t EEPROM_Buffer[PAGES][PAGE_SIZE] = {0};

void AT24C02_Init(void)
{
    MyI2C_Init();
}

/*
 * 将缓冲区中的所有数据写入AT24C02中
 */
void AT24C02_All_Write(void)
{
    for (uint8_t page = 0; page < PAGES; page ++)
    {
        MyI2C_Start();                                          //起始信号
        MyI2C_SendByte((AT24C02_ADDRESS << 1) | WRITE_CMD);     //发送从机设备地址（高7位）和写指令（低1位）
        MyI2C_ReceiveAck();
        MyI2C_SendByte(page * PAGE_SIZE);                       //发送写入操作起始地址
        MyI2C_ReceiveAck();

        for (uint8_t i = 0; i < PAGE_SIZE; i ++)
        {
            MyI2C_SendByte(EEPROM_Buffer[page][i]);
            MyI2C_ReceiveAck();
        }

        MyI2C_Stop();                                           //停止信号

        Delay_ms(5);                                            //每次写入操作结束等待芯片内部写入完成
    }
}

/*
 * 将AT24C02中的所有数据读取至缓冲区
 */
void AT24C02_All_Read(void)
{
    MyI2C_Start();                                              //起始信号
    MyI2C_SendByte((AT24C02_ADDRESS << 1) | WRITE_CMD);         //发送从机设备地址（高7位）和写指令（低1位），目的是调整指针位置
    MyI2C_ReceiveAck();
    MyI2C_SendByte(0x00);                                       //发送读取操作起始地址，其实就是第一个字节地址
    MyI2C_ReceiveAck();

    MyI2C_Start();                                              //起始信号
    MyI2C_SendByte((AT24C02_ADDRESS << 1) | READ_CMD);          //发送从机设备地址（高7位）和读指令（低1位）
    MyI2C_ReceiveAck();

    uint8_t *p = (uint8_t *)EEPROM_Buffer;
    for (uint16_t i = 0; i < EEPROM_SIZE; i ++)
    {
        *p ++ = MyI2C_ReceiveByte();                            //先通过指针接收数据，后将指针后移

        if (i != EEPROM_SIZE - 1)
        {
            MyI2C_SendAck(ACK);                                 //发送应答
        }
        else
        {
            MyI2C_SendAck(NACK);                                //最后一次，不应答，准备结束通信
        }
    }

    MyI2C_Stop();                                               //停止信号
}
