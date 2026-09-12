#ifndef __POWER_H__
#define __POWER_H__

#include "common.h"

/************************ 系统状态 ************************/
extern U8 sys_state;   // E_SYS_ON / E_SYS_OFF (GLOBAL.H 中定义)

/************************ 电源请求来源 ************************/
typedef enum {
	POWER_SRC_KEY = 0,
	POWER_SRC_PLC,
	POWER_SRC_TIMER
} PowerRequestSource_t;

/************************ 函数声明 ************************/
void Power_Init(void);           // 初始化 (默认开机状态)
void Power_On(void);             // 开机: 打开 LCD/背光/LED
void Power_Off(void);            // 关机: 关闭 LCD/背光/LED/蜂鸣器
void Power_Toggle(void);         // 切换开关机
void Power_RequestOn(PowerRequestSource_t source);
void Power_RequestOff(PowerRequestSource_t source);

#endif
