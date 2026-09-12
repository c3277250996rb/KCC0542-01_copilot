#ifndef __KEY_H__
#define __KEY_H__

#include "common.h"

/************************ ����Ӳ������ ************************/
// ԭ��ͼ:
//   SW1 -> P04/CIN1   (key 1)
//   SW2 -> P30/C0P4   (key 2)
//   SW3 -> P21        (key 3)
//   SW4 -> P13        (key 4)
// �������½ӵ�, �ⲿ��������, �͵�ƽ��Ч

typedef enum {
    KEY1 = 0,   // SW1 - P04
    KEY2 = 1,   // SW2 - P30
    KEY3 = 2,   // SW3 - P21
    KEY4 = 3,   // SW4 - P13
    KEY_COUNT = 4
} KeyId_t;

/************************ �������� ************************/
#define KEY_SCAN_PERIOD_MS    10    // ɨ������ (ms)
#define KEY_DEBOUNCE_CNT      3     // �������� (3 * 10ms = 30ms)
#define KEY_LONG_PRESS_MS     500   // �����ж�ʱ�� (ms)
#define KEY_HOLD_PERIOD_MS    200   // ��ס����ˢ�¼�� (ms)

/************************ �����¼����� ************************/
typedef enum {
    KEY_EVT_NONE       = 0,  // ���¼�
    KEY_EVT_PRESS      = 1,  // ���� (�������״ΰ���˲��)
    KEY_EVT_RELEASE    = 2,  // �ͷ�
    KEY_EVT_LONG_PRESS = 3,  // ����
    KEY_EVT_HOLD       = 4,  // ��ס����ˢ�� (ÿ 200ms һ��)
} KeyEventType_t;

/************************ �����¼��ṹ�� ************************/
typedef struct {
    KeyId_t        id;      // ���� ID (KEY1 ~ KEY4)
    KeyEventType_t type;    // �¼�����
    U16            hold_ms; // �����¼�ʱ�İ���ʱ�� (ms)
} KeyEvent_t;

/************************ �������� ************************/
void        Key_InitTimer1(void);              // ��ʼ�� Timer1 (1ms �ж�����)
void        Key_Task(void);                    // ��ѭ������: 10ms ɨ���Ƶ
void        Key_Init(void);                    // ��ʼ������ 4 �������� GPIO
void        Key_Scan(void);                    // ɨ�����а��� (�ڲ�����)
KeyEvent_t  Key_GetEvent(void);                // ��ȡ������һ���������¼�
bit         Key_IsPressed(KeyId_t id);         // ��ѯָ��������ǰ�Ƿ�ס
U16         Key_GetHoldTime(KeyId_t id);       // ��ȡָ��������סʱ�� (ms)
bit         Key_IsScanTick(void);              // ��ѯ Key_Scan ��ִ�й� (10ms ���ı�־)

#endif
