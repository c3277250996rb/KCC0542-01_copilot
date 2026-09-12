#include "common.h"



void  delay(unsigned int t)
{
    while(t--);
}

// 校验和
u8 check_sum(u8 *dat, u8 from, u8 to)
{
	u16 sum, i;
    u8 retu_sum=0;
	sum = 0;    
	for(i = from; i <= to; i++)
	{
		sum += dat[i];
	}
	retu_sum=sum &0x00ff;
	return retu_sum;
}


u8 isBitSet(u8 dat, u8 bbit)
{	
	if (bbit>7)  bbit = 0;
		
	if(dat&(1<<bbit))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void adjustNum(u8 *num, char adjust, u8 ss , u8 bb, u8 roll)
{
	int  tt = *num;
	tt = tt + adjust;
	
	if(tt < ss){
		tt = roll ? bb : ss;
	}
	else if(tt > bb){
		tt = roll ? ss : bb;
	}
	else;
	
	*num = (u8)tt;
}

// 将两个数组合并成一个
void my_strcat_arrcy(char *str, char *arr, char * brr)
{
    int i;
    for( i = 0; i < strlen(str); i++)
    {
        brr[i] = str[i]; 
    }
    for(i = 0; i < strlen(arr); i++)
    {
        brr[strlen(str) + i] = arr[i];
    }
}
