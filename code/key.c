#include "key.h"

/************************ 每个按键的独立状态 ************************/
// 注意: C51 不允许在含函数指针的 struct 中使用 bit 成员
// 所以所有布尔标志统一用 U8 存放
typedef struct {
    KeyId_t   id;
    // 扫描状态机变量 (U8 代替 bit, 避免 C51 存储类限制)
    U8        debounce_cnt;        // 消抖计数器
    U8        last_state;          // 上一次稳定状态 (1=按下, 0=释放)
    U8        current_state;       // 消抖中的原始状态
    U16       hold_ms;             // 按住累计时长 (ms)
    U8        long_triggered;      // 长按是否已触发
    U16       hold_period;         // 按住周期累加器
} KeyState_t;

/************************ 按键数组 ************************/
static KeyState_t s_keys[KEY_COUNT];
#define KEY_EVENT_QUEUE_SIZE 8
static KeyEvent_t s_event_queue[KEY_EVENT_QUEUE_SIZE];
static U8 s_event_queue_head;
static U8 s_event_queue_tail;
static U8 s_event_queue_count;

/* 1ms 中断标志 (ISR 置位, Key_Task 消费) */
bit Sys1ms;
static U8 s_key_scan_cnt;   // 10ms 扫描分频计数器
static U8 s_scan_tick;      // Key_Scan 刚执行过的一次性节拍标志

static void Key_QueueEvent(KeyId_t id, KeyEventType_t type, U16 hold_ms)
{
    if (s_event_queue_count >= KEY_EVENT_QUEUE_SIZE) return;

    s_event_queue[s_event_queue_tail].id = id;
    s_event_queue[s_event_queue_tail].type = type;
    s_event_queue[s_event_queue_tail].hold_ms = hold_ms;
    s_event_queue_tail++;
    if (s_event_queue_tail >= KEY_EVENT_QUEUE_SIZE)
    {
        s_event_queue_tail = 0;
    }
    s_event_queue_count++;
}

/************************ 引脚读取 (低电平有效) ************************/
static U8 read_pin(KeyId_t id)
{
    switch (id)
    {
        case KEY1: return (P04 == 0) ? 1 : 0;
        case KEY2: return (P31 == 0) ? 1 : 0;
        case KEY3: return (P17 == 0) ? 1 : 0;
        case KEY4: return (P13 == 0) ? 1 : 0;
        default:   return 0;
    }
}

/************************ 引脚初始化 ************************/
static void init_pin(KeyId_t id)
{
    switch (id)
    {
        case KEY1: P04_IN_UP; break;
        case KEY2: P31_IN_UP; break;
        case KEY3: P17_IN_UP; break;
        case KEY4: P13_IN_UP; break;
        default: break;
    }
}

/************************ Timer1 初始化 (产生 1ms 中断) ************************/
// Fsys=24MHz, 12 分频 -> 2MHz, 1ms = 2000 计数, 初值 = 0xF830
void Key_InitTimer1(void)
{
    TMR_ConfigRunMode(TMR1, TMR_MODE_TIMING, TMR_TIM_16BIT);
    TMR_ConfigTimerClk(TMR1, TMR_CLK_DIV_12);
    TMR_ConfigTimerPeriod(TMR1, 0xF8, 0x30);
    TMR_EnableOverflowInt(TMR1);
    TMR_Start(TMR1);
    EA = 1;          // 开总中断
}

/************************ 主循环任务: 10ms 分频 ************************/
// 在主循环中调用, 内部通过 Sys1ms 标志分频到 10ms 调 Key_Scan
void Key_Task(void)
{
    if (Sys1ms)
    {
        Sys1ms = 0;
        if (++s_key_scan_cnt >= KEY_SCAN_PERIOD_MS)
        {
            s_key_scan_cnt = 0;
            s_scan_tick = 1;
            Key_Scan();
        }
    }
}

/************************ 查询/消费 Key_Scan 节拍 ************************/
// 返回 1 表示 Key_Scan 刚执行过 (在 Key_Task 调用时置位, 读取后清零)
// 用于 Plc_Task 等其他模块复用 10ms 基准
bit Key_IsScanTick(void)
{
    bit t = s_scan_tick;
    s_scan_tick = 0;
    return t;
}

/************************ 初始化所有按键 ************************/
void Key_Init(void)
{
    U8 i;
    for (i = 0; i < KEY_COUNT; i++)
    {
        s_keys[i].id               = (KeyId_t)i;
        init_pin(s_keys[i].id);    // 配置为上拉输入
        s_keys[i].debounce_cnt     = 0;
        s_keys[i].last_state       = 0; // 初始视为未按下
        s_keys[i].current_state    = 0;
        s_keys[i].hold_ms          = 0;
        s_keys[i].long_triggered   = 0;
        s_keys[i].hold_period      = 0;
    }
    s_event_queue_head = 0;
    s_event_queue_tail = 0;
    s_event_queue_count = 0;
}

/************************ 扫描单个按键 (状态机) ************************/
static void ScanOneKey(KeyState_t *k)
{
    // 1) 读取当前引脚原始电平 (1=按下, 0=释放)
    U8 raw = read_pin(k->id);

    // 2) 消抖: 连续 N 次相同才认为稳定
    if (raw != k->current_state)
    {
        k->current_state = raw;
        k->debounce_cnt  = 1;
        return;
    }

    if (k->debounce_cnt < KEY_DEBOUNCE_CNT)
    {
        k->debounce_cnt++;
        return;
    }

    // 3) 稳定状态处理
    if (k->current_state != k->last_state)
    {
        // 状态跳变
        if (k->current_state)   // 稳定按下
        {
            Key_QueueEvent(k->id, KEY_EVT_PRESS, 0);
            k->hold_ms        = 0;
            k->long_triggered = 0;
            k->hold_period    = 0;
        }
        else                    // 稳定释放
        {
            if (k->last_state)  // 之前是按下
            {
                Key_QueueEvent(k->id, KEY_EVT_RELEASE, k->hold_ms);
            }
        }
        k->last_state = k->current_state;
    }
    else
    {
        // 状态稳定未变
        if (k->current_state)   // 持续按着
        {
            k->hold_ms += KEY_SCAN_PERIOD_MS;

            // 长按判定 (仅触发一次)
            if (!k->long_triggered && (k->hold_ms >= KEY_LONG_PRESS_MS))
            {
                Key_QueueEvent(k->id, KEY_EVT_LONG_PRESS, k->hold_ms);
                k->long_triggered = 1;
            }

            // 按住周期刷新 (每 KEY_HOLD_PERIOD_MS 置位一次事件)
            k->hold_period += KEY_SCAN_PERIOD_MS;
            if (k->hold_period >= KEY_HOLD_PERIOD_MS)
            {
                k->hold_period     = 0;
                Key_QueueEvent(k->id, KEY_EVT_HOLD, k->hold_ms);
            }
        }
    }
}

/************************ 扫描所有按键 (10ms 周期调用) ************************/
void Key_Scan(void)
{
    U8 i;
    for (i = 0; i < KEY_COUNT; i++)
    {
        ScanOneKey(&s_keys[i]);
    }
}

/************************ 获取并消费一个待处理事件 ************************/
// 按事件发生顺序消费队列中的一个事件
KeyEvent_t Key_GetEvent(void)
{
    KeyEvent_t evt;
    evt.id       = KEY1;
    evt.type     = KEY_EVT_NONE;
    evt.hold_ms  = 0;

    if (s_event_queue_count != 0)
    {
        evt = s_event_queue[s_event_queue_head];
        s_event_queue_head++;
        if (s_event_queue_head >= KEY_EVENT_QUEUE_SIZE)
        {
            s_event_queue_head = 0;
        }
        s_event_queue_count--;
    }
    return evt;
}

/************************ 查询指定按键是否按住 ************************/
bit Key_IsPressed(KeyId_t id)
{
    if (id >= KEY_COUNT) return 0;
    return s_keys[id].last_state ? 1 : 0;
}

/************************ 获取指定按键按住时长 (ms) ************************/
U16 Key_GetHoldTime(KeyId_t id)
{
    if (id >= KEY_COUNT) return 0;
    return s_keys[id].hold_ms;
}
