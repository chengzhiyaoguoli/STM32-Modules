#include "stm32f4xx.h"
#include "LED.h"
#include "Serial.h"
#include "AT24C02.h"
#include "Delay.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Timer_Delay_Init();
	LED_Init();
	Serial_Init(115200);
    AT24C02_Init();

	Serial_Printf("系统启动...\r\n");
    
	LED_ON();
    
    EEPROM_Buffer[0][0] = 0x01;
    Serial_Printf("AT24C02的0地址写入值：%u\r\n", EEPROM_Buffer[0][0]);
    
    AT24C02_All_Write();
    EEPROM_Buffer[0][0] = 0x02;
    Serial_Printf("EEPROM_Buffer[0][0]更改为：%u\r\n", EEPROM_Buffer[0][0]);
    
    AT24C02_All_Read();
    Serial_Printf("AT24C02的0地址读取值：%u\r\n", EEPROM_Buffer[0][0]);
	while(1)
	{
		
	}
}
