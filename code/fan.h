#ifndef __FAN_H__
#define __FAN_H__

#include "common.h"

/************************ 风扇 PWM 输出 ************************/
// VSP = P26 = EPWM5 (PG5), MUX = 0x17
// FG-IN = P22 = Timer2 CC0
#define FAN_PWM_CH          EPWM5
#define FAN_PWM_CHANNEL_MSK EPWM_CH_5_MSK
#define FAN_PWM_PIN         P26

#define FAN_CONTROL_MODE_DUTY      0
#define FAN_CONTROL_MODE_FREQUENCY 1

// 0: 实际风扇使用占空比调速; 1: FG-IN 与 PWM 回环测试使用频率调速
// #define FAN_CONTROL_MODE FAN_CONTROL_MODE_DUTY
#define FAN_CONTROL_MODE FAN_CONTROL_MODE_FREQUENCY

#define FAN_PWM_CLOCK_HZ       1000000UL
#define FAN_PWM_FREQUENCY_HZ   25000UL
#define FAN_PWM_PERIOD_TICK    ((U16)(FAN_PWM_CLOCK_HZ / FAN_PWM_FREQUENCY_HZ))
#define FAN_TEST_PERIOD_TICK   10000       // 回环测试初始 100Hz (1MHz / 10000)

/************************ 风扇 FG 输入 ************************/
// FG-IN = P22 = CC0 (Timer2 捕获通道 0)
#define FAN_FG_CAP_CH       TMR2_CC0
#define FAN_FG_PIN_CFG      P22CFG
#define FAN_FG_PORT_TRIS    P2TRIS
#define FAN_FG_PORT_UP      P2UP
#define FAN_FG_PIN_BIT      2

/************************ RPM 采样参数 ************************/
#define FAN_FG_PULSES_PER_REV 2             // 每转 FG 脉冲数 (标准 4-wire fan = 2)
#define FAN_RPM_SAMPLE_MS     500           // M 法时间窗口 (ms)
#define FAN_RPM_TICK_MS       1             // Timer1 1ms 节拍

// 公式: RPM = pulse_cnt * 60000 / (pulses_per_rev * sample_ms)
//       = pulse_cnt * 60000 / (2 * 500) = pulse_cnt * 60
#define FAN_RPM_SCALE  ((U16)(60000UL / (FAN_FG_PULSES_PER_REV * FAN_RPM_SAMPLE_MS)))

/************************ 全局变量 ************************/
extern volatile U16 fan_rpm;                 // 当前 RPM (M 法计算结果)

/************************ 函数声明 ************************/
void Fan_Init(void);                         // 初始化 PWM + FG 捕获
void Fan_ResetRuntime(void);                  // 清零档位相关运行状态
void Fan_SetDuty(U8 percent);                // 设置占空比 0~100%
void Fan_SetFrequency(U16 frequency_hz);     // 设置 PWM 频率，输出固定 50% 占空比
void Fan_SetSpeed(U16 value);                // 根据 FAN_CONTROL_MODE 选择调速方式

// ISR 接入点 (在 isr.c 中调用)
void Fan_FgCaptureIsr(void);                 // Timer2 捕获中断: 累加 FG 脉冲
void Fan_RpmSampleIsr(void);                 // Timer1 1ms 中断: M 法时间窗口

#endif
