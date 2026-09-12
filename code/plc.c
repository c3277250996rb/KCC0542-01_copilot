#include "plc.h"
#include "power.h"
#include "key.h"

/************************ 内部状态 ************************/
static U8 plc_debounce_cnt;   // 消抖计数器
static U8 plc_stable;         // 稳定状态: 1=短接(LOW), 0=开路(HIGH)
static U8 plc_raw;            // 当前消抖中的原始状态
static U8 plc_scan_cnt;       // 10ms -> 100ms 分频

/************************ 初始化 ************************/
void Plc_Init(void)
{
    PLC_IN_UP;                // P02 上拉输入
    plc_debounce_cnt = 0;
    plc_stable       = 0;     // 默认视为开路
    plc_raw          = 0;
    plc_scan_cnt     = 0;
}

/************************ PLC 扫描 (100ms 周期, 由 Plc_Task 分频调用) ************************/
static void Plc_Scan(void)
{
    // 1) 读取引脚: LOW=短接, HIGH=开路 -> 反相为 1/0
    U8 raw = (PLC_PIN == 0) ? 1 : 0;

    // 2) 消抖 (3 次, 即 300ms)
    if (raw != plc_raw)
    {
        plc_raw = raw;
        plc_debounce_cnt = 1;
        return;
    }

    if (plc_debounce_cnt < PLC_DEBOUNCE_CNT)
    {
        plc_debounce_cnt++;
        return;
    }

    // 3) 稳定状态跳变才触发动作 (边沿触发, 不覆盖按键)
    if (plc_raw != plc_stable)
    {
        plc_stable = plc_raw;
        if (plc_stable)   // 稳定短接 -> PLC 命令开机
        {
            Power_RequestOn(POWER_SRC_PLC);
        }
        else              // 稳定开路 -> PLC 命令关机
        {
            Power_RequestOff(POWER_SRC_PLC);
        }
    }
}

/************************ 主循环任务 ************************/
// 建议放在 Key_Task() 之后调用, 利用 Key_Scan() 的 10ms 节拍分频
// 每 10 次 Key_Scan = 100ms 执行一次 Plc_Scan
void Plc_Task(void)
{
    if (Key_IsScanTick())    // Key_Scan 刚执行过 (10ms 节拍)
    {
        if (++plc_scan_cnt >= PLC_SCAN_PERIOD_MS / KEY_SCAN_PERIOD_MS)
        {
            plc_scan_cnt = 0;
            Plc_Scan();
        }
    }
}

/************************ 查询当前短接状态 ************************/
U8 Plc_IsShorted(void)
{
    return plc_stable;
}
