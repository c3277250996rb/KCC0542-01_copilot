/* 紫悦云宝萍琪珍奇嘉儿柔柔 */
#include "cms8s6990.h"
#include "common.h"
#include "key.h"
#include "led.h"
#include "buzzer.h"
#include "power.h"
#include "plc.h"
#include "fan.h"
#include "display.h"
#include "settings.h"
#include "alarm.h"

static U8 fan_tick = 0;  // 风扇档位: 0=关, 1=低速, 2=高速
static U16 displayed_fan_rpm = 0;
static U8 key4_graphic_index = 0;

static code U32 fan_level_graphics[] = {
    0,
    DISPLAY_GRAPHIC_T4 | DISPLAY_GRAPHIC_T5 | DISPLAY_GRAPHIC_T6 |
    DISPLAY_GRAPHIC_T7 | DISPLAY_GRAPHIC_T8,
    DISPLAY_GRAPHIC_T4 | DISPLAY_GRAPHIC_T5 | DISPLAY_GRAPHIC_T6 |
    DISPLAY_GRAPHIC_T7 | DISPLAY_GRAPHIC_T8 | DISPLAY_GRAPHIC_T9 |
    DISPLAY_GRAPHIC_T10 | DISPLAY_GRAPHIC_T11 | DISPLAY_GRAPHIC_T12 |
    DISPLAY_GRAPHIC_T13
};

/* KEY4 按键可选择的图形，0 号元素表示全部关闭。 */
static code U32 key4_graphic_masks[] = {
    0,
    DISPLAY_GRAPHIC_T1,
    DISPLAY_GRAPHIC_T2,
    DISPLAY_GRAPHIC_T3,
    DISPLAY_GRAPHIC_T14,
    DISPLAY_GRAPHIC_T15,
    DISPLAY_GRAPHIC_T16,
    DISPLAY_GRAPHIC_T17,
    DISPLAY_GRAPHIC_T18,
    DISPLAY_GRAPHIC_T19,
    DISPLAY_GRAPHIC_T20,
    DISPLAY_GRAPHIC_T21,
    DISPLAY_GRAPHIC_T22,
    DISPLAY_GRAPHIC_T23
};
#define KEY4_GRAPHIC_COUNT 14

static void Fan_RefreshDisplay(void)
{
    DisplayGraphicsSet(
        fan_level_graphics[fan_tick] |
        key4_graphic_masks[key4_graphic_index]);
}

static void Fan_ApplyLevel(void)
{
    if (fan_tick == 0)
    {
        Fan_SetSpeed(0);
    }
    else if (fan_tick == 1)
    {
        Fan_SetSpeed(40);
    }
    else
    {
        Fan_SetSpeed(80);
    }

    Display6Digit10Seg(fan_tick);
    Fan_RefreshDisplay();
}

int main(void)
{
    LCD_DATA_OUT;
    LCD_WR_OUT;
    LCD_CS_OUT;
    BACK_LIGHT_OUT;
    Init_1621();
    HT1621_all_off(16);
    BACK_LIGHT_SET(1);

    LED_Init();         // 初始化 LED 引脚 (P01)
    LED_On();           // 点亮 LED

    Buzzer_Init();      // 初始化蜂鸣器 (P23 推挽输出, 默认关闭)

    Key_InitTimer1();   // 初始化 Timer1 (1ms 中断驱动按键扫描)
    Key_Init();         // 初始化 4 个按键 (SW1~SW4)

    Power_Init();       // 电源状态初始化
    Plc_Init();         // PLC 无源控制信号初始化 (P02)
    Fan_Init();         // 风扇 PWM + FG 速度反馈初始化
    Settings_Init();    // 设置菜单状态机
    Alarm_Init();       // 超速报警输出

    Display4Digits(0000);

    while(1)
    {
        Key_Task();     // 1ms 节拍 -> 10ms 扫描所有按键
        if (Settings_Task())
        {
            fan_tick = 0;
            Fan_ApplyLevel();
        }
        Plc_Task();     // 10ms 节拍 -> 100ms 扫描 PLC 信号
        Alarm_Task(fan_rpm, Settings_GetAlarmRpm(),
               (sys_state == E_SYS_ON) ? 1 : 0);

        if (!Settings_IsActive() && (fan_rpm != displayed_fan_rpm))
        {
            displayed_fan_rpm = fan_rpm;
            Display4Digits(displayed_fan_rpm);
        }

        // ====== 事件处理 ======
        {
            KeyEvent_t evt;
            while (1)
            {
                evt = Key_GetEvent();
                if (evt.type == KEY_EVT_NONE) break;

                // KEY1 固定作为电源键
                if (evt.id == KEY1)
                {
                    if (evt.type == KEY_EVT_PRESS)
                    {
                        Buzzer_Beep(50);
                        Power_Toggle();
                    }
                    else if (evt.type == KEY_EVT_LONG_PRESS)
                    {
                        Buzzer_Beep(1000);
                    }
                    continue;
                }

                /* 设置状态优先消费 KEY2/3/4，避免与运行档位逻辑耦合。 */
                if (evt.id == KEY4 || Settings_IsActive())
                {
                    Settings_HandleKeyEvent(evt);
                    if (Settings_IsActive() || evt.id == KEY4) continue;
                }

                // 关机状态下忽略所有非电源键
                if (sys_state == E_SYS_OFF) continue;

                // ====== 开机状态下: 处理 KEY2 / KEY3 / KEY4 ======
                switch (evt.id)
                {
                    case KEY2:
                        switch (evt.type)
                        {
                            case KEY_EVT_PRESS:
                                Buzzer_Beep(50);
                                if (fan_tick > 0) fan_tick--;
                                Fan_ApplyLevel();
                                break;
                            case KEY_EVT_LONG_PRESS: Buzzer_Beep(1000); break;
                            default: break;
                        }
                        break;

                    case KEY3:
                        switch (evt.type)
                        {
                            case KEY_EVT_PRESS:
                                Buzzer_Beep(50);
                                if (fan_tick < 2) fan_tick++;
                                Fan_ApplyLevel();
                                break;
                            case KEY_EVT_LONG_PRESS:
                                Buzzer_Beep(1000);
                                break;
                            default:
                                break;
                        }
                        break;

                    case KEY4:
                        break;

                    default: break;
                }
            }
        }
    }
}
