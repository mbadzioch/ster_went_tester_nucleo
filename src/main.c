#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx_conf.h"
#include "delay.h"
#include "timer.h"
#include "debugkom.h"
#include "radio_driver.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"
#include "board.h"
#include "kom.h"

#define ADC1_DR_Address    0x40012440

#define FILTER_LEN 32

#define TH_HIGH 1990
#define TH_LOW 1800
#define TH_ON 100


uint16_t RegularConvData_Tab[FILTER_LEN+1];

void ADC1_Configure(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	/* ADC1 DeInit */
	ADC_DeInit(ADC1);

	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);


	ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles);

//	/* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */
//	ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);
//	ADC_TempSensorCmd(ENABLE);
//
//	/* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */
//	ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles);
//	ADC_VrefintCmd(ENABLE);
//
//	/* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */
//	ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);
//	ADC_VbatCmd(ENABLE);

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

	/* ADC1 regular Software Start Conv */
	ADC_StartOfConversion(ADC1);
}

void DMA1_Configure(void)
{
	DMA_InitTypeDef   DMA_InitStructure;
	/* DMA1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = FILTER_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* DMA1 Channel1 enable */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}



uint32_t meas_buf;
char bufek[8];
int main()
{
	DelayInit();
	Timer_Init();
	Debug_Init();
	BRD_Init();
	//Kom_Init();
	ADC1_Configure();
	DMA1_Configure();

	while (1)
	{
		//Kom_Main();
		Debug_Main();

		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET );
		/* Clear DMA TC flag */
		DMA_ClearFlag(DMA1_FLAG_TC1);
		meas_buf=0;
		for (uint8_t i=0; i<FILTER_LEN; i++){
			meas_buf+=RegularConvData_Tab[i];
		}
		meas_buf = meas_buf / FILTER_LEN;
//		sprintf(bufek,"%d\n\r\n\r",RegularConvData_Tab[0]);
//		PC_Debug(bufek);
		if(meas_buf > TH_HIGH){
			BRD_Set(boardSet_LED_GREEN,0);
			BRD_Set(boardSet_LED_YELLOW,0);
			BRD_Set(boardSet_LED_RED,1);
		}
		else if(meas_buf > TH_LOW){
			BRD_Set(boardSet_LED_GREEN,1);
			BRD_Set(boardSet_LED_YELLOW,0);
			BRD_Set(boardSet_LED_RED,0);
		}
		else if(meas_buf > TH_ON){
			BRD_Set(boardSet_LED_GREEN,0);
			BRD_Set(boardSet_LED_YELLOW,0);
			BRD_Set(boardSet_LED_RED,1);
		}
		else{
			BRD_Set(boardSet_LED_GREEN,0);
			BRD_Set(boardSet_LED_YELLOW,1);
			BRD_Set(boardSet_LED_RED,0);
		}

	}


}

