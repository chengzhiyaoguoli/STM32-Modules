#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "OLED_Data.h"
#include "HC_04.h"
#include "Delay.h"

//编码格式：UTF-8

int main(void)
{
	OLED_Init();
	HC_04_Init();
	
	uint8_t i = 0;
	
	OLED_Printf(0, 0, OLED_8X16, "Hallo, World!");
	OLED_Update();
	
	while (1)
	{
		if (i >= 255)
		{
			i = 0;
		}
		i ++;
		OLED_Printf(0, 16, OLED_8X16, "%d", i);
		OLED_Update();
		HC_04_SendByte(i);
		
		Delay_ms(1000);
	}
}
