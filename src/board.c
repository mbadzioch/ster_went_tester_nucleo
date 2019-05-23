/*
 * board.c
 *
 *  Created on: 30 sty 2019
 *      Author: Marcin
 */

#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx_conf.h"
#include "board.h"

/*
 * 	=============  HSI  ==============
 *
 * 	LED: -> HIGH = LED On
 *
 * 	LED1 - PB9
 * 	LED2 - PC13
 *
 * 	TACT SW: -> LOW = Pressed
 *
 * 	SW1 - PA15
 * 	SW2 - PA10
*/
GPIO_InitTypeDef GPIO_InitStructure;


void BRD_Init()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}
void BRD_Set(boardSetComp_E boardSet, uint8_t val)
{
	if(boardSet == boardSet_LED_GREEN){
		if(val == 1)GPIO_SetBits(GPIOB,GPIO_Pin_6);
		else GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	}
	if(boardSet == boardSet_LED_RED){
		if(val == 1)GPIO_SetBits(GPIOB,GPIO_Pin_12);
		else GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	}
	if(boardSet == boardSet_LED_YELLOW){
		if(val == 1)GPIO_SetBits(GPIOB,GPIO_Pin_11);
		else GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	}
}
void BRD_Get(boardGetComp_E boardGet, uint8_t* val)
{
	if(boardGet == boardGet_SW1){
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10) == 0) *val = 1;
		else *val = 0;
	}

	if(boardGet == boardGet_SW2){
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) == 0) *val = 1;
		else *val = 0;
	}
}
