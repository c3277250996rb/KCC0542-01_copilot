#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "common.h"

/************************ 蜂鸣器引脚定义 ************************/
// 原理图:
//   BUZ 阳极接 +5V, 阴极接 Q2(S8050) 集电极
//   Q2 基极经 R8(1K) 接 BUZZER 信号, 发射极接地
//   有源蜂鸣器: P23 输出高电平 -> Q2 导通 -> 蜂鸣器鸣响 (高电平有效)
//   只需 GPIO 开关控制, 无需 PWM/方波
#define BUZZER_OUT     P23_OUT     // P23 推挽输出
#define BUZZER_PIN     P23

/************************ 函数声明 ************************/
void Buzzer_Init(void);      // 初始化 (P23 推挽输出, 默认关闭)
void Buzzer_On(void);        // 鸣响
void Buzzer_Off(void);       // 关闭
void Buzzer_Set(u8 val);     // 1=鸣响, 0=关闭
void Buzzer_Toggle(void);    // 翻转状态
void Buzzer_Beep(u16 ms);    // 鸣响指定时长 (阻塞)

#endif
