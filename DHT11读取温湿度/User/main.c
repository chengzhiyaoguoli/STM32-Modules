#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"

int main(void)
{
	OLED_Init();
	
	OLED_ShowString(1, 1, "Humidity:");
	
	while(1)
	{
		DHT11_Read_Data(DHT11_Data, &Temperature, &Humidity);
		OLED_ShowNum(1, 10, Humidity, 2);
	}
}
