#ifndef __HC_05_H
#define __HC_05_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern uint8_t HC_04_RxFlag;
extern uint8_t Float_Data[4];
extern uint8_t HC_04_RxPacket[100];

void HC_04_Init(void);
void HC_04_ATSend(const char* ATCommand);
void HC_04_SendByte(uint8_t Byte);
void HC_04_SendArray(uint8_t *Array, uint16_t Length);
void HC_04_SendString(char *String);
uint32_t HC_04_Pow(uint32_t X, uint32_t Y);
void HC_04_SendNumber(uint32_t Number, uint8_t Length);
void HC_04_SendFloat(float Data);
void HC_04_Printf(char *format, ...);
uint8_t HC_04_GetRxFlag(void);
uint8_t HC_04_GetRxDate(void);

#endif
