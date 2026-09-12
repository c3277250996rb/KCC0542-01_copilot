#ifndef _GLOBALS_H_
#define _GLOBALS_H_


typedef unsigned char BOOL;
typedef unsigned char U8;
typedef unsigned int  U16;
typedef unsigned long U32;
typedef signed char   S8;
typedef signed int    S16;
typedef signed long   S32;
typedef void ( *FUNPTR )( void );

#define    NOP()   do{_nop_();}while(0)

//#define BIG_ENDIAN

#define SRAM  
#define XRAM          xdata

#define TRUE    1
#define FALSE   0
#define LOWBYTE(X)   ((U8)((X)&0xff))
#define HIGHBYTE(X)  ((U8)((X)/0x100))
#define INCEX(X)     if(++X==0)X--//(X=((X)+1>(X))?(X)+1:(X))
#define DECEX(X)     if(X!=0)X--//(X=((X)-1<(X))?(X)-1:(X))
#define BMSK(N)      (1U<<(N))
#define BSET(X,N)    (X)|=BMSK(N)
#define BCLR(X,N)    (X)&=(BMSK(N)^-1)
#define BCPL(X,N)    (X)^=BMSK(N)
#define BTST(X,N)    ((X)&BMSK(N))
#define BSETEX(X,N)  X[(N)>>3]|=((U8)BMSK((N)&7))
#define BCLREX(X,N)  X[(N)>>3]&=(((U8)BMSK((N)&7))^-1)
#define BCPLEX(X,N)  X[(N)>>3]^=((U8)BMSK((N)&7))
#define BTSTEX(X,N) (X[(N)>>3]&((U8)BMSK((N)&7)))
#define LENOF(ARR)  ((U8)(sizeof(ARR)/sizeof(ARR[0])))

#define C_SEGA  0X01
#define C_SEGB  0X02
#define C_SEGC  0X04
#define C_SEGD  0X08
#define C_SEGE  0X10
#define C_SEGF  0X20
#define C_SEGG  0X40
#define C_SEGP  0X80

#define C_CODE_0    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_1    (C_SEGB|C_SEGC)
#define C_CODE_2    (C_SEGA|C_SEGB|C_SEGD|C_SEGE|C_SEGG)
#define C_CODE_3    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGG)
#define C_CODE_4    (C_SEGB|C_SEGC|C_SEGF|C_SEGG)
#define C_CODE_5    (C_SEGA|C_SEGC|C_SEGD|C_SEGF|C_SEGG)
#define C_CODE_6    (C_SEGA|C_SEGC|C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_7    (C_SEGA|C_SEGB|C_SEGC)
#define C_CODE_8    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_9    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGF|C_SEGG)
#define C_CODE_A    (C_SEGA|C_SEGB|C_SEGC|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_a    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGG)
#define C_CODE_b    (C_SEGC|C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_C    (C_SEGA|C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_c    (C_SEGD|C_SEGE|C_SEGG)
#define C_CODE_d    (C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGG)
#define C_CODE_E    (C_SEGA|C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_e    (C_SEGA|C_SEGB|C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_F    (C_SEGA|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_G    (C_SEGA|C_SEGC|C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_g    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGF|C_SEGG)
#define C_CODE_H    (C_SEGB|C_SEGC|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_h    (C_SEGC|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_I    (C_SEGE|C_SEGF)
#define C_CODE_i    (C_SEGA|C_SEGC|C_SEGG)
#define C_CODE_J    (C_SEGB|C_SEGC|C_SEGD|C_SEGE)
#define C_CODE_L    (C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_N    (C_SEGA|C_SEGB|C_SEGC|C_SEGE|C_SEGF)
#define C_CODE_n    (C_SEGC|C_SEGE|C_SEGG)
#define C_CODE_O    (C_SEGA|C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_o    (C_SEGC|C_SEGD|C_SEGE|C_SEGG)
#define C_CODE_P    (C_SEGA|C_SEGB|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_q    (C_SEGA|C_SEGB|C_SEGC|C_SEGF|C_SEGG)
#define C_CODE_r    (C_SEGE|C_SEGG)
#define C_CODE_S    (C_SEGA|C_SEGC|C_SEGD|C_SEGF|C_SEGG)
#define C_CODE_T    (C_SEGA|C_SEGE|C_SEGF)
#define C_CODE_t    (C_SEGD|C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_U    (C_SEGB|C_SEGC|C_SEGD|C_SEGE|C_SEGF)
#define C_CODE_u    (C_SEGC|C_SEGD|C_SEGE)
#define C_CODE_y    (C_SEGB|C_SEGC|C_SEGD|C_SEGF|C_SEGG)
#define C_CODE_X    (C_SEGE|C_SEGF|C_SEGG)
#define C_CODE_HENG         (C_SEGG)


#define MILISEC  1

#define SECOND  1UL
#define MINUTE  60UL
#define HOUR  3600UL


#define  E_SYS_INIT         0
#define  E_SYS_OFF          1
#define  E_SYS_ON           2
#define  E_SYS_VERIFY       3




#endif

