#include "ht1621b.h"

//**********************************************************
//背光控制
//**********************************************************
void BACK_LIGHT_SET(u8 val)
{
    if(val != 0)
    {
        BACK_LIGHT_PIN = 1;
    }
    else
    {
        BACK_LIGHT_PIN = 0;
    }
}

char dispnum[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	  
//地址<=8真值表顺序DCBADFGE
/*0,1,2,3,4,5,6,7,8,9,*/
const char num[]={0xbf,0x06,0xdb,0x5f,0x76,0x6d,0xed,0x07,0xef,0x6f}; 
//地址>8<=16//
const char num1[]={0xfa,0x0a,0xd6,0x9e,0x2e,0xbd,0xfc,0x1a,0xfe,0xbe};
//地址>16
const char num2[]={0xaf,0xa0,0xcb,0xe9,0xe4,0x6d,0x6f,0xa8,0xef,0xed};
//
const char num3[]={
    
    0x10,0x20,0x01,             //负离子
    0x40,0x80,0x01,0x04,0x02,   //风扇
    0x31,0xce ,                  //负离子/风扇
    0xff
};
//**********************************************************
//延时函数
//**********************************************************
void Delay20us()		
{
	unsigned char i;
	_nop_();
	i = 60;
	while (--i);
}
   
void Delay_nms(unsigned int n)       //N ms延时函数
{
	unsigned int i=0,j=0;
	for (i=0;i<n;i++)
		for(j = 0 ; j < 792 ; j++);
}

//**********************************************************
//发送数据
//**********************************************************
void SendBit_1621(unsigned char sdat,unsigned char cnt) //data 的高cnt 位写入HT1621，高位在前 
{ 
	unsigned char i; 
	for(i=0;i<cnt;i++) 
	{ 
		LCD_WR0;
		Delay20us(); 
		if(sdat&0x80)
		{
			LCD_DATA1; 
		}
		else 
		{
			LCD_DATA0; 
		}
		Delay20us();
		LCD_WR1;
		Delay20us();
		sdat<<=1; 
	} 
	Delay20us(); 
}


//**********************************************************
//发送指令
//**********************************************************
void SendCmd_1621(unsigned char command)			//发送指令 
{ 
    u8 i=0;
	LCD_CS0; 
    for(i=0;i<5;i++);
	SendBit_1621(0x80,4);    //写入标志码“100”和9 位command 命令，由于 
	SendBit_1621(command,8); //没有使有到更改时钟输出等命令，为了编程方便 
	for(i=0;i<5;i++);
    LCD_CS1;                     //直接将command 的最高位写“0” 
} 

//**********************************************************
//发送指令和数据
//**********************************************************
void Write_1621(unsigned char addr,unsigned char sdat) 
{ 
    u8 i=0;
	addr<<=2; 
	LCD_CS0; 
    for(i=0;i<5;i++);
	SendBit_1621(0xa0,3);       //写入标志码“101” 
	SendBit_1621(addr,6);       //写入addr 的高6位 
	SendBit_1621(sdat,8);       //写入data 的8位 
    for(i=0;i<5;i++);
	LCD_CS1; 
}

//**********************************************************
//清除显示
//**********************************************************
void HT1621_all_off(unsigned char num) 
{ 
	unsigned char i; 
	unsigned char addr=0; 
	for(i=0;i<num;i++) 
	{ 
		Write_1621(addr,0x00); 
		addr+=2; 
	} 
}

//**********************************************************
//全部点亮
//**********************************************************
void HT1621_all_on(unsigned char num) 
{ 
	unsigned char i; 
	unsigned char addr=0; 
	for(i=0;i<num;i++) 
	{ 
		Write_1621(addr,0xff); 
		addr+=2; 
	} 
}

//**********************************************************
//全部点亮,显示相同数字
//**********************************************************
void HT1621_all_on_num(unsigned char num,unsigned char xx) 
{ 
	unsigned char i; 
	unsigned char addr=0; 
	for(i=0;i<num;i++) 
	{ 
		Write_1621(addr,xx); 
		addr+=2; 
	} 
} 

//**********************************************************
//初始化1621
//**********************************************************
void Init_1621(void) 
{
	LCDoff();
	SendCmd_1621(Sys_en);
	SendCmd_1621(RCosc);    
	SendCmd_1621(ComMode);  
	LCDon();
}

//**********************************************************
//液晶关闭
//**********************************************************
void LCDoff(void) 
{  
	SendCmd_1621(LCD_off);  
} 

//**********************************************************
//液晶打开
//**********************************************************
void LCDon(void) 
{  
	SendCmd_1621(LCD_on);  
}
//**********************************************************
//显示数组数据
//**********************************************************
void Displaydata(void)//屏显示
{

}       



