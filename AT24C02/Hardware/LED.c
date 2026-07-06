#include "LED.h"

void LED_Init(void)
{
    RCC_AHB1PeriphClockCmd(LED_GPIO_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, Bit_SET);
}

void LED_ON(void)
{
    GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, Bit_RESET);
}

void LED_OFF(void)
{
    GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, Bit_SET);
}
