#include "Delay.h"

/*使用TIM6基本定时器作为延时函数时钟源，APB1总线时钟频率42MHz
如果该总线的预分频系数（Prescaler） 等于 1：
定时器时钟 = 总线时钟 （x1）
如果该总线的预分频系数 不等于 1（比如 2, 4, 8...）：
定时器时钟 = 总线时钟 x 2 （倍频）
根据上述规则，TIM6时钟频率为84MHz*/
void Timer_Delay_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 65536 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;                   //分频计数器频率1MHz，也就是1us，计数器每次自增都是1us
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM6, ENABLE);
}

// /*最大65000us，也就是65ms，再高就会超出Period最大值陷入死循环*/
// void Delay_us(uint16_t us)
// {
//     TIM_SetCounter(TIM6, 0);
//     TIM_Cmd(TIM6, ENABLE);
//     while(TIM_GetCounter(TIM6) < us);
//     TIM_Cmd(TIM6, DISABLE);
// }

// 延时：利用差值计算，不需要开关定时器，也不需要清零
void Delay_us(uint16_t us)
{
    uint16_t start = TIM6->CNT; // 记录刚进来的时间
    
    // 利用 uint16_t 的溢出特性，即使 CNT 翻转（从65535变回0），差值计算依然正确
    while((uint16_t)(TIM6->CNT - start) < us); 
}

/*没有理论最大值，在uint32_t范围内即可*/
void Delay_ms(uint32_t ms)
{
    while(ms --)
    {
        Delay_us(1000);
    }
}
