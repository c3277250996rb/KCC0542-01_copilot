#ifndef __FAN_H__
#define __FAN_H__

#include "common.h"

/************************ ���� PWM ��� ************************/
// VSP = P26 = EPWM5 (PG5), MUX = 0x17
// FG-IN = P22 = Timer2 CC0
#define FAN_PWM_CH          EPWM5
#define FAN_PWM_CHANNEL_MSK EPWM_CH_5_MSK
#define FAN_PWM_PIN         P26

#define FAN_CONTROL_MODE_DUTY      0
#define FAN_CONTROL_MODE_FREQUENCY 1

// 0: ʵ�ʷ���ʹ��ռ�ձȵ���; 1: FG-IN �� PWM �ػ�����ʹ��Ƶ�ʵ���
// #define FAN_CONTROL_MODE FAN_CONTROL_MODE_DUTY
#define FAN_CONTROL_MODE FAN_CONTROL_MODE_FREQUENCY

#define FAN_PWM_CLOCK_HZ       1000000UL
#define FAN_PWM_FREQUENCY_HZ   25000UL
#define FAN_PWM_PERIOD_TICK    ((U16)(FAN_PWM_CLOCK_HZ / FAN_PWM_FREQUENCY_HZ))
#define FAN_TEST_PERIOD_TICK   10000       // �ػ����Գ�ʼ 100Hz (1MHz / 10000)

/************************ ���� FG ���� ************************/
// FG-IN = P22 = CC0 (Timer2 ����ͨ�� 0)
#define FAN_FG_CAP_CH       TMR2_CC0
#define FAN_FG_PIN_CFG      P22CFG
#define FAN_FG_PORT_TRIS    P2TRIS
#define FAN_FG_PORT_UP      P2UP
#define FAN_FG_PIN_BIT      2

/************************ RPM �������� ************************/
#define FAN_FG_PULSES_PER_REV 2             // ÿת FG ������ (��׼ 4-wire fan = 2)
#define FAN_RPM_SAMPLE_MS     200           // M ��ʱ�䴰�� (ms)
#define FAN_RPM_TICK_MS       1             // Timer1 1ms ����

// ��ʽ: RPM = pulse_cnt * 60000 / (pulses_per_rev * sample_ms)
//       = pulse_cnt * 60000 / (2 * 200) = pulse_cnt * 150
#define FAN_RPM_SCALE  ((U16)(60000UL / (FAN_FG_PULSES_PER_REV * FAN_RPM_SAMPLE_MS)))

/************************ ȫ�ֱ��� ************************/
extern volatile U16 fan_rpm;                 // ��ǰ RPM (M ��������)

/************************ �������� ************************/
void Fan_Init(void);                         // ��ʼ�� PWM + FG ����
void Fan_ResetRuntime(void);                  // ���㵵λ�������״̬
void Fan_SetDuty(U8 percent);                // ����ռ�ձ� 0~100%
void Fan_SetFrequency(U16 frequency_hz);     // ���� PWM Ƶ�ʣ�����̶� 50% ռ�ձ�
void Fan_SetSpeed(U16 value);                // ���� FAN_CONTROL_MODE ѡ����ٷ�ʽ

// ISR ����� (�� isr.c �е���)
void Fan_FgCaptureIsr(void);                 // Timer2 �����ж�: �ۼ� FG ����
void Fan_RpmSampleIsr(void);                 // Timer1 1ms �ж�: M ��ʱ�䴰��

#endif
