#ifndef __24C02_H
#define __24C02_H

#include "stm32f4xx.h"
#include "MyI2C.h"
#include "Serial.h"

#define EEPROM_SIZE         256             //可存储总字节数
#define PAGE_SIZE           8               //页字节大小
#define PAGES               32              //总页数
#define AT24C02_ADDRESS     0x50            //器件地址，A0，A1，A2均接地为0
#define WRITE_CMD           0x00            //写指令
#define READ_CMD            0x01            //读指令

extern uint8_t EEPROM_Buffer[PAGES][PAGE_SIZE];

void AT24C02_Init(void);
void AT24C02_All_Write(void);
void AT24C02_All_Read(void);

#endif
