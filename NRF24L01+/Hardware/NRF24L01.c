#include "NRF24L01.h"

uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH] = {0x01, 0x02, 0x03, 0x04};     //发送有效载荷数据

uint8_t NRF24L01_RxPacket_P0[NRF24L01_RX_PACKET_WIDTH];                 //接收通道0数据缓冲区
uint8_t NRF24L01_RxPacket_P1[NRF24L01_RX_PACKET_WIDTH];                 //接收通道1数据缓冲区
uint8_t NRF24L01_RxPacket_P2[NRF24L01_RX_PACKET_WIDTH];                 //接收通道2数据缓冲区
uint8_t NRF24L01_RxPacket_P3[NRF24L01_RX_PACKET_WIDTH];                 //接收通道3数据缓冲区
uint8_t NRF24L01_RxPacket_P4[NRF24L01_RX_PACKET_WIDTH];                 //接收通道4数据缓冲区
uint8_t NRF24L01_RxPacket_P5[NRF24L01_RX_PACKET_WIDTH];                 //接收通道5数据缓冲区

/*发送地址为接收方的接收通道地址，开启自动应答后需要将自己的一个通道接收地址改为发送地址才能接收应答*/
uint8_t NRF24L01_TxAddress[5] = {0x22, 0xAA, 0xBB, 0xCC, 0xDD};         //发送通道地址，5字节

/*虽说有6个接收通道，如果设备既需要接收又需要发送建议只用5个接收通道，留一个通道用于接收应答，如果只是单向传输则无所谓，但是可以在发送结束后将应答通道地址改回去也可以*/
/*逆天！地址低位先写，所以0x00, 0x11, 0x22, 0x33, 0x44写进去后地址为0x4433221100，写发送地址相同，前低位后高位*/
uint8_t NRF24L01_RxAddress_P0[5] = {0x00, 0x11, 0x22, 0x33, 0x44};      //接收通道0地址，5字节
uint8_t NRF24L01_RxAddress_P1[5] = {0x11, 0xAA, 0xBB, 0xCC, 0xDD};      //接收通道1地址，5字节
uint8_t NRF24L01_RxAddress_P2 = 0x22;                                   //接收通道2地址，1字节，高位字节与通道1相同
uint8_t NRF24L01_RxAddress_P3 = 0x33;
uint8_t NRF24L01_RxAddress_P4 = 0x44;
uint8_t NRF24L01_RxAddress_P5 = 0x55;

//引脚配置重定义
void NRF24L01_W_CE(uint8_t BitValue)
{
    GPIO_WriteBit(NRF24L01_GPIO_PORT, NRF24L01_GPIO_CE_PIN, (BitAction)BitValue);
}

void NRF24L01_W_CSN(uint8_t BitValue)
{
    GPIO_WriteBit(NRF24L01_GPIO_PORT, NRF24L01_GPIO_CSN_PIN, (BitAction)BitValue);
}

void NRF24L01_W_SCK(uint8_t BitValue)
{
    GPIO_WriteBit(NRF24L01_GPIO_PORT, NRF24L01_GPIO_SCK_PIN, (BitAction)BitValue);
}

void NRF24L01_W_MOSI(uint8_t BitValue)
{
    GPIO_WriteBit(NRF24L01_GPIO_PORT, NRF24L01_GPIO_MOSI_PIN, (BitAction)BitValue);
}

uint8_t NRF24L01_R_MISO(void)
{
    return GPIO_ReadInputDataBit(NRF24L01_GPIO_PORT, NRF24L01_GPIO_MISO_PIN);
}

//引脚初始化
void NRF24L01_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(NRF24L01_GPIO_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = NRF24L01_GPIO_CE_PIN | NRF24L01_GPIO_CSN_PIN | NRF24L01_GPIO_SCK_PIN | NRF24L01_GPIO_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NRF24L01_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = NRF24L01_GPIO_MISO_PIN;
    GPIO_Init(NRF24L01_GPIO_PORT, &GPIO_InitStructure);

    NRF24L01_W_CE(0);       //待机模式1，退出收发模式，既不发送也不接收
    NRF24L01_W_CSN(1);      //N代表低电平有效，默认给高电平不选中从机
    NRF24L01_W_SCK(0);      //SCK空闲时低电平，对应SPI模式0或模式1（这里选用模式0）
    NRF24L01_W_MOSI(0);     //电平随意
}

//通信协议：软件模拟SPI时序和从机交换字节
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
    for (uint8_t i = 0; i < 8; i ++)
    {
        //SPI移除数据
        if (Byte & 0x80)
        {
            NRF24L01_W_MOSI(1);
        }
        else
        {
            NRF24L01_W_MOSI(0);
        }
        Byte <<= 1;
        //SCK置高电平
        NRF24L01_W_SCK(1);
        //SPI移入数据
        if (NRF24L01_R_MISO())
        {
            Byte |= 0x01;
        }
        //SCK置低电平
        NRF24L01_W_SCK(0);
    }
    return Byte;
}

//指令实现
/*写单字节寄存器*/
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    NRF24L01_SPI_SwapByte(Data);
    NRF24L01_W_CSN(1);
}

/*读单字节寄存器*/
uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
    return Data;
}

/*写多字节寄存器*/
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t* DataArray, uint8_t Count)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    for (uint8_t i = 0; i < Count; i ++)
    {
        NRF24L01_SPI_SwapByte(DataArray[i]);
    }
    NRF24L01_W_CSN(1);
}

/*读多字节寄存器*/
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t* DataArray, uint8_t Count)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    for (uint8_t i = 0; i < Count; i ++)
    {
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    }
    NRF24L01_W_CSN(1);
}

/*写TxFIFO*/
void NRF24L01_WriteTxPayload(uint8_t* DataArray, uint8_t Count)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < Count; i ++)
    {
        NRF24L01_SPI_SwapByte(DataArray[i]);
    }
    NRF24L01_W_CSN(1);
}

/*读RxFIFO*/
void NRF24L01_ReadRxPayload(uint8_t* DataArray, uint8_t Count)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
    for (uint8_t i = 0; i < Count; i ++)
    {
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    }
    NRF24L01_W_CSN(1);
}

/*清空TxFIFO*/
void NRF24L01_FlushTx(void)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
    NRF24L01_W_CSN(1);
}

/*清空RxFIFO*/
void NRF24L01_FlushRx(void)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
    NRF24L01_W_CSN(1);
}

/*读取状态寄存器，非其他寄存器*/
uint8_t NRF24L01_ReadStatus(void)
{
    uint8_t Status;
    NRF24L01_W_CSN(0);
    Status = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
    return Status;
}

/*功能函数*/
/*掉电状态*/
void NRF24L01_PowerDown(void)
{
    uint8_t config;
    NRF24L01_W_CE(0);
    config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    config &= ~0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, config);
}

/*待机状态1*/
void NRF24L01_StandbyI(void)
{
    uint8_t config;
    NRF24L01_W_CE(0);
    config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    config |= 0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, config);
}

/*接收状态*/
void NRF24L01_RxMode(void)
{
    uint8_t config;
    NRF24L01_W_CE(0);
    config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    config |= 0x03;
    NRF24L01_WriteReg(NRF24L01_CONFIG, config);
    NRF24L01_W_CE(1);
}

/*发送状态*/
void NRF24L01_TxMode(void)
{
    uint8_t config;
    NRF24L01_W_CE(0);
    config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    config |= 0x02;
    config &= ~0x01;
    NRF24L01_WriteReg(NRF24L01_CONFIG, config);
    NRF24L01_W_CE(1);
}

/*模块初始化*/
void NRF24L01_Init(void)
{
    NRF24L01_GPIO_Init();

    NRF24L01_WriteReg(NRF24L01_CONFIG, 0x08);                           //开启接收、发送、接收FIFO满中断，使能CRC校验，CRC一个字节，PWR_UP和PRIM_RX通过使用过程中状态函数写入
    NRF24L01_WriteReg(NRF24L01_EN_AA, 0x3F);                            //使能6个接收通道自动应答
    NRF24L01_WriteReg(NRF24L01_EN_RXADDR, 0x3F);                        //使能6个通道的接收地址
    NRF24L01_WriteReg(NRF24L01_SETUP_AW, 0x03);                         //设置5字节的发送和接收地址宽度
    NRF24L01_WriteReg(NRF24L01_SETUP_RETR, 0x03);                       //3次自动重传，自动重传间隔250us
    NRF24L01_WriteReg(NRF24L01_RF_CH, 0x02);                            //载波工作频率2.402GHz，默认值
    NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x0E);                         //通信速率2Mbps，发射功率0dBm

    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress_P0, 5);  //接收通道0地址，5字节
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P1, NRF24L01_RxAddress_P1, 5);  //接收通道1地址，5字节
    NRF24L01_WriteReg(NRF24L01_RX_ADDR_P2, NRF24L01_RxAddress_P2);      //接收通道2地址，1字节
    NRF24L01_WriteReg(NRF24L01_RX_ADDR_P3, NRF24L01_RxAddress_P3);      //接收通道3地址，1字节
    NRF24L01_WriteReg(NRF24L01_RX_ADDR_P4, NRF24L01_RxAddress_P4);      //接收通道4地址，1字节
    NRF24L01_WriteReg(NRF24L01_RX_ADDR_P5, NRF24L01_RxAddress_P5);      //接收通道5地址，1字节

    NRF24L01_WriteReg(NRF24L01_RX_PW_P0, NRF24L01_RX_PACKET_WIDTH);     //接收通道0有效载荷数据宽度，通过宏指定
    NRF24L01_WriteReg(NRF24L01_RX_PW_P1, NRF24L01_RX_PACKET_WIDTH);     //接收通道1有效载荷数据宽度，通过宏指定
    NRF24L01_WriteReg(NRF24L01_RX_PW_P2, NRF24L01_RX_PACKET_WIDTH);     //接收通道2有效载荷数据宽度，通过宏指定
    NRF24L01_WriteReg(NRF24L01_RX_PW_P3, NRF24L01_RX_PACKET_WIDTH);     //接收通道3有效载荷数据宽度，通过宏指定
    NRF24L01_WriteReg(NRF24L01_RX_PW_P4, NRF24L01_RX_PACKET_WIDTH);     //接收通道4有效载荷数据宽度，通过宏指定
    NRF24L01_WriteReg(NRF24L01_RX_PW_P5, NRF24L01_RX_PACKET_WIDTH);     //接收通道5有效载荷数据宽度，通过宏指定

    NRF24L01_RxMode();
}

/*发送函数*/
void NRF24L01_Send(void)
{
    /*写入发送地址和有效载荷，同时将接收通道0的接收地址写为发送地址，用于接收应答*/
    NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);
    NRF24L01_WriteTxPayload(NRF24L01_TxPacket, NRF24L01_TX_PACKET_WIDTH);
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);

    NRF24L01_TxMode();

    while (1)
    {
        uint8_t Status = NRF24L01_ReadStatus();
        if (Status & 0x20)
        {
            /*状态寄存器的TX_DS位置1，代表发送成功*/
            break;
        }
        else if (Status & 0x10)
        {
            /*状态寄存器的MAX_RT位置1，代表发送失败且达到最大重发次数*/
            break;
        }
    }

    /*TX_DS和MAX_RT的值要手动清0，状态寄存器有特殊写1清0机制，并非直接覆盖整个寄存器的值，写1的位清0，写0的位不变*/
    NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);

    /*清空TxFIFO，防止发送失败的数据残留等意外情况*/
    NRF24L01_FlushTx();

    /*将用于接收应答的通道地址改回初始化的地址，后续正常使用*/
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress_P0, 5);

    NRF24L01_RxMode();
}

/*接收函数*/
uint8_t NRF24L01_Receive(void)
{
    uint8_t Status = NRF24L01_ReadStatus();
    if (Status & 0x40)
    {
        uint8_t Pipe = (Status >> 1) & 0x07;

        switch (Pipe)
        {
            case 0:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P0, NRF24L01_RX_PACKET_WIDTH);
                break;
            case 1:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P1, NRF24L01_RX_PACKET_WIDTH);
                break;
            case 2:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P2, NRF24L01_RX_PACKET_WIDTH);
                break;
            case 3:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P3, NRF24L01_RX_PACKET_WIDTH);
                break;
            case 4:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P4, NRF24L01_RX_PACKET_WIDTH);
                break;
            case 5:
                NRF24L01_ReadRxPayload(NRF24L01_RxPacket_P5, NRF24L01_RX_PACKET_WIDTH);
                break;
        }
        
        NRF24L01_WriteReg(NRF24L01_STATUS, 0x40);
        return 1;
    }
    return 0;
}
