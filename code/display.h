#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "common.h"

/* ��ʾһ����λʮ��������ǰ���㱣����ֻʹ��ĩ��λ�� */
void Display4Digits(U16 value);

/* ����д�뵱ǰ�������λ���� */
void Display4DigitsRefresh(void);

/* ����ͼ�ο���λ��Ĭ��ȫ���رգ����ͼ�ο��� | ��ϡ� */
#define DISPLAY_GRAPHIC_T1   ((U32)0x000001UL)
#define DISPLAY_GRAPHIC_T2   ((U32)0x000002UL)
#define DISPLAY_GRAPHIC_T3   ((U32)0x000004UL)
#define DISPLAY_GRAPHIC_T4   ((U32)0x000008UL)
#define DISPLAY_GRAPHIC_T5   ((U32)0x000010UL)
#define DISPLAY_GRAPHIC_T6   ((U32)0x000020UL)
#define DISPLAY_GRAPHIC_T7   ((U32)0x000040UL)
#define DISPLAY_GRAPHIC_T8   ((U32)0x000080UL)
#define DISPLAY_GRAPHIC_T9   ((U32)0x000100UL)
#define DISPLAY_GRAPHIC_T10  ((U32)0x000200UL)
#define DISPLAY_GRAPHIC_T11  ((U32)0x000400UL)
#define DISPLAY_GRAPHIC_T12  ((U32)0x000800UL)
#define DISPLAY_GRAPHIC_T13  ((U32)0x001000UL)
#define DISPLAY_GRAPHIC_T14  ((U32)0x002000UL)
#define DISPLAY_GRAPHIC_T15  ((U32)0x004000UL)
#define DISPLAY_GRAPHIC_T16  ((U32)0x008000UL)
#define DISPLAY_GRAPHIC_T17  ((U32)0x010000UL)
#define DISPLAY_GRAPHIC_T18  ((U32)0x020000UL)
#define DISPLAY_GRAPHIC_T19  ((U32)0x040000UL)
#define DISPLAY_GRAPHIC_T20  ((U32)0x080000UL)
#define DISPLAY_GRAPHIC_T21  ((U32)0x100000UL)
#define DISPLAY_GRAPHIC_T22  ((U32)0x200000UL)
#define DISPLAY_GRAPHIC_T23  ((U32)0x400000UL)

void DisplayGraphicsSet(U32 graphic_mask);
void DisplayGraphicsOn(U32 graphic_mask);
void DisplayGraphicsOff(U32 graphic_mask);
void DisplayGraphicsRefresh(void);
void DisplayShowNormal(U16 rpm, U8 fan_level, U32 graphics);
void DisplayShowTimer(U8 value, bit icon_on);
void DisplayShowAlarmRpm(U16 value, bit icon_on);

/* ֻ��ʾ�� 1 �� 10 �ιܣ���ֵ��Χ 0~9�� */
void Display1Digit10Seg(U8 digit);

/* �������µ� 5~10 �Źܣ���ֵ��Χ 0~9�� */
void Display5Digit10Seg(U8 digit);
void Display6Digit10Seg(U8 digit);
void Display7Digit10Seg(U8 digit);
void Display8Digit10Seg(U8 digit);
void Display9Digit10Seg(U8 digit);
void Display10Digit10Seg(U8 digit);

#endif
