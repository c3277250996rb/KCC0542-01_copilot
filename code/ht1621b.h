#ifndef __SEGMENTLCD__H__
#define __SEGMENTLCD__H__
#include "common.h"


//定义HT1621的命令 
#define  ComMode    0x52  //4COM,1/3bias  1000    010 1001  0  
#define  RCosc      0x30  //内部RC振荡器(上电默认)1000 0011 0000 
#define  LCD_on     0x06  //打开LCD 偏压发生器1000     0000 0 11 0 
#define  LCD_off    0x04  //关闭LCD显示 
#define  Sys_en     0x02  //系统振荡器开 1000   0000 0010 
#define  CTRl_cmd   0x80  //写控制命令 
#define  Data_cmd   0xa0  //写数据命令 

//设置变量寄存器函数
#define sbi(x, y)  (x |= (1 << y))   /*置位寄器x的第y位*/
#define cbi(x, y)  (x &= ~(1 <<y ))  /*清零寄器x的第y位*/  

////IO端口定义
//sbit LCD_DATA = P2^0;
//sbit LCD_WR = P2^1;
//sbit LCD_CS = P2^2;
 
#define     LCD_DATA_OUT            P32_OUT     
#define     LCD_WR_OUT              P36_OUT  
#define     LCD_CS_OUT              P05_OUT

#define   LCD_DATA                  P32
#define   LCD_WR                    P36
#define   LCD_CS                    P05 
 
#define BACK_LIGHT_OUT      P03_OUT
#define BACK_LIGHT_PIN      P03


//定义端口HT1621数据端口 
#define LCD_DATA1    LCD_DATA = 1 
#define LCD_DATA0    LCD_DATA = 0
#define LCD_WR1      LCD_WR = 1  
#define LCD_WR0      LCD_WR = 0  
#define LCD_CS1      LCD_CS = 1  
#define LCD_CS0      LCD_CS = 0

//背光控制
extern void BACK_LIGHT_SET(u8 val);   // 1=开, 0=关

//函数声明
extern void Delay20us();       //20 us延时函数
extern void SendBit_1621(unsigned char sdat,unsigned char cnt); //data 的高cnt 位写入HT1621，高位在前
extern void Delay_nms(unsigned int n);       //N ms延时函数
extern void SendCmd_1621(unsigned char command);			//发送指令
extern void Write_1621(unsigned char addr,unsigned char sdat);
extern void HT1621_all_off(unsigned char num);
extern void HT1621_all_on(unsigned char num);
extern void HT1621_all_on_num(unsigned char num,unsigned char xx);
extern void LCDoff(void);
extern void LCDon(void);
extern void Displaybihua(void);
extern void Init_1621(void);
extern void Displayall8(void);
extern void Displaydata(void);//屏显示，


extern char dispnum[10];
extern const char num[]; 
extern const char num1[];
extern const char num2[];
extern const char num3[];
#endif