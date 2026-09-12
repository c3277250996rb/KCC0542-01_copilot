#include "power.h"
#include "ht1621b.h"
#include "led.h"
#include "buzzer.h"
#include "fan.h"
#include "display.h"
#include "settings.h"
#include "alarm.h"

/************************ 系统状态变量 ************************/
U8 sys_state = E_SYS_ON;   // 默认开机

/************************ 开机动作 ************************/
void Power_On(void)
{
    LCDon();
    HT1621_all_on(16);
    BACK_LIGHT_SET(1);
    LED_On();
    Fan_ResetRuntime();
    Settings_Reset();
    Alarm_Reset();
    sys_state = E_SYS_ON;
    DisplayGraphicsRefresh();
}

/************************ 关机动作 ************************/
void Power_Off(void)
{
    LCDoff();
    HT1621_all_off(16);
    BACK_LIGHT_SET(0);
    LED_Off();
    Buzzer_Off();
    Fan_ResetRuntime();
    Settings_Reset();
    Alarm_Reset();
    sys_state = E_SYS_OFF;
}

/************************ 切换开关机 ************************/
void Power_Toggle(void)
{
    if (sys_state == E_SYS_ON)
        Power_RequestOff(POWER_SRC_KEY);
    else
        Power_RequestOn(POWER_SRC_KEY);
}

/************************ 统一电源请求入口 ************************/
void Power_RequestOn(PowerRequestSource_t source)
{
    source = source;
    if (sys_state != E_SYS_ON)
    {
        Power_On();
    }
}

void Power_RequestOff(PowerRequestSource_t source)
{
    source = source;
    if (sys_state != E_SYS_OFF)
    {
        Power_Off();
    }
}

/************************ 初始化 ************************/
void Power_Init(void)
{
    sys_state = E_SYS_ON;
}
