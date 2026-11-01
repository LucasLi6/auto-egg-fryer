#ifndef __LED_H
#define __LED_H
#include "sys.h"

								  
////////////////////////////////////////////////////////////////////////////////// 	


//LED端口定义
#define LED0 PEout(6)	// DS0
#define LED1 PIout(8)	// DS1	 

#define C1  PDout(14)  //    YA1   (开蛋器工作电机正)   没使用           
#define C2  PDout(0)  //      YA2   开蛋器工作电机反                         PD0
#define C3  PEout(7)  //      YA3  开蛋器翻转电机正                          PE7
#define C4  PEout(9)  //      YA4  开蛋器翻转电机反                          PE9
#define C5  PEout(11)  //    YA5  煎盘翻转电机正                             PE11
#define C6  PEout(13)  //    YA6  煎盘翻转电机反                             PE13
#define C7  PEout(15)  //   YA7  煎盘平转电机正                              PE15
#define C8  PCout(9)  //     Y1     煎盘平转电机反                           PC9
#define C9  PCout(8)  //     Y2                              l               PC8
#define C10  PGout(8)  //     Y3   电磁炉下加热                              PG8
#define C11  PGout(7)  //     Y4   电磁炉上加热                              PG7
#define C12  PGout(6)  //     Y5   喷油电机                                  PG6
#define C13  PBout(11)  //                                                  PB11
#define C14  PBout(1)  //      Y7  蜂鸣器                                    PB1

#define C15  PBout(3)  //电磁炉上电源开关
#define C16  PGout(15)  //电磁炉下电源开关

#define C18  PBout(15)  //装蛋器电机正
#define C19  PHout(13)  //装蛋器电机反
#define C20  PCout(10)  //
#define C21  PCout(11)  //

#define KEY1 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)	//按键1              PB5
#define KEY2 		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)	//按键2              PB8
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)	//按键3              PE5
#define KEY4 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//按键4              PE3
#define KEY5 		GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_2)	//按键5              PI2

#define A3144_1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)	//开蛋器工作位置1  PE2   
#define A3144_2 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)//开蛋器工作位置2  PD15
#define A3144_3 		GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_1)	//开蛋器翻转位置1  PI1
#define A3144_4 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8)	//开蛋器翻转位置2  PE8 
#define A3144_5 		GPIO_ReadInputDataBit(GPIOI,GPIO_Pin_0)	// 煎盘翻转位置1   PI0  
#define A3144_6 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12)	//煎盘翻转位置2  PE12
#define A3144_7 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)	//煎盘平转位置1  PE14
#define A3144_8 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)	//煎盘平转位置2    PD8
#define A3144_9 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)	//煎盘平转位置3  PD10  
#define A3144_10 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10)	//煎盘平转位置4  PG10 

#define A3144_11 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5)	//装蛋器位置1      PD5      
#define A3144_12 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)	//装蛋器位置2      PF0
#define A3144_13 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)	//
#define A3144_14 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)	//
#define A3144_15 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9)	//



void LED_Init(void);//初始化		 	

u8 KEY_Scan(u8 mode);
#endif
