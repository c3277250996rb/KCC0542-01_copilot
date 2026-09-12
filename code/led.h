#ifndef __LED_H__
#define __LED_H__

#include "common.h"

/************************ LED 引脚定义 ************************/
// 原理图 KCC0542-01:
//   LED1/LED2 阳极接 +5V, 阴极经 R5/R6(1K) 汇合后接 Q1(S8050) 集电极
//   Q1 基极经 R7(1K) 接 P01, R9(10K) 为基极下拉
//   P01 输出高电平 -> Q1 导通 -> LED1/LED2 同时点亮 (高电平有效)
#define LED_OUT     P01_OUT
#define LED_PIN     P01

/************************ 函数声明 ************************/
void LED_Init(void);     // 初始化 LED 引脚 (P01 推挽输出, 默认熄灭)
void LED_On(void);       // 点亮 LED1/LED2
void LED_Off(void);      // 熄灭
void LED_Set(u8 val);    // 1=亮, 0=灭
void LED_Toggle(void);   // 翻转 LED 状态 (亮<->灭)

#endif
