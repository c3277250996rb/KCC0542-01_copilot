#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "common.h"

/************************ ���������Ŷ��� ************************/
// ԭ��ͼ:
//   BUZ ������ +5V, ������ Q2(S8050) ���缫
//   Q2 ������ R8(1K) �� BUZZER �ź�, ���伫�ӵ�
//   ��Դ������: P23 ����ߵ�ƽ -> Q2 ��ͨ -> ���������� (�ߵ�ƽ��Ч)
//   ֻ�� GPIO ���ؿ���, ���� PWM/����
#define BUZZER_OUT     P23_OUT     // P23 �������
#define BUZZER_PIN     P23

/************************ �������� ************************/
void Buzzer_Init(void);      // ��ʼ�� (P23 �������, Ĭ�Ϲر�)
void Buzzer_On(void);        // ����
void Buzzer_Off(void);       // �ر�
void Buzzer_Set(u8 val);     // 1=����, 0=�ر�
void Buzzer_Toggle(void);    // ��ת״̬
void Buzzer_Beep(u16 ms);    // ����ָ��ʱ�� (����)
void Buzzer_Task(void);      // ����������ʱ������

#endif
