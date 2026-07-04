#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "NRF24L01.h"

uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	Key_Init();
	NRF24L01_Init();
	
	while (1)
	{	
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			NRF24L01_TxPacket[0] ++;
			NRF24L01_TxPacket[1] ++;
			NRF24L01_TxPacket[2] ++;
			NRF24L01_TxPacket[3] ++;

			NRF24L01_Send();
		}
		if (NRF24L01_Receive())
		{
			OLED_ShowHexNum(0, 0, NRF24L01_RxPacket_P0[0], 2, OLED_8X16);
			OLED_ShowHexNum(0, 16, NRF24L01_RxPacket_P0[1], 2, OLED_8X16);
			OLED_ShowHexNum(0, 32, NRF24L01_RxPacket_P0[2], 2, OLED_8X16);
			OLED_ShowHexNum(0, 48, NRF24L01_RxPacket_P0[3], 2, OLED_8X16);

			OLED_ShowHexNum(32, 0, NRF24L01_RxPacket_P1[0], 2, OLED_8X16);
			OLED_ShowHexNum(32, 16, NRF24L01_RxPacket_P1[1], 2, OLED_8X16);
			OLED_ShowHexNum(32, 32, NRF24L01_RxPacket_P1[2], 2, OLED_8X16);
			OLED_ShowHexNum(32, 48, NRF24L01_RxPacket_P1[3], 2, OLED_8X16);

			OLED_ShowHexNum(64, 0, NRF24L01_RxPacket_P2[0], 2, OLED_8X16);
			OLED_ShowHexNum(64, 16, NRF24L01_RxPacket_P2[1], 2, OLED_8X16);
			OLED_ShowHexNum(64, 32, NRF24L01_RxPacket_P2[2], 2, OLED_8X16);
			OLED_ShowHexNum(64, 48, NRF24L01_RxPacket_P2[3], 2, OLED_8X16);

			OLED_ShowHexNum(96, 0, NRF24L01_RxPacket_P3[0], 2, OLED_8X16);
			OLED_ShowHexNum(96, 16, NRF24L01_RxPacket_P3[1], 2, OLED_8X16);
			OLED_ShowHexNum(96, 32, NRF24L01_RxPacket_P3[2], 2, OLED_8X16);
			OLED_ShowHexNum(96, 48, NRF24L01_RxPacket_P3[3], 2, OLED_8X16);
		}
		OLED_Update();
	}
}
