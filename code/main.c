/* �����Ʊ�Ƽ������ζ����� */
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

static U8 fan_tick = 0;  // ���ȵ�λ: 0=��, 1=����, 2=����
static U16 displayed_fan_rpm = 0;
static U8 key4_graphic_index = 0;
static U32 displayed_graphics;
static U8 displayed_level;
static bit displayed_alarm;
static U8 displayed_delay;
static bit display_was_settings;

static code U32 fan_level_graphics[] = {
    0,
    DISPLAY_GRAPHIC_T4 | DISPLAY_GRAPHIC_T5 | DISPLAY_GRAPHIC_T6 |
    DISPLAY_GRAPHIC_T7 | DISPLAY_GRAPHIC_T8,
    DISPLAY_GRAPHIC_T4 | DISPLAY_GRAPHIC_T5 | DISPLAY_GRAPHIC_T6 |
    DISPLAY_GRAPHIC_T7 | DISPLAY_GRAPHIC_T8 | DISPLAY_GRAPHIC_T9 |
    DISPLAY_GRAPHIC_T10 | DISPLAY_GRAPHIC_T11 | DISPLAY_GRAPHIC_T12 |
    DISPLAY_GRAPHIC_T13
};

/* KEY4 ������ѡ���ͼ�Σ�0 ��Ԫ�ر�ʾȫ���رա� */
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
    U32 graphics = fan_level_graphics[fan_tick] |
                   key4_graphic_masks[key4_graphic_index];

    if (fan_tick != 0) graphics |= DISPLAY_GRAPHIC_T1;
    DisplayGraphicsSet(
        graphics);
}

static void App_RefreshNormalDisplay(void)
{
    U32 graphics;
    U8 delay = Settings_GetRemainingSeconds() ? 1 : 0;
    bit alarm = Alarm_IsActive();

    graphics = fan_level_graphics[fan_tick] |
               key4_graphic_masks[key4_graphic_index];
    if (fan_tick != 0) graphics |= DISPLAY_GRAPHIC_T1;
    if (delay) graphics |= DISPLAY_GRAPHIC_T18;
    if (alarm) graphics |= DISPLAY_GRAPHIC_T3;

    if (display_was_settings ||
        displayed_fan_rpm != fan_rpm ||
        displayed_level != fan_tick ||
        displayed_alarm != alarm ||
        displayed_delay != delay ||
        displayed_graphics != graphics)
    {
        displayed_fan_rpm = fan_rpm;
        displayed_level = fan_tick;
        displayed_alarm = alarm;
        displayed_delay = delay;
        displayed_graphics = graphics;
        display_was_settings = 0;
        DisplayShowNormal(fan_rpm, fan_tick, graphics);
    }
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

    LED_Init();         // ��ʼ�� LED ���� (P01)
    LED_On();           // ���� LED

    Buzzer_Init();      // ��ʼ�������� (P23 �������, Ĭ�Ϲر�)

    Key_InitTimer1();   // ��ʼ�� Timer1 (1ms �ж���������ɨ��)
    Key_Init();         // ��ʼ�� 4 ������ (SW1~SW4)

    Power_Init();       // ��Դ״̬��ʼ��
    Plc_Init();         // PLC ��Դ�����źų�ʼ�� (P02)
    Fan_Init();         // ���� PWM + FG �ٶȷ�����ʼ��
    Settings_Init();    // ���ò˵�״̬��
    Alarm_Init();       // ���ٱ������

    Display4Digits(0000);

    while(1)
    {
        Key_Task();     // 1ms ���� -> 10ms ɨ�����а���
        Buzzer_Task();
        if (Settings_Task())
        {
            fan_tick = 0;
            Fan_ApplyLevel();
        }
        Plc_Task();     // 10ms ���� -> 100ms ɨ�� PLC �ź�
        Alarm_Task(fan_rpm, Settings_GetAlarmRpm(),
               (sys_state == E_SYS_ON) ? 1 : 0);

        if (!Settings_IsActive())
            App_RefreshNormalDisplay();
        else
            display_was_settings = 1;

        // ====== �¼����� ======
        {
            KeyEvent_t evt;
            while (1)
            {
                evt = Key_GetEvent();
                if (evt.type == KEY_EVT_NONE) break;

                // KEY1 �̶���Ϊ��Դ��
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

                /* ����״̬�������� KEY2/3/4�����������е�λ�߼���ϡ� */
                if (evt.id == KEY4 || Settings_IsActive())
                {
                    Settings_HandleKeyEvent(evt);
                    if (Settings_IsActive() || evt.id == KEY4) continue;
                }

                // �ػ�״̬�º������зǵ�Դ��
                if (sys_state == E_SYS_OFF) continue;

                // ====== ����״̬��: ���� KEY2 / KEY3 / KEY4 ======
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
