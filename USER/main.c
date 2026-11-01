#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "stmflash.h" 
#include "key.h"  

#define FLASH_SAVE_ADDR  0X0800C004 	//设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
										//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.

////////////////////////////////////////////


void led_595_io_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟


	//GPIOB7初始化设置  SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	//GPIOE6  E4初始化设置  CLK   
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO

	//GPIOD7初始化设置   LOAD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO

	GPIO_ResetBits(GPIOB,  GPIO_Pin_7);						//拉低端口为低电平
	GPIO_ResetBits(GPIOE,  GPIO_Pin_4);						//拉低端口为低电平
	GPIO_ResetBits(GPIOD,  GPIO_Pin_7);						//拉低端口为低电平
	
}

#define SDA   PBout(7)
#define CLK  PEout(4)
#define LOAD   PDout(7)


/****************数码管段码表：共阳**********************/

//        A
//
//  F           B     
//
//				G
//
//  E            C
//
//         D     H

//                       0      1      2     3
const u8 Tab[]={0xc0,0xf9,0xa4,0xb0,    					//0  1   2   3
//               4       5      6      7	
                0x99,0x92,0x82,0xf8,    					        //4  5   6   7   
//               8       9      A      b	
                0x80,0x90,0x88,0x83,    					      // 8  9   10   11   
//               C      d      E      F     灭   .	
                0xc6,0xa1,0x86,0x8e,0xff,0x7f ,       //12  13  14  15  16  17
//               中--	左上  左下  下--
								0xbf ,0xdf,   0xef ,  0xf7,                   //18   19  20  21  
//	             右下  右上   上--
	              0xfb,    0xfd,    0xfe,                          //22    23   24
//                P      Y      J	
	               0x8c,0x91,0xf1,0xff,0xff,                   //25   26  27
	
//              0.      1.      2 .    3.	
								0x40,0x79,0x24,0x30,    					//  30+    0.  1.   2.   3.
//               4.       5.      6.      7.	  8.    9.  A. 
                0x19,0x12,0x02,0x78,0x00,0x10 ,0x08   					  // 30+      4  5   6   7   8.  9  A.  	
	
};


//写1位数字对应码表内容
void SPI_595(uint8_t value)
{
	uint8_t i;
	for(i=0;i<8;i++)  
	{
		CLK =0;		//CLK拉低
		if(0x80&value)												//判断当前发送位数据
			SDA =1;		//当前数据位为1，则拉高SDA
		else
			SDA =0;	//当前数据位为0，则拉低SDA
		CLK =1;			//拉高CLK，产生上升沿，保存移位数据
		value = value <<1 ;										//将数据左移一位
	}
	SDA =0;			//数据输入完毕后复位SDA/CLK
	CLK =0;				
}

void load(void)
{
	LOAD =1;				//拉高LOAD,产生上升沿，锁存移位数据并输出
	LOAD =0;			//完毕后复位LOAD
}

//显示4位数字
void SMG_Num4(int  dat)//显示4位数字
{
	u8 i,i1,i2,n1,n2,n3,n4;
	
	i=0;i1=0;i2=0;
	
	n1=dat/1000;
	if (n1==0) {i=1;}
	if(i==1)n1=16;
	
	n2=dat%1000/100;
	if (n2==0 && i==1) {i1=1;}
	if(i1==1)n2=16;

	n3=dat%100/10;
	if (n3==0 && i==1 && i1==1) {i2=1;}
	if(i2==1)n3=16;	
	
	n4=dat%10;
	
	SPI_595(Tab[ n4]);
	SPI_595(Tab[ n3]);	
	SPI_595(Tab[ n2]);	
	SPI_595(Tab[ n1]);	
	
	load();
}

//显示1位字符+3位数字
void SMG_Num1_3(u8 n, int  dat)//显示1位字符+3位数字
{
	u8 i1,i2,n2,n3,n4;
	
	i1=0;i2=0;
	
	n2=dat%1000/100;
	if (n2==0) {i1=1;}
	if(i1==1)n2=16;

	n3=dat%100/10;
	if (n3==0 && i1==1) {i2=1;}
	if(i2==1)n3=16;	
	
	n4=dat%10;
	
	SPI_595(Tab[ n4]);
	SPI_595(Tab[ n3]);	
	SPI_595(Tab[ n2]);	
	SPI_595(Tab[ n]);	
	
	load();
}
//显示各单独位
void SMG_Num1_1_1_1(u8 n1, u8  n2, u8  n3, u8  n4)//
{
	SPI_595(Tab[ n4]);
	SPI_595(Tab[ n3]);	
	SPI_595(Tab[ n2]);	
	SPI_595(Tab[ n1]);	
	load();
}

//显示A3144状态，0动作
void A3144_show(void)  //显示A3144状态，0动作
{
	u8  a1,a2,a3,a4,n1,n2,n3,n4;
	a1=0;a2=0;a3=0;a4=0;
	n1=0;n2=0;n3=0;n4=0;
	if(A3144_1 ==0)a1=1;
	if(A3144_2 ==0)a2=2;if(a1>0 && a2>0 )a2=30;
	n1=a1+a2;    
	if(n1==0)n1=16;
	a1=0;a2=0;a3=0;a4=0;
	if(A3144_3 ==0)a1=3;
	if(A3144_4 ==0)a2=4;if(a1>0 && a2>0)a2=30;
	n2=a1+a2;  
	if(n2==0)n2=16;
	a1=0;a2=0;a3=0;a4=0;
	if(A3144_5 ==0)a1=5;
	if(A3144_6 ==0)a2=6;if(a1>0 && a2>0)a2=30;
	n3=a1+a2;  
	if(n3==0)n3=16;
	a1=0;a2=0;a3=0;a4=0;
	if(A3144_7 ==0)a1=7;
	if(A3144_8 ==0)a2=8;
	if(A3144_9 ==0)a3=9;
	if(A3144_10 ==0)a4=10;
	n4=a1+a2+a3+a4; if(n4>10)n4=10+30;
	if(n4==0)n4=16;
	//n1=1;n2=2;n3=3;n4=4;
	SPI_595(Tab[ n4]);
	SPI_595(Tab[ n3]);
	SPI_595(Tab[ n2]);	
	SPI_595(Tab[ n1]);		
	load();
}

	
	

#define T_ERR   14000000      //电机运行超时系数	
#define T_SET  14000000    //设置时间系数

u8 K1_Flag =0;//按键1按下标志
	
//开蛋电机运行到位置1   开蛋
u8  m1_a1(void)
{
	u32  t;
	t=0;
	do{
		C2=1;
		if( KEY2 ==0 && K1_Flag ==1){C2=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
			 C2=0;
			//显示   C1A1
			SMG_Num1_1_1_1(12,1,10,1);
			return 1;
		}
	}while (A3144_1 ==1);
	 C2=0;
	return 0;//无错误

}
//开蛋电机运行到位置2  开蛋返回
u8  m1_a2(void)
{
	u32  t;
	 t=0;
	do{
		C2=1;
			if( KEY2 ==0 && K1_Flag ==1){C2=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C2=0;
			//显示   C1A2
			SMG_Num1_1_1_1(12,1,10,2);
			return 1;
		}	
	}while (A3144_2 ==1);
	 C2=0;
return 0;//无错误
}

//开蛋翻转电机运行到位置3    开蛋翻转
u8  m2_a3(void)
{
	u32  t;
	 t=0;
	do{
		C3=1;C4=0;
		if( KEY2 ==0 && K1_Flag ==1){C3=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
			 C3=0;
			//显示   C2A3
			SMG_Num1_1_1_1(12,2,10,3);
			return 1;
		}
	}while (A3144_3 ==1);
	 C3=0;
	return 0;//无错误
}
//开蛋翻转电机运行到位置4   开蛋翻转返回
u8  m2_a4(void)
{
	u32  t;
	 t=0;
	do{
		C4=1;C3=0;
		if( KEY2 ==0 && K1_Flag ==1){C4=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C4=0;
			//显示   C2A4
			SMG_Num1_1_1_1(12,2,10,4);
			return 1;
		}	
	}while (A3144_4 ==1);
	 C4=0;
return 0;//无错误
}


//煎盘翻转电机运行到位置5   煎盘翻转
u8  m3_a6(void)
{
	u32  t;
	 t=0;
	do{
		C6=1;C5=0;//C5=1;C6=0;
		if( KEY2 ==0 && K1_Flag ==1){C6=0;K1_Flag =2;return 1;}		
		t++;
		if(t>(T_ERR *1.4))//超时有错误
		{
			 C6=0;//C5=0;
			//显示   C3A6
			SMG_Num1_1_1_1(12,3,10,6);
			return 1;
		}
	}while (A3144_6 ==1);//(A3144_5 ==1);
	C6=0;// C5=0;
	return 0;//无错误
}
//煎盘翻转电机运行到位置6   煎盘翻转返回
u8  m3_a5(void)
{
		u32  t;
	 t=0;
	do{
		C5=1;C6=0;
		if( KEY2 ==0 && K1_Flag ==1){C5=0;K1_Flag =2;return 1;}		
		t++;
		if(t>(T_ERR *1.4))//超时有错误
		{
		 C5=0;
			//显示   C3A5
			SMG_Num1_1_1_1(12,6,10,6);
			return 1;
		}	
	}while (A3144_5 ==1);//(A3144_6 ==1);
	 C5=0;
return 0;//无错误
	

}


//煎盘平转电机运行到位置7   开蛋和装蛋位置
u8  m4_a7(void)
{
	u32  t;
	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A7
			SMG_Num1_1_1_1(12,4,10,7);
			return 1;
		}	
	}while (A3144_7 ==1 && A3144_5 ==0);
	 C7=0;
return 0;//无错误
}
u8  m4_a7_A(void)//调试霍尔元件A7
{
	u32  t;
	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A7
			SMG_Num1_1_1_1(12,4,10,7);
			return 1;
		}	
	}while (A3144_7 ==1);
	 C7=0;
return 0;//无错误
}

//煎盘平转电机运行到位置10   倒出煎好蛋位置
u8  m4_a10(void)
{
	u32  t;
	 t=0;
	do{
		C8=1;C7=0;
		if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4AA
			SMG_Num1_1_1_1(12,4,10,10);
			return 1;
		}	
	}while (A3144_10 ==1 && A3144_5 ==0);
	 C8=0;
return 0;//无错误
}
u8  m4_a10_A(void)//调试霍尔元件A10
{
	u32  t;
	 t=0;
	do{
		C8=1;C7=0;
		if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4AA
			SMG_Num1_1_1_1(12,4,10,10);
			return 1;
		}	
	}while (A3144_10 ==1);
	 C8=0;
return 0;//无错误
}
//煎盘平转电机运行到位置8    蛋加热位置
u8  m4_a8(void)
{
	u32  t;
	if(A3144_7 ==0)  //如果停在蛋加热位置左侧右行
	{
	 t=0;
	do{
		C8=1;C7=0;
				if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,8);
			return 1;
		}	
	}while (A3144_8 ==1 && A3144_5 ==0);
	 C8=0;
	}
	
	if( A3144_9 ==0 || A3144_10 ==0)//如果停在蛋加热位置右侧左行
	{	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,8);
			return 1;
		}	
	}while (A3144_8 ==1 && A3144_5 ==0);
	 C7=0;	
	}
return 0;//无错误
}
u8  m4_a8_A(void)//调试霍尔元件A8
{
	u32  t;
	if(A3144_7 ==0)  //如果停在蛋加热位置左侧右行
	{
	 t=0;
	do{
		C8=1;C7=0;
				if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,8);
			return 1;
		}	
	}while (A3144_8 ==1);
	 C8=0;
	}
	
	if( A3144_9 ==0 || A3144_10 ==0)//如果停在蛋加热位置右侧左行
	{	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,8);
			return 1;
		}	
	}while (A3144_8 ==1);
	 C7=0;	
	}
return 0;//无错误
}
//煎盘平转电机运行到位置9  喷油位置
u8  m4_a9(void)
{
	u32  t;
	if(A3144_7 ==0 || A3144_8 ==0)//如果停在喷油位置左侧右行
	{
	 t=0;
	do{
		C8=1;C7=0;
		if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,9);
			return 1;
		}	
	}while (A3144_9 ==1 && A3144_5 ==0);
	 C8=0;
	}
	
	if( A3144_10 ==0)//如果停在喷油位置右侧左行
	{	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A7
			SMG_Num1_1_1_1(12,4,10,9);
			return 1;
		}	
	}while (A3144_9 ==1 && A3144_5 ==0);
	 C7=0;	
	}
return 0;//无错误
}
u8  m4_a9_A(void)//调试霍尔元件A9
{
	u32  t;
	if(A3144_7 ==0 || A3144_8 ==0)//如果停在喷油位置左侧右行
	{
	 t=0;
	do{
		C8=1;C7=0;
		if( KEY2 ==0 && K1_Flag ==1){C8=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C8=0;
			//显示   C4A8
			SMG_Num1_1_1_1(12,4,10,9);
			return 1;
		}	
	}while (A3144_9 ==1);
	 C8=0;
	}
	
	if( A3144_10 ==0)//如果停在喷油位置右侧左行
	{	
	 t=0;
	do{
		C7=1;C8=0;
		if( KEY2 ==0 && K1_Flag ==1){C7=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C7=0;
			//显示   C4A7
			SMG_Num1_1_1_1(12,4,10,9);
			return 1;
		}	
	}while (A3144_9 ==1);
	 C7=0;	
	}
return 0;//无错误
}

//装蛋电机运行到位置11     装蛋
u8  m5_a11(void)
{
	u32  t;
	 t=0;
	do{
		C19=1;C20=0;
		if( KEY2 ==0 && K1_Flag ==1){C19=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
			 C19=0;
			//显示   C5A11
			SMG_Num1_1_1_1(12,5,10,11);
			return 1;
		}
	}while (A3144_11 ==1 && A3144_2 ==0);
	 C19=0;
	return 0;//无错误
}
u8  m5_a11_A(void)//调试霍尔元件A11
{
	u32  t;
	 t=0;
	do{
		C19=1;C20=0;
		if( KEY2 ==0 && K1_Flag ==1){C19=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
			 C19=0;
			//显示   C5A11
			SMG_Num1_1_1_1(12,5,10,11);
			return 1;
		}
	}while (A3144_11 ==1);
	 C19=0;
	return 0;//无错误
}

//装蛋电机返到位置12    装蛋返回
u8  m5_a12(void)
{
	u32  t;
	 t=0;
	do{
		C20=1;C19=0;
		if( KEY2 ==0 && K1_Flag ==1){C20=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C20=0;
			//显示   C5A12
			SMG_Num1_1_1_1(12,5,10,12);
			return 1;
		}	
	}while (A3144_12 ==1 && A3144_2 ==0);
	 C20=0;
return 0;//无错误
}
u8  m5_a12_A(void)//调试霍尔元件A12
{
	u32  t;
	 t=0;
	do{
		C20=1;C19=0;
		if( KEY2 ==0 && K1_Flag ==1){C20=0;K1_Flag =2;return 1;}		
		t++;
		if(t>T_ERR)//超时有错误
		{
		 C20=0;
			//显示   C5A12
			SMG_Num1_1_1_1(12,5,10,12);
			return 1;
		}	
	}while (A3144_12 ==1);
	 C20=0;
return 0;//无错误
}



	u8	buf[4];//flash存储的4 个字节

int main(void)
{ 
	u16 c ;
	u8 d;
	u8 i;
	u16  nn;
	u32 t;
	u16 n0,n1,n2;
	u8 key=0;
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	//uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	//KEY_Init();					//按键初始化 
	
	LED0=1;//板上指示灯亮
	LED1=1;
	delay_ms(50);  
	
	led_595_io_init();			//初始化数据传输引脚
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)buf,4);//读flash
		
		
	while(1)
	{
		nn=123;
		SMG_Num1_1_1_1(18,18,18,18);//显示- - - -
		while(1)
		{
			key=KEY_Scan(1);//读键值
			if(key ==1 && buf[0] > 0) // buf[0]=0是测试状态
			{
				K1_Flag =1;//按键1按下标志

				//复位			
				C10 =0;		C11 =0;//停止加热
				C12 =0;	//停止喷油
				if(A3144_5==1){m4_a10();	m3_a5();} //如煎盘不是水平状态先水平在A10处复位
				m1_a2();//复位
				m2_a4();
				m3_a5();
				m4_a10();delay_ms(200);
				m4_a9();
				m5_a12();

				if(A3144_5==1) goto aa;//如煎盘不是水平状态先水平在A10处复位
//喷油
				SMG_Num1_3(18, 1);
				C12 =1;
 //延时
				for (nn = 0; nn <buf[0]; nn++)
				{
					    if(KEY2 ==0) break;
							delay_ms(1000);
				}
				C12 =0;	//停止喷油
				
//煎盘平移到A8电磁炉位置    
				m4_a7();	delay_ms(200);
				m4_a8();	
				if(K1_Flag ==2)  goto aa;				
				
//预热				
				SMG_Num1_3(18, 2);
				
				C10 =1;	//电磁炉下加热
				C11 =1;//电磁炉上加热
				delay_ms(1000);
				C15 =1;	//电磁炉上电源开关
				C16 =1;//电磁炉下电源开关
				delay_ms(1000);
				C15 =0;	
				C16 =0;
			
//延时
				for (nn = 0; nn <buf[1]; nn++)
				{
					if(nn ==1){m5_a11();delay_ms(1000);m5_a12();}//放蛋
					    if(KEY2 ==0) break;
							delay_ms(1000);
				}				
				C10 =0;
				C11 =0;//电磁炉加热停
				
//煎盘平移到A7开蛋位置       
				SMG_Num1_3(18, 3);
				m4_a7();
				if(K1_Flag ==2)  goto aa;

//开蛋器开蛋
				SMG_Num1_3(18, 4);
				m1_a1();
				if(K1_Flag ==2)  goto aa;
//延时				
				delay_ms(3000);				
					
//煎盘平移到A8电磁炉位置    
				SMG_Num1_3(18, 5);
				m4_a8();	
				if(K1_Flag ==2)  goto aa;

				C10 =1;	//电磁炉下加热
				C11 =1;//电磁炉上加热
				delay_ms(1000);
				C15 =1;	//电磁炉上电源开关
				C16 =1;//电磁炉下电源开关
				delay_ms(1000);
				C15 =0;	
				C16 =0;
				
//开蛋器翻转
				SMG_Num1_3(18, 6);
				m2_a3();
				if(K1_Flag ==2)  goto aa;
//延时				
				delay_ms(3000);		
				
//开蛋器复位	
				SMG_Num1_3(18,7);
				m1_a2();					
				if(K1_Flag ==2)  goto aa;
				
	//开蛋器翻转 复位
				SMG_Num1_3(18, 8);
				m2_a4();				
				if(K1_Flag ==2)  goto aa;
								
//继续加热   延时
				SMG_Num1_3(18,9);
//C10  下电磁炉   间歇加热
//C11  上电磁炉
				 c =buf[2]*10 + buf[3];
				 d=0;
         t=0;
	
				for (nn = 0; nn < c; nn++)
				{  
						//下电磁炉10秒加热10秒停止循环
					d++;
					if(d>10)
					{
						if(t==0)
						{							
							t=1;
							C10 =1;
							delay_ms(1000);
							C16 =1;//电磁炉下电源开关
							delay_ms(1000);
							C16 =0;							
							
						}
						else
						{
							t=0;
							C10 =0;
						}
						d=0;
					}
	
					if(KEY2 ==0) break;
					delay_ms(1000);
				}			
				C10 =0;		C11 =0;//停止加热
				
//煎盘平移到A10位置   倒成品 
				SMG_Num1_3(18, 10);
				m4_a10();	
				if(K1_Flag ==2)  goto aa;
				
//煎盘翻转				
				SMG_Num1_3(18, 11);
			
				for (i = 0; i <7; i++)//产生振动
				{
					m3_a6();
					C5=1;delay_ms(200);C5=0;delay_ms(100);
				}				
				m3_a6();
				if(K1_Flag ==2)  goto aa;
				
//煎盘翻转	复位		
				SMG_Num1_3(18, 12);
				m3_a5();delay_ms(1000);
				if(K1_Flag ==2)  goto aa;
				
//煎盘平移到A9喷油初始位置   			
				SMG_Num1_3(18, 13);
				m4_a9();		
				if(K1_Flag ==2)  goto aa;
				delay_ms(2000);
				SMG_Num1_1_1_1(18,18,18,18);
				K1_Flag =0;//按键1按下标志

//蜂鸣器响
				for (i = 0; i <3; i++)
				{
					C14= 1;
					delay_ms(500);
					C14= 0;
					delay_ms(500);		
				}		
				goto aa1;
aa:		
//复位			
				C10 =0;		C11 =0;//停止加热
				C12 =0;	//停止喷油
				if(A3144_5==1){m4_a10();	m3_a5();} //如煎盘不是水平状态先水平在A10处复位
				m1_a2();//复位
			  m2_a4();
				m3_a5();
				m4_a10();delay_ms(200);
			  m4_a9();	
				m5_a12();
				SMG_Num1_1_1_1(19,19,19,19);
					
aa1:				

				key=0;
			}	
		
			if(key ==2)
			{
				if(K1_Flag ==2)//按键1没按下运行
				{
//复位			
					C10 =0;		C11 =0;//停止加热
					C12 =0;	//停止喷油
					if(A3144_5==1){m4_a10();	m3_a5();} //如煎盘不是水平状态先水平在A10处复位
					m1_a2();//复位
					m2_a4();
					m3_a5();
					m4_a10();delay_ms(200);
					m4_a9();	
					m5_a12();
					SMG_Num1_1_1_1(18,18,18,18);//显示—— ——
					K1_Flag =0;//按键1按下标志		
				}
		
				key=0;
			}	

			i=1;
			if(key ==3)//设置存储 喷油时间P  预热时间Y  加热时间J
			{

 //读flash
				STMFLASH_Read(FLASH_SAVE_ADDR,(u32*) buf, 4);
			
				n0=buf[0];//喷油时间P
				n1=buf[1];//预热时间Y
				n2=buf[2]*10+buf[3];//加热时间J
				if(i==1)
				{			
					SMG_Num1_3(25, n0);
					delay_ms(500);
				}
				if(i==2)
				{
					SMG_Num1_3(26, n1);
					delay_ms(500);
				}
				if(i==3)
				{
					SMG_Num1_3(27, n2);
					delay_ms(500);
				}	
			
				t=0;
				do{

					t++;
					if( KEY3 ==0)//
					{
						t=0;
						i++;
						if(i==1)
						{
						SMG_Num1_3(25, n0);	//显示喷油设置时间
						delay_ms(500);
							}
							if(i==2)
						{
						SMG_Num1_3(26, n1);//显示预热设置时间
						delay_ms(500);
							}
					if(i==3)
						{
						SMG_Num1_3(27, n2);//显示加热设置时间
						delay_ms(500);
							}
						if(i>3)i=0;
					}			
							// +1
					if( KEY4 ==0)//
					{
						t=0;
						if(i==1)
						{
							n0++;
					SMG_Num1_3(25, n0);	//显示喷油设置时间
						delay_ms(500);				
						}
						
							if(i==2)
						{
							n1++;
					SMG_Num1_3(26, n1);//显示预热设置时间
						delay_ms(500);				
						}			
						
							if(i==3)
						{
							n2++;
					SMG_Num1_3(27, n2);//显示加热设置时间
						delay_ms(500);				
						}			
							
					}			
							
								// -1			
					if( KEY5 ==0)//
					{
									t=0;
								if(i==1)
						{
							n0--;
					SMG_Num1_3(25, n0);//显示喷油设置时间
						delay_ms(500);				
						}
						
							if(i==2)
						{
							n1--;
					SMG_Num1_3(26, n1);//显示预热设置时间
						delay_ms(500);				
						}			
						
							if(i==3)
						{
							n2--;
					SMG_Num1_3(27, n2);//显示加热设置时间
						delay_ms(500);				
						}			
							
					}			
				
			}while (t < T_SET );
	
//存flash
			buf[0]=n0;
			buf[1]=n1;
			buf[2]=n2/10;
			buf[3]=n2%10;
			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*) buf,4);		
			SMG_Num1_1_1_1(18,18,18,18);
			
			key=0;
		}		
		
			if(key ==4)
		{
			if(buf[0] >0)  // 0 不测试
			{
				//复位			
				C10 =0;		C11 =0;//停止加热
				C12 =0;	//停止喷油
				if(A3144_5==1){m4_a10();	m3_a5();} //如煎盘不是水平状态先水平在A10处复位
				m1_a2();//复位
				m2_a4();
				m3_a5();
				m4_a10();delay_ms(200);
				m4_a9();	
				m5_a12();
				A3144_show();  //显示A3144状态，0动作		
				delay_ms(2000);			
				SMG_Num1_1_1_1(18,18,18,18);	
			}
	
			if(buf[0] ==0 && buf[1] ==1) // 1 开蛋电机测试
			{
				m1_a1();
			}				
			if(buf[0] ==0 && buf[1] ==2) // 2 开蛋翻转电机测试
			{
				m2_a3();
			}				
			if(buf[0] ==0 && buf[1] ==3) // 3 煎盘翻转电机测试
			{
				m3_a5();
			}				
			if(buf[0] ==0 && buf[1] ==4) // 4 煎盘平转电机测试 A7 - A8
			{
				m4_a7_A();
			}				
			if(buf[0] ==0 && buf[1] ==5) // 5 煎盘平转电机测试 A9 - A10
			{
				m4_a9_A();
			}				
			if(buf[0] ==0 && buf[1] ==6) // 6 装蛋电机测试	
			{
				m5_a11_A();
			}				

			key=0;
		}		
		
			if(key ==5)
		{
			if(buf[0] >0)  // 0 不测试
			{	
				A3144_show();  //显示A3144状态，0动作		
				delay_ms(2000);
				SMG_Num1_1_1_1(18,18,18,18);
			}
			
			if(buf[0] ==0 && buf[1] ==1) // 1 开蛋电机测试
			{
				m1_a2();
			}				
			if(buf[0] ==0 && buf[1] ==2) // 2 开蛋翻转电机测试
			{
				m2_a4();

			}				
			if(buf[0] ==0 && buf[1] ==3) // 3 煎盘翻转电机测试
			{
				m3_a6();

			}				
			if(buf[0] ==0 && buf[1] ==4) // 4 煎盘平转电机测试 A7 - A8
			{
				m4_a8_A();

			}				
			if(buf[0] ==0 && buf[1] ==5) // 5 煎盘平转电机测试 A9 - A10
			{
				m4_a10_A();
			}				
			if(buf[0] ==0 && buf[1] ==6) // 6 装蛋电机测试	
			{
				m5_a12_A();
			}			
			
			key=0;
		}		
		
		
	}    
}
}
