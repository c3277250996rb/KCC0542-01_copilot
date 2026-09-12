#include "buzzer.h"

/************************ 内部状态 ************************/
static bit s_buzzer_on;     // 当前开关状态 (1=鸣响, 0=关闭)

/************************ 初始化 ************************/
void Buzzer_Init(void)
{
    BUZZER_OUT;          // P23 推挽输出
    BUZZER_PIN = 0;      // 默认关闭
    s_buzzer_on = 0;
}

/************************ 鸣响 ************************/
void Buzzer_On(void)
{
    BUZZER_PIN = 1;
    s_buzzer_on = 1;
}

/************************ 关闭 ************************/
void Buzzer_Off(void)
{
    BUZZER_PIN = 0;
    s_buzzer_on = 0;
}

/************************ 设置 ************************/
void Buzzer_Set(u8 val)
{
    BUZZER_PIN = (val != 0) ? 1 : 0;
    s_buzzer_on = (val != 0);
}

/************************ 翻转 ************************/
void Buzzer_Toggle(void)
{
    BUZZER_PIN = !BUZZER_PIN;
    s_buzzer_on = !s_buzzer_on;
}

/************************ 毫秒级阻塞延时 (基于 1ms 系统滴答) ************************/
static void buzzer_delay_ms(U16 ms)
{
    while (ms--)
    {
        while (!Sys1ms);    // 等待 1ms 标志置位
        Sys1ms = 0;         // 消费该滴答
    }
}

/************************ 鸣响指定时长 (阻塞) ************************/
void Buzzer_Beep(U16 ms)
{
    Buzzer_On();
    buzzer_delay_ms(ms);
    Buzzer_Off();
}
