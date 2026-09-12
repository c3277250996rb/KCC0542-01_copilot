#ifndef __APP_EVENT_H__
#define __APP_EVENT_H__

#include "common.h"

/* 应用层只接收这些语义事件，硬件扫描细节留在 key/plc 模块。 */
typedef enum {
    APP_EVT_NONE = 0,
    APP_EVT_KEY1_SHORT,
    APP_EVT_KEY1_LONG,
    APP_EVT_PLC_ON,
    APP_EVT_PLC_OFF,
    APP_EVT_KEY2_UP,
    APP_EVT_KEY2_DOWN,
    APP_EVT_KEY3_UP,
    APP_EVT_KEY3_DOWN,
    APP_EVT_KEY4_SHORT,
    APP_EVT_KEY4_LONG
} AppEventType_t;

typedef struct {
    AppEventType_t type;
    U16 hold_ms;
} AppEvent_t;

#endif