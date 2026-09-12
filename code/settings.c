#include "settings.h"
#include "display.h"
#include "buzzer.h"

extern volatile unsigned int SysTickMs;

static SettingsState_t s_state;
static U16 s_time;
static U16 s_alarm_rpm;
static U8 s_remaining_seconds;
static U16 s_last_second_tick;
static U16 s_last_blink_tick;
static bit s_icon_on;

static U16 Increase(U16 value, U16 maximum)
{
    if (value < maximum) value++;
    return value;
}

static U16 Decrease(U16 value, U16 minimum)
{
    if (value > minimum) value--;
    return value;
}

static U16 IncreaseAlarmRpm(U16 value)
{
    if (value + SETTINGS_ALARM_RPM_STEP > SETTINGS_ALARM_RPM_MAX)
        return SETTINGS_ALARM_RPM_MAX;
    return value + SETTINGS_ALARM_RPM_STEP;
}

static U16 DecreaseAlarmRpm(U16 value)
{
    if (value < SETTINGS_ALARM_RPM_STEP)
        return SETTINGS_ALARM_RPM_MIN;
    return value - SETTINGS_ALARM_RPM_STEP;
}

static void Settings_RefreshDisplay(void)
{
    if (s_state == SETTINGS_TIMER)
        DisplayShowTimer((U8)s_time, s_icon_on);
    else if (s_state == SETTINGS_ALARM_RPM)
        DisplayShowAlarmRpm(s_alarm_rpm, s_icon_on);
}

static void Settings_RefreshCountdown(void)
{
    Display9Digit10Seg((U8)(s_remaining_seconds % 10));
    Display10Digit10Seg((U8)(s_remaining_seconds / 10));
}

void Settings_Init(void)
{
    s_state = SETTINGS_IDLE;
    s_time = 0;
    s_alarm_rpm = 3000;
    s_remaining_seconds = 0;
    s_last_second_tick = 0;
    s_last_blink_tick = 0;
    s_icon_on = 1;
}

void Settings_Reset(void)
{
    s_state = SETTINGS_IDLE;
    s_remaining_seconds = 0;
    s_last_second_tick = (U16)SysTickMs;
    s_last_blink_tick = (U16)SysTickMs;
    s_icon_on = 1;
    Settings_RefreshCountdown();
}

bit Settings_Task(void)
{
    U16 now;

    now = (U16)SysTickMs;
    if (s_state != SETTINGS_IDLE)
    {
        if ((U16)(now - s_last_blink_tick) >= 500)
        {
            s_last_blink_tick = now;
            s_icon_on = s_icon_on ? 0 : 1;
            Settings_RefreshDisplay();
        }
        return 0;
    }

    if (s_remaining_seconds == 0) return 0;

    if ((U16)(now - s_last_second_tick) < 1000) return 0;

    s_last_second_tick = now;
    s_remaining_seconds--;
    Settings_RefreshCountdown();
    return (s_remaining_seconds == 0) ? 1 : 0;
}

void Settings_HandleKeyEvent(KeyEvent_t event)
{
    if (event.id == KEY4 && event.type == KEY_EVT_LONG_PRESS)
    {
        if (s_state == SETTINGS_IDLE)
        {
            s_state = SETTINGS_TIMER;
            s_last_blink_tick = (U16)SysTickMs;
            s_icon_on = 1;
        }
        else
        {
            s_state = SETTINGS_IDLE;
            s_remaining_seconds = (U8)s_time;
            s_last_second_tick = (U16)SysTickMs;
            s_icon_on = 1;
        }
        Buzzer_Beep(300);
        if (s_state != SETTINGS_IDLE) Settings_RefreshDisplay();
        else Settings_RefreshCountdown();
        return;
    }

    if (s_state == SETTINGS_IDLE) return;

    if (event.id == KEY4 && event.type == KEY_EVT_PRESS)
    {
        if (s_state == SETTINGS_TIMER)
            s_state = SETTINGS_ALARM_RPM;
        else
            s_state = SETTINGS_TIMER;
        Buzzer_Beep(50);
        Settings_RefreshDisplay();
        return;
    }

    if ((event.type != KEY_EVT_PRESS) && (event.type != KEY_EVT_HOLD)) return;

    if (event.id == KEY2)
    {
        if (s_state == SETTINGS_TIMER)
            s_time = Decrease(s_time, SETTINGS_TIME_MIN);
        else
            s_alarm_rpm = DecreaseAlarmRpm(s_alarm_rpm);
        Buzzer_Beep(20);
    }
    else if (event.id == KEY3)
    {
        if (s_state == SETTINGS_TIMER)
            s_time = Increase(s_time, SETTINGS_TIME_MAX);
        else
            s_alarm_rpm = IncreaseAlarmRpm(s_alarm_rpm);
        Buzzer_Beep(20);
    }
    else
    {
        return;
    }

    Settings_RefreshDisplay();
}

SettingsState_t Settings_GetState(void)
{
    return s_state;
}

bit Settings_IsActive(void)
{
    return (s_state == SETTINGS_IDLE) ? 0 : 1;
}

U16 Settings_GetTime(void)
{
    return s_time;
}

U8 Settings_GetRemainingSeconds(void)
{
    return s_remaining_seconds;
}

U16 Settings_GetAlarmRpm(void)
{
    return s_alarm_rpm;
}