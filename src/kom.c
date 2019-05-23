/*=======================================================================================*
 * @file    kom.c
 * @author  Marcin Badzioch
 * @version 0.5
 * @date    02-07-2017
 * @brief   This file contains all implementations for USB communication module.
 *======================================================================================*/

/**
 * @addtogroup kom Description
 * @{
 * @brief Module for communication .
 */

/*======================================================================================*/
/*                       ####### PREPROCESSOR DIRECTIVES #######                        */
/*======================================================================================*/
/*-------------------------------- INCLUDE DIRECTIVES ----------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx_conf.h"
#include "kom.h"
#include "timer.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"
#include "debugkom.h"
#include "board.h"
/*----------------------------- LOCAL OBJECT-LIKE MACROS -------------------------------*/
#define KOM_CMD_TERMINATOR '\n'
#define COMMAND_BUF_LEN 64
/*---------------------------- LOCAL FUNCTION-LIKE MACROS ------------------------------*/

/*======================================================================================*/
/*                      ####### LOCAL TYPE DECLARATIONS #######                         */
/*======================================================================================*/
/*---------------------------- ALL TYPE DECLARATIONS -----------------------------------*/

/*-------------------------------- OTHER TYPEDEFS --------------------------------------*/

/*------------------------------------- ENUMS ------------------------------------------*/

/*------------------------------- STRUCT AND UNIONS ------------------------------------*/
/*======================================================================================*/
/*                    ####### LOCAL FUNCTIONS PROTOTYPES #######                        */
/*======================================================================================*/
static void Kom_CmdProcess();
static int my_strcmp (char* str1 , char* str2);
static void my_strcut (char* str_in , char delimiter , unsigned int delim_num , char* str_out , int max_len );
static void my_strcat (char* str_1 , char* str_2 , char* str_out , int max_len );
static void str_append (char* str_out , char* str_in , int max_len);
static void my_itoa (int i , char* str_out );
static uint32_t my_atoi (char* str_in );

/*======================================================================================*/
/*                         ####### OBJECT DEFINITIONS #######                           */
/*======================================================================================*/
/*--------------------------------- EXPORTED OBJECTS -----------------------------------*/
extern komUSBStatusType komUSBStatus;
extern uint8_t radio_command;
/*---------------------------------- LOCAL OBJECTS -------------------------------------*/
USB_CORE_HANDLE  USB_Device_dev ;
uint8_t recvBuf[KOM_RECV_BUF_LENGTH];
uint8_t recvBufEnd=0,recvCmdRdyFlag=0;

const char* commands [] = { "help","IDN?","REC?"};

const int command_count = 3;

char command_buf [COMMAND_BUF_LEN];

const char* IDN_string = "868_USB_Bridge\r\n";
const char* unknown_cmd_txt = "Unknown command!";
const char* wrong_param_txt = "Parameter out of range!";
uint8_t komTimer;
uint8_t sentflag=0;
/*======================================================================================*/
/*                  ####### EXPORTED FUNCTIONS DEFINITIONS #######                      */
/*======================================================================================*/

void Kom_Init(void)
{
	USBD_Init(&USB_Device_dev,
			&USR_desc,
			&USBD_CDC_cb,
			&USR_cb);

	Timer_Register(&komTimer,100,timerOpt_AUTOSTOP);
}
void Kom_Main(void)
{
	uint8_t i=0;
	if(recvCmdRdyFlag==1){
		recvCmdRdyFlag=0;
		for (i= 0 ; i < recvBufEnd ; i ++) command_buf [i] = recvBuf[i];
		recvBufEnd=0;
		Kom_CmdProcess();
		for (i = 0 ; i < COMMAND_BUF_LEN ; i ++) command_buf [i] = 0;
	}

//	if(komUSBStatus == KOM_USB_CONNECTED) BRD_Set(boardSet_LED1,1);
//	else BRD_Set(boardSet_LED1,0);
//
//	if(Timer_Check(&komTimer)==1){
//		BRD_Set(boardSet_LED2,0);
//	}
//
//	if(sentflag==1){
//		BRD_Set(boardSet_LED2,1);
//		Timer_Reset(&komTimer);
//		sentflag=0;
//	}
}

void Kom_Receive(uint8_t byte)
{
	if(recvBufEnd < KOM_RECV_BUF_LENGTH){
		recvBuf[recvBufEnd++]=byte;
		if(byte == KOM_CMD_TERMINATOR)recvCmdRdyFlag=1;
	}
	else{
		if(recvCmdRdyFlag==0)recvBufEnd=0;
	}
}

void Kom_Send(uint8_t* s,uint16_t datalen)
{
	if(komUSBStatus == KOM_USB_CONNECTED){
		VCP_DataTx(s,datalen);
		sentflag=1;
	}
}
/*======================================================================================*/
/*                   ####### LOCAL FUNCTIONS DEFINITIONS #######                        */
/*======================================================================================*/

static void Kom_SendResponse(char *s)
{
	uint8_t len=0;

	while(*(s+len)!=0)len++;

	VCP_DataTx(s,len);
	VCP_DataTx("\r\n",2);

}



static void Kom_CmdProcess()
{
    int i , command_num = -1;
    char t_s[COMMAND_BUF_LEN];
    uint32_t t_i;
	char cbufe[120];
	uint16_t leng;
    for (i = 0 ; i < command_count ; i++){
    	if ( my_strcmp ((char*)command_buf , (char*) commands [i]) ) {
    		command_num = i;
    		break;
    	}
	}

    switch (command_num) {
		case 0: { // command help
			for (i = 0 ; i < command_count ; i++) {
			Kom_SendResponse ((char*)commands[i]);
			}
		};break;

		case 1 : { // command IDN
			Kom_SendResponse ((char*)IDN_string);
		};break;

		case 2 : { // command REC?

			leng=sprintf(cbufe,"SEND: %d %d:%d\n\rF:%d %d C:%d R:%d Tz:%d Ti:%d Hi:%d To:%d Ho:%d %d %d\n\n\r",4,18,3,1023,-12,12,130,144,155,16,17,178,44,555);
			Kom_Send(cbufe,leng);
		};break;

    }
}
//
//	case 2: { // command SET?
//		my_itoa(driver.parSet,t_s);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 3: { // command SET
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 100){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.parSet=t_i;
//			driver.setCurrent=(double)driver.parSet*(double)driver.maxCurrent/100.0;
//			Kom_SendResponse(command_buf);
//		}
//
//	};break;
//
//	case 4: { // command CUR_SET?
//		my_itoa(driver.setCurrent,t_s);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 5: { // command MAX_CUR?
//		my_itoa(driver.maxCurrent,t_s);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 6: { // command MAX_CUR
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 1000){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.maxCurrent=t_i;
//			driver.setCurrent=(double)driver.parSet*(double)driver.maxCurrent/100.0;
//			Kom_SendResponse(command_buf);
//		}
//	};break;
//
//	case 7: { // command STATUS?
//		sprintf(t_s,"%d,%d,%d,%d,%d",meas.temperatureBrd,meas.currentLed,meas.voltageLed,meas.voltageInput,driver.fanPwmRead/10);
//		Kom_SendResponse(t_s);
//
//		/*sprintf(t_s,"\n\r%d %d %d",driver.fanMode,driver.fanState,driver.fanPwmSet);
//		Kom_SendResponse(t_s);*/
//	};break;
//
//	case 8: { // command FILTER?
//		sprintf(t_s,"%d",driver.filterState);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 9: { // command FILTER
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i == 0){
//			driver.filterState=t_i;
//			Driver_FilterSwitch(FILTER_OFF);
//			Kom_SendResponse(command_buf);
//		}
//		else if(t_i == 1){
//			driver.filterState=t_i;
//			Driver_FilterSwitch(FILTER_ON);
//			Kom_SendResponse(command_buf);
//		}
//		else{
//			Kom_SendResponse((char*)wrong_param_txt);
//
//		}
//	};break;
//
//	case 10: { // command FAN_MODE?
//		sprintf(t_s,"%d",driver.fanMode);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 11: { // command FAN_MODE
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 3){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.fanMode=t_i;
//			Kom_SendResponse(command_buf);
//		}
//	};break;
//
//	case 12: { // command FAN_STATE?
//		sprintf(t_s,"%d",driver.fanState);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 13: { // command FAN_TEMP?
//		sprintf(t_s,"%d",driver.fanTemp/10);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 14: { // command FAN_TEMP
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//
//		if((t_i > 100) || (t_i < 30)){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.fanTemp=t_i*10;
//			Kom_SendResponse(command_buf);
//		}
//	};break;
//
//	case 15: { // command CUTOFF_TEMP?
//		sprintf(t_s,"%d",driver.cutoffTemp/10);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 16: { // command CUTOFF_TEMP
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 120){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.cutoffTemp=t_i*10;
//			Kom_SendResponse(command_buf);
//		}
//	};break;
//
//	case 17: { // command CUTOFF_MODE?
//		sprintf(t_s,"%d",driver.cutoffMode);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 18: { // command CUTOFF_MODE
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 1){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			driver.cutoffMode=t_i;
//			Kom_SendResponse(command_buf);
//		}
//	};break;
//
//	case 19: { // command CUTOFF_STATE?
//		sprintf(t_s,"%d",driver.cutoffState);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 20: { // command READ_SETTINGS?
//		sprintf(t_s,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",driver.parSet,driver.setCurrent
//				,driver.maxCurrent,driver.filterState,driver.fanTemp,driver.fanMode
//				,driver.fanState,driver.cutoffTemp,driver.cutoffMode,driver.cutoffState);
//		Kom_SendResponse(t_s);
//	};break;
//
//	case 21: { // command PWM_SET
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		if(t_i > 1000){
//			Kom_SendResponse((char*)wrong_param_txt);
//		}
//		else{
//			Driver_FanPWMSet((uint16_t)t_i);
//			Kom_SendResponse(command_buf);
//		}
//
//	};break;
//
//	case 22: { // command PWM_PRESC
//		my_strcut ((char*)command_buf , ' ' , 1 , t_s , 64);
//		t_i = my_atoi (t_s);
//		//if(t_i > 1000){
//			Kom_SendResponse((char*)wrong_param_txt);
//		//}
//		//else{
//			//Driver_FanPrescSet((uint16_t)t_i);
//		//	Kom_SendResponse(command_buf);
//		//}
//
//	};break;
//
//	case 23:{	// RESET
//		Kom_SendResponse(command_buf);
//		NVIC_SystemReset();
//	}break;
//
//	case -1:Kom_SendResponse((char*)unknown_cmd_txt);break;
//    }
//
//}

/*
 *  String dedicated functions
 */

static int my_strcmp (char* str1 , char* str2)
{
    unsigned int t_i = 0;
    int cmp_ok = 1;
    while  ( (*(str1+t_i) != 0)  && (*(str2+t_i) != 0) ) {
        if (*(str1+t_i)  != *(str2+t_i))  cmp_ok = 0;
        t_i ++;
    }
    if (t_i == 0) cmp_ok = 0;
    if (*(str2+t_i) != 0 )  cmp_ok = 0;
    return (cmp_ok);
}

static void my_strcut (char* str_in , char delimiter , unsigned int delim_num , char* str_out , int max_len )
{
    unsigned int t_i = 0 , t_i2 = 0 , delim_count = 0;
    while ( *(str_in + t_i) != 0 )  {
        if ( ( delim_count == delim_num ) && ( *(str_in + t_i) != delimiter) ) {
            *(str_out + t_i2) = *(str_in + t_i);
            t_i2 ++;
            *(str_out + t_i2) = 0;
            if (t_i2 >= max_len - 1) break;
        }
        if (*(str_in + t_i)  == delimiter ) delim_count ++;
        if (delim_count > delim_num) return;
        t_i ++;
    }
}

static void my_strcat (char* str_1 , char* str_2 , char* str_out , int max_len )
{
    unsigned int t_i = 0 , t_i2 = 0;
    while (*(str_1 + t_i) != 0)  {
	    *(str_out + t_i2) =  *(str_1 + t_i);
	    t_i++;
	    t_i2++;
	    *(str_out + t_i2) = 0;
            if (t_i2 >= max_len - 1) return;
    }

    t_i = 0;
    while (*(str_2 + t_i) != 0)  {
	    *(str_out + t_i2) =  *(str_2 + t_i);
	    t_i++;
	    t_i2++;
	    *(str_out + t_i2) = 0;
            if (t_i2 >= max_len - 1) return;
    }

}

static void str_append (char* str_out , char* str_in , int max_len)
{
    unsigned int t_i = 0 ,t_i2 = 0;
    while (*(str_out + t_i) != 0) t_i ++;
    while (*(str_in + t_i2) != 0) {
	*(str_out + t_i) = *(str_in + t_i2);
	t_i ++;
	t_i2 ++;
        if (t_i >= max_len - 1) break;
    }
    *(str_out + t_i) = 0;
}

static void my_itoa (int i , char* str_out )
{
    unsigned int t_i = 0 , u_i , u_i2 , mask;
    if (i < 0) {
	*(str_out + t_i) = '-';
	t_i ++;
	u_i = - i;
    } else u_i = i;
    u_i2 = u_i;

    for (mask = 1000000000 ; mask > 0 ; mask /= 10) {
	if (u_i >= mask) {
	    *(str_out + t_i) = '0' + u_i2 / mask;
	    t_i++;
	}
	u_i2 = u_i2 % mask;
    }

    if (u_i == 0) {
	    *(str_out + t_i) = '0';
	    t_i++;
    }
    *(str_out + t_i) = 0;
}


static uint32_t my_atoi (char* str_in )
{
    uint32_t t_i = 0, t_i2 = 0 , sign = 0;
    if ( *str_in == '-' ) {t_i2 ++; sign = 1; }
    while ((*(str_in + t_i2) >= '0') && (*(str_in + t_i2) <= '9')) {
	t_i = t_i * 10 + *(str_in + t_i2) - '0';
	t_i2++;
    }

    if (sign) return (-t_i) ; else return (t_i);
}

/**
 * @} end of group kom
 */
