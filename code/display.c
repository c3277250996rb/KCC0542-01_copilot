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

/* 清空一次完整显示帧的 RAM 缓冲区，避免上一帧的段位残留。 */
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
 * 向指定 HT1621 RAM 地址追加段位。
 * bits 使用 8 位掩码，bit7 对应该地址的最高位，bit0 对应最低位。
 * 该函数使用 OR，因此不会覆盖同一地址上已经加入的数字段或图形段。
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

/* 将 display_graphics 中开启的 T1~T23 图形段合并到显示 RAM。 */
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

/* 设置全部图形段状态：传入的位为1则点亮，为0则关闭。 */
void DisplayGraphicsSet(U32 graphic_mask)
{
    display_graphics = graphic_mask;
    DisplayGraphicsRefresh();
}

/* 打开指定图形段，未包含在掩码中的图形保持原状态。 */
void DisplayGraphicsOn(U32 graphic_mask)
{
    display_graphics |= graphic_mask;
    DisplayGraphicsRefresh();
}

/* 关闭指定图形段，未包含在掩码中的图形保持原状态。 */
void DisplayGraphicsOff(U32 graphic_mask)
{
    display_graphics &= ~graphic_mask;
    DisplayGraphicsRefresh();
}

/* 按当前保存的数字和图形状态重建并刷新整块显示。 */
void DisplayGraphicsRefresh(void)
{
    DisplayRamBuffer ram;

    BuildDisplayRam(&ram);
    WriteDisplayRam(&ram);
}

/* 写入 1~4 号联动数字管对应的 RAM 地址。 */
static void WritePrimaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_19, ram->ram19);
    Write_1621(DISPLAY_RAM_ADDRESS_21, ram->ram21);
    Write_1621(DISPLAY_RAM_ADDRESS_23, ram->ram23);
    Write_1621(DISPLAY_RAM_ADDRESS_31, ram->ram31);
    Write_1621(DISPLAY_RAM_ADDRESS_29, ram->ram29);
    Write_1621(DISPLAY_RAM_ADDRESS_27, ram->ram27);
}

/* 写入 5~8 号独立数字管及其所在的 RAM 地址。 */
static void WriteSecondaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_25, ram->ram25);
    Write_1621(DISPLAY_RAM_ADDRESS_01, ram->ram01);
    Write_1621(DISPLAY_RAM_ADDRESS_03, ram->ram03);
    Write_1621(DISPLAY_RAM_ADDRESS_05, ram->ram05);
    Write_1621(DISPLAY_RAM_ADDRESS_07, ram->ram07);
}

/* 写入 9、10 号独立数字管及图形段所在的 RAM 地址。 */
static void WriteTertiaryDisplayRam(DisplayRamBuffer *ram)
{
    Write_1621(DISPLAY_RAM_ADDRESS_09, ram->ram09);
    Write_1621(DISPLAY_RAM_ADDRESS_11, ram->ram11);
    Write_1621(DISPLAY_RAM_ADDRESS_13, ram->ram13);
    Write_1621(DISPLAY_RAM_ADDRESS_15, ram->ram15);
    Write_1621(DISPLAY_RAM_ADDRESS_17, ram->ram17);
}

/* 按硬件分组顺序，将完整 RAM 缓冲区写入 HT1621。 */
static void WriteDisplayRam(DisplayRamBuffer *ram)
{
    WritePrimaryDisplayRam(ram);
    WriteSecondaryDisplayRam(ram);
    WriteTertiaryDisplayRam(ram);
}

/*
 * 根据当前显示状态生成一帧 RAM 数据：
 * 1~4 号管共享 display_number，显示其千、百、十、个位；
 * 5~10 号管分别使用 display_independent_digits[0..5]；
 * 最后叠加独立的 T1~T23 图形段。
 */
static void BuildDisplayRam(DisplayRamBuffer *ram)
{
    u8 digit1 = (u8)(display_number / 1000);
    u8 digit2 = (u8)((display_number / 100) % 10);
    u8 digit3 = (u8)((display_number / 10) % 10);
    u8 digit4 = (u8)(display_number % 10);

    ClearDisplayRam(ram);
    AddDigitSegments(DISPLAY_DIGIT_1, GetDigitSegmentMask(digit1), ram);
    AddDigitSegments(DISPLAY_DIGIT_2, GetDigitSegmentMask(digit2), ram);
    AddDigitSegments(DISPLAY_DIGIT_3, GetDigitSegmentMask(digit3), ram);
    AddDigitSegments(DISPLAY_DIGIT_4, GetDigitSegmentMask(digit4), ram);

    AddDigitSegments(DISPLAY_DIGIT_5,
                     GetDigitSegmentMask(display_independent_digits[0]), ram);
    AddDigitSegments(DISPLAY_DIGIT_6,
                     GetDigitSegmentMask(display_independent_digits[1]), ram);
    AddDigitSegments(DISPLAY_DIGIT_7,
                     GetDigitSegmentMask(display_independent_digits[2]), ram);
    AddDigitSegments(DISPLAY_DIGIT_8,
                     GetDigitSegmentMask(display_independent_digits[3]), ram);
    AddDigitSegments(DISPLAY_DIGIT_9,
                     GetDigitSegmentMask(display_independent_digits[4]), ram);
    AddDigitSegments(DISPLAY_DIGIT_10,
                     GetDigitSegmentMask(display_independent_digits[5]), ram);

    AddGraphicSegments(ram);
}

/* 设置 1~4 号管联动显示的四位数，并立即刷新。 */
void Display4Digits(U16 value)
{
    display_number = value % 10000;
    Display4DigitsRefresh();
}

/* 使用当前保存的四位数和独立管状态重新刷新显示。 */
void Display4DigitsRefresh(void)
{
    DisplayRamBuffer ram;

    BuildDisplayRam(&ram);
    WriteDisplayRam(&ram);
}

/* 直接测试 1 号管；该接口只写入主显示 RAM 区域。 */
void Display1Digit10Seg(U8 digit)
{
    DisplayRamBuffer ram;

    ClearDisplayRam(&ram);
    AddDigitSegments(DISPLAY_DIGIT_1, GetDigitSegmentMask(digit), &ram);
    WritePrimaryDisplayRam(&ram);
}

/* 独立设置 6 号管，digit 超过9时只保留个位。 */
void Display6Digit10Seg(U8 digit)
{
    display_independent_digits[1] = digit % 10;
    DisplayGraphicsRefresh();
}

/* 独立设置 5 号管，digit 超过9时只保留个位。 */
void Display5Digit10Seg(U8 digit)
{
    display_independent_digits[0] = digit % 10;
    DisplayGraphicsRefresh();
}

/* 独立设置 7 号管，digit 超过9时只保留个位。 */
void Display7Digit10Seg(U8 digit)
{
    display_independent_digits[2] = digit % 10;
    DisplayGraphicsRefresh();
}

/* 独立设置 8 号管，digit 超过9时只保留个位。 */
void Display8Digit10Seg(U8 digit)
{
    display_independent_digits[3] = digit % 10;
    DisplayGraphicsRefresh();
}

/* 独立设置 9 号管，digit 超过9时只保留个位。 */
void Display9Digit10Seg(U8 digit)
{
    display_independent_digits[4] = digit % 10;
    DisplayGraphicsRefresh();
}

/* 独立设置 10 号管，digit 超过9时只保留个位。 */
void Display10Digit10Seg(U8 digit)
{
    display_independent_digits[5] = digit % 10;
    DisplayGraphicsRefresh();
}
