#include "alarm.h"

extern volatile unsigned int SysTickMs;

static bit s_alarm_active;
static U8 s_alternate_output;
static U16 s_last_output_tick;

static void Alarm_SetOutputs(U8 active)
{
    if (!active)
    {
        P15 = 0;
        P24 = 0;
        P25 = 0;
        return;
    }

    P15 = 1;
    P24 = s_alternate_output ? 0 : 1;
    P25 = s_alternate_output ? 1 : 0;
}

void Alarm_Init(void)
{
    P15_OUT;
    P24_OUT;
    P25_OUT;
    s_alarm_active = 0;
    s_alternate_output = 0;
    s_last_output_tick = (U16)SysTickMs;
    Alarm_SetOutputs(0);
}

void Alarm_Reset(void)
{
    s_alarm_active = 0;
    s_alternate_output = 0;
    s_last_output_tick = (U16)SysTickMs;
    Alarm_SetOutputs(0);
}

void Alarm_Task(U16 rpm, U16 alarm_rpm, bit system_on)
{
    U16 now;

    if (!system_on || alarm_rpm == 0 || rpm <= alarm_rpm)
    {
        if (s_alarm_active) Alarm_Reset();
        return;
    }

    if (!s_alarm_active)
    {
        s_alarm_active = 1;
        s_alternate_output = 0;
        s_last_output_tick = (U16)SysTickMs;
        Alarm_SetOutputs(1);
        return;
    }

    now = (U16)SysTickMs;
    if ((U16)(now - s_last_output_tick) >= ALARM_OUTPUT_PERIOD_MS)
    {
        s_last_output_tick = now;
        s_alternate_output = s_alternate_output ? 0 : 1;
        Alarm_SetOutputs(1);
    }
}

bit Alarm_IsActive(void)
{
    return s_alarm_active;
}