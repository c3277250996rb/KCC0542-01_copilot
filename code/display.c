#include "display.h"
#include "ht1621b.h"

#define DISPLAY_SEGMENT_A 0x001
#define DISPLAY_SEGMENT_B 0x002
#define DISPLAY_SEGMENT_C 0x004
#define DISPLAY_SEGMENT_D 0x008
#define DISPLAY_SEGMENT_E 0x010
#define DISPLAY_SEGMENT_F 0x020
#define DISPLAY_SEGMENT_G 0x040
#define DISPLAY_SEGMENT_J 0x080
#define DISPLAY_SEGMENT_L 0x100
#define DISPLAY_SEGMENT_N 0x200
#define DISPLAY_SEGMENT_H 0x400
#define DISPLAY_SEGMENT_K 0x800
#define DISPLAY_SEGMENT_M 0x1000
#define DISPLAY_SEGMENT_Y 0x2000

#define DISPLAY_RAM_ADDRESS_19 19
#define DISPLAY_RAM_ADDRESS_21 21
#define DISPLAY_RAM_ADDRESS_23 23
#define DISPLAY_RAM_ADDRESS_27 27
#define DISPLAY_RAM_ADDRESS_29 29
#define DISPLAY_RAM_ADDRESS_31 31
#define DISPLAY_RAM_ADDRESS_01 1
#define DISPLAY_RAM_ADDRESS_03 3
#define DISPLAY_RAM_ADDRESS_05 5
#define DISPLAY_RAM_ADDRESS_07 7
#define DISPLAY_RAM_ADDRESS_25 25
#define DISPLAY_RAM_ADDRESS_09 9
#define DISPLAY_RAM_ADDRESS_11 11
#define DISPLAY_RAM_ADDRESS_13 13
#define DISPLAY_RAM_ADDRESS_15 15
#define DISPLAY_RAM_ADDRESS_17 17

#define DISPLAY_DIGIT_1 1
#define DISPLAY_DIGIT_2 2
#define DISPLAY_DIGIT_3 3
#define DISPLAY_DIGIT_4 4
#define DISPLAY_DIGIT_5 5
#define DISPLAY_DIGIT_6 6
#define DISPLAY_DIGIT_7 7
#define DISPLAY_DIGIT_8 8
#define DISPLAY_DIGIT_9 9
#define DISPLAY_DIGIT_10 10

typedef struct
{
    u8 ram01;
    u8 ram03;
    u8 ram05;
    u8 ram07;
    u8 ram19;
    u8 ram21;
    u8 ram23;
    u8 ram27;
    u8 ram29;
    u8 ram31;
    u8 ram25;
    u8 ram09;
    u8 ram11;
    u8 ram13;
    u8 ram15;
    u8 ram17;
} DisplayRamBuffer;

static U16 display_number = 0;
static U8 display_independent_digits[6] = {0, 0, 0, 0, 0, 0};
static U32 display_graphics = 0;
static U8 display_independent_mask = 0x3f;
static bit display_primary_enabled = 1;

static void WriteDisplayRam(DisplayRamBuffer *ram);
static void BuildDisplayRam(DisplayRamBuffer *ram);

static U16 GetDigitSegmentMask(u8 digit)
{
    static code U16 segment_code[10] = {
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_E | DISPLAY_SEGMENT_F,
        DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C,
        // DISPLAY_SEGMENT_J | DISPLAY_SEGMENT_N,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_E | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_F | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_F | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_E | DISPLAY_SEGMENT_F | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_E | DISPLAY_SEGMENT_F | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L,
        DISPLAY_SEGMENT_A | DISPLAY_SEGMENT_B | DISPLAY_SEGMENT_C | DISPLAY_SEGMENT_D | DISPLAY_SEGMENT_F | DISPLAY_SEGMENT_G | DISPLAY_SEGMENT_L
    };

    return segment_code[digit % 10];
}

static void AddDigitSegments(u8 position, U16 segment_mask,
                             DisplayRamBuffer *ram)
{
    if (position == DISPLAY_DIGIT_1)/* 1 (AFGE_JLND, B--C_----)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram19 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram19 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram19 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram19 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram19 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram19 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram19 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram19 |= 0x01;

        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram21 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram21 |= 0x10;
    }
    else if (position == DISPLAY_DIGIT_2)/* 2 (-FE-_AGND, BJLC_----)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram21 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram21 |= 0x20;

        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram21 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram21 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram21 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram21 |= 0x01;
        
        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram23 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram23 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram23 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram23 |= 0x10;
    }
    else if (position == DISPLAY_DIGIT_3)/* 3 (----_AFGE, JLND_----, ----_B--C)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram23 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram23 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram23 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram23 |= 0x01;

        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram31 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram31 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram31 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram31 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram29 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram29 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_4)/* 4 (AGND_-FE-, ----_BJLC)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram29 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram29 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram29 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram29 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram29 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram29 |= 0x02;

        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram27 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram27 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram27 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram27 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_5)/* 5 (ABCD_-FGE)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram25 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram25 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram25 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram25 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram25 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram25 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram25 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_6)/* 6 (-FGE_ABCD)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram01 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram01 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram01 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram01 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram01 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram01 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram01 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_7)/* 7 (DCBA_-EGF)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram03 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram03 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram03 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram03 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram03 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram03 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram03 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_8)/* 8 (DCBA_-EGF)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram05 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram05 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram05 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram05 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram05 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram05 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram05 |= 0x01;
    }
    else if (position == DISPLAY_DIGIT_9)/* 9 (----_FGYE, AHJN_BKLM, --CD_----)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram13 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram13 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_Y) ram->ram13 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram13 |= 0x01;

        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram15 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_H) ram->ram15 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram15 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram15 |= 0x10;

        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram15 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_K) ram->ram15 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram15 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_M) ram->ram15 |= 0x01;

        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram17 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram17 |= 0x10;
    }
    else if (position == DISPLAY_DIGIT_10)/* 10 (----_FGYE, MLKA_NJH-, -BCD_----)*/
    {
        if (segment_mask & DISPLAY_SEGMENT_F) ram->ram09 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_G) ram->ram09 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_Y) ram->ram09 |= 0x02;
        if (segment_mask & DISPLAY_SEGMENT_E) ram->ram09 |= 0x01;

        if (segment_mask & DISPLAY_SEGMENT_M) ram->ram11 |= 0x80;
        if (segment_mask & DISPLAY_SEGMENT_H) ram->ram11 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_K) ram->ram11 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_N) ram->ram11 |= 0x10;
        
        if (segment_mask & DISPLAY_SEGMENT_A) ram->ram11 |= 0x08;
        if (segment_mask & DISPLAY_SEGMENT_J) ram->ram11 |= 0x04;
        if (segment_mask & DISPLAY_SEGMENT_L) ram->ram11 |= 0x02;

        if (segment_mask & DISPLAY_SEGMENT_B) ram->ram13 |= 0x40;
        if (segment_mask & DISPLAY_SEGMENT_C) ram->ram13 |= 0x20;
        if (segment_mask & DISPLAY_SEGMENT_D) ram->ram13 |= 0x10;
    }
}

/* ���һ��������ʾ֡�� RAM ��������������һ֡�Ķ�λ������ */
static void ClearDisplayRam(DisplayRamBuffer *ram)
{
    ram->ram01 = 0;
    ram->ram03 = 0;
    ram->ram05 = 0;
    ram->ram07 = 0;
    ram->ram19 = 0;
    ram->ram21 = 0;
    ram->ram23 = 0;
    ram->ram27 = 0;
    ram->ram29 = 0;
    ram->ram31 = 0;
    ram->ram25 = 0;
    ram->ram09 = 0;
    ram->ram11 = 0;
    ram->ram13 = 0;
    ram->ram15 = 0;
    ram->ram17 = 0;
}

/*
 * ��ָ�� HT1621 RAM ��ַ׷�Ӷ�λ��
 * bits ʹ�� 8 λ���룬bit7 ��Ӧ�õ�ַ�����λ��bit0 ��Ӧ���λ��
 * �ú���ʹ�� OR����˲��Ḳ��ͬһ��ַ���Ѿ���������ֶλ�ͼ�ζΡ�
 */
static void AddRamBits(DisplayRamBuffer *ram, u8 address, u8 bits)
{
    switch (address)
    {
        case DISPLAY_RAM_ADDRESS_01: ram->ram01 |= bits; break;
        case DISPLAY_RAM_ADDRESS_03: ram->ram03 |= bits; break;
        case DISPLAY_RAM_ADDRESS_05: ram->ram05 |= bits; break;
        case DISPLAY_RAM_ADDRESS_07: ram->ram07 |= bits; break;
        case DISPLAY_RAM_ADDRESS_09: ram->ram09 |= bits; break;
        case DISPLAY_RAM_ADDRESS_11: ram->ram11 |= bits; break;
        case DISPLAY_RAM_ADDRESS_13: ram->ram13 |= bits; break;
        case DISPLAY_RAM_ADDRESS_15: ram->ram15 |= bits; break;
        case DISPLAY_RAM_ADDRESS_17: ram->ram17 |= bits; break;
        case DISPLAY_RAM_ADDRESS_19: ram->ram19 |= bits; break;
        case DISPLAY_RAM_ADDRESS_21: ram->ram21 |= bits; break;
        case DISPLAY_RAM_ADDRESS_23: ram->ram23 |= bits; break;
        case DISPLAY_RAM_ADDRESS_25: ram->ram25 |= bits; break;
        case DISPLAY_RAM_ADDRESS_27: ram->ram27 |= bits; break;
        case DISPLAY_RAM_ADDRESS_29: ram->ram29 |= bits; break;
        case DISPLAY_RAM_ADDRESS_31: ram->ram31 |= bits; break;
        default: break;
    }
}

/* �� display_graphics �п����� T1~T23 ͼ�ζκϲ�����ʾ RAM�� */
static void AddGraphicSegments(DisplayRamBuffer *ram)
{
    if (display_graphics & DISPLAY_GRAPHIC_T11) AddRamBits(ram, DISPLAY_RAM_ADDRESS_27, 0x80);
    if (display_graphics & DISPLAY_GRAPHIC_T12) AddRamBits(ram, DISPLAY_RAM_ADDRESS_27, 0x40);
    if (display_graphics & DISPLAY_GRAPHIC_T13) AddRamBits(ram, DISPLAY_RAM_ADDRESS_27, 0x20);
    if (display_graphics & DISPLAY_GRAPHIC_T23) AddRamBits(ram, DISPLAY_RAM_ADDRESS_27, 0x10);

    if (display_graphics & DISPLAY_GRAPHIC_T10) AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x80);
    if (display_graphics & DISPLAY_GRAPHIC_T16) AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x40);
    if (display_graphics & DISPLAY_GRAPHIC_T22) AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x20);
    if (display_graphics & DISPLAY_GRAPHIC_T21) AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x10);
    if (display_graphics & DISPLAY_GRAPHIC_T1)  AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x08);
    if (display_graphics & DISPLAY_GRAPHIC_T2)  AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x04);
    if (display_graphics & DISPLAY_GRAPHIC_T3)  AddRamBits(ram, DISPLAY_RAM_ADDRESS_07, 0x02);

    if (display_graphics & DISPLAY_GRAPHIC_T7) AddRamBits(ram, DISPLAY_RAM_ADDRESS_09, 0x80);
    if (display_graphics & DISPLAY_GRAPHIC_T6) AddRamBits(ram, DISPLAY_RAM_ADDRESS_09, 0x40);
    if (display_graphics & DISPLAY_GRAPHIC_T5) AddRamBits(ram, DISPLAY_RAM_ADDRESS_09, 0x20);
    if (display_graphics & DISPLAY_GRAPHIC_T4) AddRamBits(ram, DISPLAY_RAM_ADDRESS_09, 0x10);
    if (display_graphics & DISPLAY_GRAPHIC_T8) AddRamBits(ram, DISPLAY_RAM_ADDRESS_11, 0x80);
    if (display_graphics & DISPLAY_GRAPHIC_T14) AddRamBits(ram, DISPLAY_RAM_ADDRESS_13, 0x80);

    if (display_graphics & DISPLAY_GRAPHIC_T9)  AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x80);
    if (display_graphics & DISPLAY_GRAPHIC_T15) AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x40);
    if (display_graphics & DISPLAY_GRAPHIC_T20) AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x08);
    if (display_graphics & DISPLAY_GRAPHIC_T19) AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x04);
    if (display_graphics & DISPLAY_GRAPHIC_T18) AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x02);
    if (display_graphics & DISPLAY_GRAPHIC_T17) AddRamBits(ram, DISPLAY_RAM_ADDRESS_17, 0x01);
}

/* ����ȫ��ͼ�ζ�״̬�������λΪ1�������Ϊ0��رա� */
void DisplayGraphicsSet(U32 graphic_mask)
{
    display_graphics = graphic_mask;
    DisplayGraphicsRefresh();
}

/* ��ָ��ͼ�ζΣ�δ�����������е�ͼ�α���ԭ״̬�� */
void DisplayGraphicsOn(U32 graphic_mask)
{
    display_graphics |= graphic_mask;
    DisplayGraphicsRefresh();
}

/* �ر�ָ��ͼ�ζΣ�δ�����������е�ͼ�α���ԭ״̬�� */
void DisplayGraphicsOff(U32 graphic_mask)
{
    display_graphics &= ~graphic_mask;
    DisplayGraphicsRefresh();
}

/* ����ǰ��������ֺ�ͼ��״̬�ؽ���ˢ��������ʾ�� */
void DisplayGraphicsRefresh(void)
{
    DisplayRamBuffer ram;

    BuildDisplayRam(&ram);
    WriteDisplayRam(&ram);
}

/* д�� 1~4 ���������ֹܶ�Ӧ�� RAM ��ַ�� */
static void WritePrimaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_19, ram->ram19);
    Write_1621(DISPLAY_RAM_ADDRESS_21, ram->ram21);
    Write_1621(DISPLAY_RAM_ADDRESS_23, ram->ram23);
    Write_1621(DISPLAY_RAM_ADDRESS_31, ram->ram31);
    Write_1621(DISPLAY_RAM_ADDRESS_29, ram->ram29);
    Write_1621(DISPLAY_RAM_ADDRESS_27, ram->ram27);
}

/* д�� 5~8 �Ŷ������ֹܼ������ڵ� RAM ��ַ�� */
static void WriteSecondaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_25, ram->ram25);
    Write_1621(DISPLAY_RAM_ADDRESS_01, ram->ram01);
    Write_1621(DISPLAY_RAM_ADDRESS_03, ram->ram03);
    Write_1621(DISPLAY_RAM_ADDRESS_05, ram->ram05);
    Write_1621(DISPLAY_RAM_ADDRESS_07, ram->ram07);
}

/* д�� 9��10 �Ŷ������ֹܼ�ͼ�ζ����ڵ� RAM ��ַ�� */
static void WriteTertiaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_09, ram->ram09);
    Write_1621(DISPLAY_RAM_ADDRESS_11, ram->ram11);
    Write_1621(DISPLAY_RAM_ADDRESS_13, ram->ram13);
    Write_1621(DISPLAY_RAM_ADDRESS_15, ram->ram15);
    Write_1621(DISPLAY_RAM_ADDRESS_17, ram->ram17);
}

/* ��Ӳ������˳�򣬽����� RAM ������д�� HT1621�� */
static void WriteDisplayRam(DisplayRamBuffer *ram)
{
    WritePrimaryDisplayRam(ram);
    WriteSecondaryDisplayRam(ram);
    WriteTertiaryDisplayRam(ram);
}

/*
 * ���ݵ�ǰ��ʾ״̬����һ֡ RAM ���ݣ�
 * 1~4 �Źܹ��� display_number����ʾ��ǧ���١�ʮ����λ��
 * 5~10 �Źֱܷ�ʹ�� display_independent_digits[0..5]��
 * �����Ӷ����� T1~T23 ͼ�ζΡ�
 */
static void BuildDisplayRam(DisplayRamBuffer *ram)
{
    u8 digit1 = (u8)(display_number / 1000);
    u8 digit2 = (u8)((display_number / 100) % 10);
    u8 digit3 = (u8)((display_number / 10) % 10);
    u8 digit4 = (u8)(display_number % 10);

    ClearDisplayRam(ram);
    if (display_primary_enabled)
    {
        AddDigitSegments(DISPLAY_DIGIT_1, GetDigitSegmentMask(digit1), ram);
        AddDigitSegments(DISPLAY_DIGIT_2, GetDigitSegmentMask(digit2), ram);
        AddDigitSegments(DISPLAY_DIGIT_3, GetDigitSegmentMask(digit3), ram);
        AddDigitSegments(DISPLAY_DIGIT_4, GetDigitSegmentMask(digit4), ram);
    }

    if (display_independent_mask & 0x01)
        AddDigitSegments(DISPLAY_DIGIT_5, GetDigitSegmentMask(display_independent_digits[0]), ram);
    if (display_independent_mask & 0x02)
        AddDigitSegments(DISPLAY_DIGIT_6, GetDigitSegmentMask(display_independent_digits[1]), ram);
    if (display_independent_mask & 0x04)
        AddDigitSegments(DISPLAY_DIGIT_7, GetDigitSegmentMask(display_independent_digits[2]), ram);
    if (display_independent_mask & 0x08)
        AddDigitSegments(DISPLAY_DIGIT_8, GetDigitSegmentMask(display_independent_digits[3]), ram);
    if (display_independent_mask & 0x10)
        AddDigitSegments(DISPLAY_DIGIT_9, GetDigitSegmentMask(display_independent_digits[4]), ram);
    if (display_independent_mask & 0x20)
        AddDigitSegments(DISPLAY_DIGIT_10, GetDigitSegmentMask(display_independent_digits[5]), ram);

    AddGraphicSegments(ram);
}

/* ���� 1~4 �Ź�������ʾ����λ����������ˢ�¡� */
void Display4Digits(U16 value)
{
    display_number = value % 10000;
    display_primary_enabled = 1;
    Display4DigitsRefresh();
}

/* ʹ�õ�ǰ�������λ���Ͷ�����״̬����ˢ����ʾ�� */
void Display4DigitsRefresh(void)
{
    DisplayRamBuffer ram;

    BuildDisplayRam(&ram);
    WriteDisplayRam(&ram);
}

/* ֱ�Ӳ��� 1 �Źܣ��ýӿ�ֻд������ʾ RAM ���� */
void Display1Digit10Seg(U8 digit)
{
    DisplayRamBuffer ram;

    ClearDisplayRam(&ram);
    AddDigitSegments(DISPLAY_DIGIT_1, GetDigitSegmentMask(digit), &ram);
    WritePrimaryDisplayRam(&ram);
}

/* �������� 6 �Źܣ�digit ����9ʱֻ������λ�� */
void Display6Digit10Seg(U8 digit)
{
    display_independent_digits[1] = digit % 10;
    DisplayGraphicsRefresh();
}

/* �������� 5 �Źܣ�digit ����9ʱֻ������λ�� */
void Display5Digit10Seg(U8 digit)
{
    display_independent_digits[0] = digit % 10;
    DisplayGraphicsRefresh();
}

/* �������� 7 �Źܣ�digit ����9ʱֻ������λ�� */
void Display7Digit10Seg(U8 digit)
{
    display_independent_digits[2] = digit % 10;
    DisplayGraphicsRefresh();
}

/* �������� 8 �Źܣ�digit ����9ʱֻ������λ�� */
void Display8Digit10Seg(U8 digit)
{
    display_independent_digits[3] = digit % 10;
    DisplayGraphicsRefresh();
}

/* �������� 9 �Źܣ�digit ����9ʱֻ������λ�� */
void Display9Digit10Seg(U8 digit)
{
    display_independent_digits[4] = digit % 10;
    DisplayGraphicsRefresh();
}

/* �������� 10 �Źܣ�digit ����9ʱֻ������λ�� */
void Display10Digit10Seg(U8 digit)
{
    display_independent_digits[5] = digit % 10;
    DisplayGraphicsRefresh();
}

void DisplayShowNormal(U16 rpm, U8 fan_level, U32 graphics)
{
    display_primary_enabled = 1;
    display_independent_mask = 0x3f;
    display_number = rpm % 10000;
    display_independent_digits[1] = fan_level % 10;
    display_graphics = graphics;
    DisplayGraphicsRefresh();
}

void DisplayShowTimer(U8 value, bit icon_on)
{
    display_primary_enabled = 0;
    display_independent_mask = 0x30;
    display_independent_digits[4] = value % 10;
    display_independent_digits[5] = value / 10;
    display_graphics = icon_on ? DISPLAY_GRAPHIC_T18 : 0;
    DisplayGraphicsRefresh();
}

void DisplayShowAlarmRpm(U16 value, bit icon_on)
{
    display_primary_enabled = 1;
    display_independent_mask = 0;
    display_number = value % 10000;
    display_graphics = icon_on ? DISPLAY_GRAPHIC_T3 : 0;
    DisplayGraphicsRefresh();
}
