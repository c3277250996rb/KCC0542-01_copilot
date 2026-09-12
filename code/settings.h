#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "common.h"
#include "key.h"

typedef enum {
    SETTINGS_IDLE = 0,
    SETTINGS_TIMER,
    SETTINGS_ALARM_RPM
} SettingsState_t;

#define SETTINGS_TIME_MIN       0
#define SETTINGS_TIME_MAX       99
#define SETTINGS_ALARM_RPM_MIN  0
#define SETTINGS_ALARM_RPM_MAX  9999
#define SETTINGS_ALARM_RPM_STEP 100

void Settings_Init(void);
void Settings_Reset(void);
bit Settings_Task(void);
void Settings_HandleKeyEvent(KeyEvent_t event);
SettingsState_t Settings_GetState(void);
bit Settings_IsActive(void);
U16 Settings_GetTime(void);
U8 Settings_GetRemainingSeconds(void);
U16 Settings_GetAlarmRpm(void);

#endif