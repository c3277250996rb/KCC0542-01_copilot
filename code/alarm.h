#ifndef __ALARM_H__
#define __ALARM_H__

#include "common.h"

#define ALARM_OUTPUT_PERIOD_MS 100

void Alarm_Init(void);
void Alarm_Task(U16 rpm, U16 alarm_rpm, bit system_on);
void Alarm_Reset(void);
bit Alarm_IsActive(void);

#endif