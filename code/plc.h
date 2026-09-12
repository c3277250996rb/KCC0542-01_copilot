#ifndef __PLC_H__
#define __PLC_H__

#include "common.h"

/************************ PLC 引脚定义 ************************/
// 当前工程使用 P02 作为 PLC 输入。P26 已用于 VSP PWM 输出。
// PLC 无源控制信号:
//   短接 (触点闭合) -> 引脚被拉低 -> 开机
//   开路 (触点断开) -> 内部上拉拉高 -> 关机
#define PLC_IN_UP     P02_IN_UP     // 配置为上拉输入
#define PLC_PIN       P02           // 读取引脚电平

/************************ 参数 ************************/
#define PLC_SCAN_PERIOD_MS   100    // 扫描周期 (ms), PLC 信号慢, 不需要太快
#define PLC_DEBOUNCE_CNT     3      // 消抖次数 (3 * 100ms = 300ms)

/************************ 函数声明 ************************/
void Plc_Init(void);               // 初始化引脚
void Plc_Task(void);               // 主循环调用: 100ms 分频
U8   Plc_IsShorted(void);          // 查询当前是否短接 (1=短接=开机态, 0=开路=关机态)

#endif
