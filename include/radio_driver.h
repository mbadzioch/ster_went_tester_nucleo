/*=======================================================================================*
 * @file    radio_driver.h
 * @author  Damian Pala
 * @date    XX-XX-20XX
 * @brief   Header file for XXX module
 *
 *          This file contains API of XXX module
 *======================================================================================*/
/*----------------------- DEFINE TO PREVENT RECURSIVE INCLUSION ------------------------*/
#ifndef RADIO_DRIVER_H_
#define RADIO_DRIVER_H_

/**
 * @addtogroup XXX Description
 * @{
 * @brief Module for... .
 */

/*======================================================================================*/
/*                       ####### PREPROCESSOR DIRECTIVES #######                        */
/*======================================================================================*/
/*-------------------------------- INCLUDE DIRECTIVES ----------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/*----------------------------- LOCAL OBJECT-LIKE MACROS -------------------------------*/
#define RADIO_DEFAULT_PACKET_LENGTH             6

/*---------------------------- LOCAL FUNCTION-LIKE MACROS ------------------------------*/
#define GPIOx(PORT_NUMBER)                          ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(PORT_NUMBER)))
#define PIN_MASK(PIN)                               (1 << (PIN))
#define RCC_PORT_MASKx(PORT_NUMBER)                 (RCC_AHB1ENR_GPIOAEN << (PORT_NUMBER))

#define BUILD_UINT16(hByte, lByte)                  (((uint16_t)hByte << 8) + lByte)
#define BUILD_UINT24(hByte, mByte, lByte)           (((uint32_t)hByte << 16) + ((uint32_t)mByte << 8) + lByte)
#define GET_MSB(word)                               ((word >> 8) & 0xFF)
#define GET_LSB(word)                               (word & 0xFF)

#define NUM_OF_ELEMS(array)                         (sizeof(array)/sizeof((array)[0]))
#define UNUSED_FUNCTION(function)                   ((void)&(function))
/*======================================================================================*/
/*                     ####### EXPORTED TYPE DECLARATIONS #######                       */
/*======================================================================================*/
/*-------------------------------- OTHER TYPEDEFS --------------------------------------*/

/*------------------------------------- ENUMS ------------------------------------------*/
typedef enum RD_RadioMode_Tag
{
    RADIO_MODE_STANDBY = 1, // Low power mode - 450nA (800us to Tx or Rx)
    RADIO_MODE_SLEEP, // Sleep with wake up to check if signal is broadcasted
    RADIO_MODE_SENSOR, // Temperature / Low bat wake up
    RADIO_MODE_READY, // Low power mode - 800uA and 200us to Tx or Rx
    RADIO_MODE_TUNE, // 8.5mA, 200us to Tx/Rx
} RD_RadioMode_T;

typedef enum RD_RadioState_Tag
{
    RADIO_STATE_IDLE = 1,
    RADIO_STATE_RX,
    RADIO_STATE_TX,
} RD_RadioState_T;

/*------------------------------- STRUCT AND UNIONS ------------------------------------*/

/*======================================================================================*/
/*                    ####### EXPORTED OBJECT DECLARATIONS #######                      */
/*======================================================================================*/

/*======================================================================================*/
/*                   ####### EXPORTED FUNCTIONS PROTOTYPES #######                      */
/*======================================================================================*/

// Init:
void RD_Init(void);
void RD_ReconfigRadioIfNeeded(void);
bool RD_IsRadioDriverInitialized(void);

// State Management:
void RD_RadioMoveToMode(RD_RadioMode_T mode);
void RD_RadioMoveToState(RD_RadioState_T state);
void RD_RadioSleep(void);
void RD_RadioInterruptEnable(void);
void RD_RadioInterruptDisable(void);
void RD_ClearInterruptsStatus(void);

//High Level send / receive
void RD_RadioTransmitData(uint8_t *dataIn, uint16_t size);
void RD_RadioReceiveData(uint8_t *dataOut, uint16_t size);

// Low level module communication
void RD_RadioSendWriteBurst(uint8_t address, uint8_t *dataIn, uint16_t size);
void RD_RadioSendReadBurst(uint8_t address, uint8_t *dataOut, uint16_t size);

// Radio settings
void RD_SetTxHeader4B(uint8_t *byte);
void RD_SetRxCheckHeader4B(uint8_t *byte);
void RD_GetRxHeader4B(uint8_t *byte);

// FIFO Management
void RD_RadioClearTxFifo(void);
void RD_RadioClearRxFifo(void);


//Low level internal functions
void RD_RadioConfig(void); //Included in Init
void RD_RadioSendWriteTransaction(uint8_t address, uint8_t data);
uint8_t RD_RadioSendReadTransaction(uint8_t address);
void EXTI4_15_IRQHandler(void);

/*======================================================================================*/
/*                          ####### INLINE FUNCTIONS #######                            */
/*======================================================================================*/

/**
 * @}
 */

#endif /* RADIO_DRIVER_H_ */
