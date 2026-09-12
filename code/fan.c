#include "fan.h"
#include "epwm.h"
#include "timer.h"
#include "gpio.h"
#include "common.h"

/************************ PWM 初始周期 ************************/
#if FAN_CONTROL_MODE == FAN_CONTROL_MODE_FREQUENCY
#define FAN_INITIAL_PERIOD_TICK FAN_TEST_PERIOD_TICK
#else
#define FAN_INITIAL_PERIOD_TICK FAN_PWM_PERIOD_TICK
#endif

/************************ 全局变量 ************************/
volatile U16 fan_rpm = 0;             // M 法计算的 RPM 结果

/************************ 内部变量 ************************/
static volatile U16 s_fg_pulse_cnt;  // FG 脉冲累计 (Timer2 捕获中断里 +1)
static volatile U16 s_sample_div;    // M 法时间窗口分频 (利用 Timer1 1ms)
static volatile U16 s_window_cnt;    // 当前窗口内的 FG 脉冲数

/************************ EPWM 初始化 (P26 输出) ************************/
static void Fan_PwmInit(void)
{
    // 1. 引脚复用: P26 = PG5 (EPWM5 输出)
    P26CFG = GPIO_MUX_PG5;              // 0x17
    P2TRIS |= (1 << 6);       // P26 设为输出方向

    // 2. EPWM 运行模式: 向下计数 + 非对称 + 独立
    EPWM_ConfigRunMode(EPWM_COUNT_DOWN
                     | EPWM_OCU_ASYMMETRIC
                     | EPWM_WFG_INDEPENDENT
                     | EPWM_OC_INDEPENDENT);

    // 3. 通道时钟 = 1MHz (Fsys=16M, PWM45PSC=1 -> 8M, DIV=16 -> 1M)
    EPWM_ConfigChannelClk(FAN_PWM_CH, EPWM_CLK_DIV_16);

    // 4. 周期 = 40 tick -> 25kHz
    EPWM_ConfigChannelPeriod(FAN_PWM_CH, FAN_INITIAL_PERIOD_TICK);

    // 5. 初始占空比 = 0 (开机默认关闭)
    EPWM_ConfigChannelSymDuty(FAN_PWM_CH, 0);

    // 6. 自动加载
    EPWM_EnableAutoLoadMode(FAN_PWM_CHANNEL_MSK);

    // 7. 启动
    EPWM_ENABLE_LOAD(FAN_PWM_CHANNEL_MSK);
    EPWM_Start(FAN_PWM_CHANNEL_MSK);
    EPWM_EnableOutput(FAN_PWM_CHANNEL_MSK);
}

/************************ FG 捕获初始化 (P22 输入) ************************/
static void Fan_FgInit(void)
{
    // 1. P22 引脚配置: 数字 GPIO 上拉输入
    //    捕获输入由 PS_CAP0 选择; P22CFG 不能配成 CC0, 0x04 是 CC0 比较输出复用
    FAN_FG_PIN_CFG  = GPIO_MUX_GPIO;
    FAN_FG_PORT_TRIS &= ~(1 << FAN_FG_PIN_BIT); // 输入方向
    FAN_FG_PORT_UP  |= (1 << FAN_FG_PIN_BIT);   // 内部上拉

    // 2. 引脚路由: P22 -> CC0 捕获输入
    GPIO_SET_PS_MODE(PS_CAP0, GPIO_P22);       // 0xF0C8 = 0x22

    // 3. Timer2 配置: 定时模式 + 禁止重载, 自由运行
    //    CC0 捕获锁存使用 RLDH/RLDL, 不能同时开 AUTO_LOAD
    TMR2_ConfigRunMode(TMR2_MODE_TIMING, TMR2_LOAD_DISBALE);
    TMR2_ConfigTimerClk(TMR2_CLK_DIV_12);      // 16MHz/12 = 1.333MHz
    TMR2_ConfigTimerPeriod(0x0000);            // 从 0 开始自由计数

    // 4. 清中断标志, 开 CC0 下降沿捕获 (FG 标准是下降沿计数)
    T2IF = 0x00;
    TMR2_EnableCapture(FAN_FG_CAP_CH, TMR2_CAP_EDGE_FALLING);

    // 5. 开 CC0 捕获中断 + Timer2 总中断
    TMR2_EnableCaptureInt(FAN_FG_CAP_CH);
    TMR2_AllIntEnable();

    // 6. 启动 Timer2
    TMR2_Start();

    // 7. 清状态变量
    s_fg_pulse_cnt = 0;
    s_sample_div   = 0;
    s_window_cnt   = 0;
    fan_rpm        = 0;
}

/************************ 初始化 ************************/
void Fan_Init(void)
{
    Fan_PwmInit();
    Fan_FgInit();
}

void Fan_ResetRuntime(void)
{
    s_fg_pulse_cnt = 0;
    s_sample_div = 0;
    s_window_cnt = 0;
    fan_rpm = 0;
    Fan_SetSpeed(0);
}

/************************ 设置占空比 ************************/
void Fan_SetDuty(U8 percent)
{
    U16 duty_tick;

    if (percent > 100) percent = 100;
    duty_tick = (U16)(((U32)percent * FAN_PWM_PERIOD_TICK) / 100UL);
    EPWM_ConfigChannelSymDuty(FAN_PWM_CH, duty_tick);
}

/************************ 设置 PWM 频率，固定 50% 占空比 ************************/
void Fan_SetFrequency(U16 frequency_hz)
{
    U16 period_tick;

    if (frequency_hz == 0)
    {
        EPWM_ConfigChannelSymDuty(FAN_PWM_CH, 0);
        return;
    }

    if (frequency_hz < 16) frequency_hz = 16;
    period_tick = (U16)(FAN_PWM_CLOCK_HZ / frequency_hz);
    if (period_tick == 0) period_tick = 1;

    EPWM_ConfigChannelPeriod(FAN_PWM_CH, period_tick);
    EPWM_ConfigChannelSymDuty(FAN_PWM_CH, period_tick / 2);
}

/************************ 根据开关选择调速方式 ************************/
void Fan_SetSpeed(U16 value)
{
#if FAN_CONTROL_MODE == FAN_CONTROL_MODE_FREQUENCY
    Fan_SetFrequency(value);
#else
    Fan_SetDuty((U8)value);
#endif
}

/************************ Timer2 捕获中断: 累加 FG 脉冲 ************************/
// 在 isr.c 的 Timer2_IRQHandler() 中调用
void Fan_FgCaptureIsr(void)
{
    if (TMR2_GetCaptureIntFlag(FAN_FG_CAP_CH))
    {
        TMR2_ClearCaptureIntFlag(FAN_FG_CAP_CH);
        s_fg_pulse_cnt++;
    }
}

/************************ Timer1 1ms 中断: M 法时间窗口 ************************/
// 在 isr.c 的 Timer1_IRQHandler() 中调用
// 公式: RPM = pulse_cnt * 60000 / (pulses_per_rev * sample_ms)
//                = pulse_cnt * 60     (2 pulses/rev, 500ms 窗口)
void Fan_RpmSampleIsr(void)
{
    if (++s_sample_div >= FAN_RPM_SAMPLE_MS)
    {
        s_sample_div = 0;
        s_window_cnt = s_fg_pulse_cnt;     // 读出窗口脉冲数
        s_fg_pulse_cnt = 0;                // 清零开始下一轮
        fan_rpm = (U16)((U32)s_window_cnt * FAN_RPM_SCALE);
    }
}
