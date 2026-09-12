#include "buzzer.h"

/************************ �ڲ�״̬ ************************/
static bit s_buzzer_on;     // ��ǰ����״̬ (1=����, 0=�ر�)
static U16 s_buzzer_until;
extern volatile unsigned int SysTickMs;

/************************ ��ʼ�� ************************/
void Buzzer_Init(void)
{
    BUZZER_OUT;          // P23 �������
    BUZZER_PIN = 0;      // Ĭ�Ϲر�
    s_buzzer_on = 0;
    s_buzzer_until = 0;
}

/************************ ���� ************************/
void Buzzer_On(void)
{
    BUZZER_PIN = 1;
    s_buzzer_on = 1;
}

/************************ �ر� ************************/
void Buzzer_Off(void)
{
    BUZZER_PIN = 0;
    s_buzzer_on = 0;
}

/************************ ���� ************************/
void Buzzer_Set(u8 val)
{
    BUZZER_PIN = (val != 0) ? 1 : 0;
    s_buzzer_on = (val != 0);
}

/************************ ��ת ************************/
void Buzzer_Toggle(void)
{
    BUZZER_PIN = !BUZZER_PIN;
    s_buzzer_on = !s_buzzer_on;
}

/************************ ����ָ��ʱ�� (������) ************************/
void Buzzer_Beep(U16 ms)
{
    Buzzer_On();
    s_buzzer_until = (U16)SysTickMs + ms;
}

void Buzzer_Task(void)
{
    if (s_buzzer_on &&
        (U16)((U16)SysTickMs - s_buzzer_until) < 0x8000)
        Buzzer_Off();
}
