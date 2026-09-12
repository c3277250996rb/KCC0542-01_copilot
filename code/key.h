#ifndef __KEY_H__
#define __KEY_H__

#include "common.h"

/************************ 按键硬件配置 ************************/
// 原理图:
//   SW1 -> P04/CIN1   (key 1)
//   SW2 -> P30/C0P4   (key 2)
//   SW3 -> P21        (key 3)
//   SW4 -> P13        (key 4)
// 按键按下接地, 外部上拉电阻, 低电平有效

typedef enum {
    KEY1 = 0,   // SW1 - P04
    KEY2 = 1,   // SW2 - P30
    KEY3 = 2,   // SW3 - P21
    KEY4 = 3,   // SW4 - P13
    KEY_COUNT = 4
} KeyId_t;

/************************ 按键参数 ************************/
#define KEY_SCAN_PERIOD_MS    10    // 扫描周期 (ms)
#define KEY_DEBOUNCE_CNT      3     // 消抖次数 (3 * 10ms = 30ms)
#define KEY_LONG_PRESS_MS     1500  // 长按判定时间 (ms)
#define KEY_HOLD_PERIOD_MS    200   // 按住周期刷新间隔 (ms)

/************************ 按键事件类型 ************************/
typedef enum {
    KEY_EVT_NONE       = 0,  // 无事件
    KEY_EVT_PRESS      = 1,  // 按下 (消抖后首次按下瞬间)
    KEY_EVT_RELEASE    = 2,  // 释放
    KEY_EVT_LONG_PRESS = 3,  // 长按
    KEY_EVT_HOLD       = 4,  // 按住周期刷新 (每 200ms 一次)
} KeyEventType_t;

/************************ 按键事件结构体 ************************/
typedef struct {
    KeyId_t        id;      // 按键 ID (KEY1 ~ KEY4)
    KeyEventType_t type;    // 事件类型
    U16            hold_ms; // 触发事件时的按下时长 (ms)
} KeyEvent_t;

/************************ 函数声明 ************************/
void        Key_InitTimer1(void);              // 初始化 Timer1 (1ms 中断驱动)
void        Key_Task(void);                    // 主循环调用: 10ms 扫描分频
void        Key_Init(void);                    // 初始化所有 4 个按键的 GPIO
void        Key_Scan(void);                    // 扫描所有按键 (内部调用)
KeyEvent_t  Key_GetEvent(void);                // 获取并消费一个待处理事件
bit         Key_IsPressed(KeyId_t id);         // 查询指定按键当前是否按住
U16         Key_GetHoldTime(KeyId_t id);       // 获取指定按键按住时长 (ms)
bit         Key_IsScanTick(void);              // 查询 Key_Scan 刚执行过 (10ms 节拍标志)

#endif
