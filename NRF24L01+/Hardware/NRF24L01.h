#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "stm32f10x.h"                  // Device header
#include "NRF24L01_Define.h"

#define NRF24L01_GPIO_PORT          GPIOA
#define NRF24L01_GPIO_CLK           RCC_APB2Periph_GPIOA
#define NRF24L01_GPIO_CE_PIN        GPIO_Pin_0
#define NRF24L01_GPIO_CSN_PIN       GPIO_Pin_1
#define NRF24L01_GPIO_SCK_PIN       GPIO_Pin_2
#define NRF24L01_GPIO_MOSI_PIN      GPIO_Pin_3
#define NRF24L01_GPIO_MISO_PIN      GPIO_Pin_4

#define NRF24L01_TX_PACKET_WIDTH    4                                   //发送通道有效载荷数据宽度，最大32字节
#define NRF24L01_RX_PACKET_WIDTH    4                                   //接收通道有效载荷数据宽度，最大32字节

extern uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];             //发送有效载荷数据

extern uint8_t NRF24L01_RxPacket_P0[NRF24L01_RX_PACKET_WIDTH];          //接收通道0数据缓冲区
extern uint8_t NRF24L01_RxPacket_P1[NRF24L01_RX_PACKET_WIDTH];          //接收通道1数据缓冲区
extern uint8_t NRF24L01_RxPacket_P2[NRF24L01_RX_PACKET_WIDTH];          //接收通道2数据缓冲区
extern uint8_t NRF24L01_RxPacket_P3[NRF24L01_RX_PACKET_WIDTH];          //接收通道3数据缓冲区
extern uint8_t NRF24L01_RxPacket_P4[NRF24L01_RX_PACKET_WIDTH];          //接收通道4数据缓冲区
extern uint8_t NRF24L01_RxPacket_P5[NRF24L01_RX_PACKET_WIDTH];          //接收通道5数据缓冲区

/*指令实现*/
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);
uint8_t NRF24L01_ReadStatus(void);

/*功能函数*/
void NRF24L01_PowerDown(void);
void NRF24L01_StandbyI(void);
void NRF24L01_RxMode(void);
void NRF24L01_TxMode(void);

void NRF24L01_Init(void);
void NRF24L01_Send(void);
uint8_t NRF24L01_Receive(void);

#endif
