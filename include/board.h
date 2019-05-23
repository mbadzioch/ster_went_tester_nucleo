/*
 * board.h
 *
 *  Created on: 30 sty 2019
 *      Author: Marcin
 */

#ifndef BOARD_H_
#define BOARD_H_

typedef enum{
	boardSet_LED_RED = 0x01,
	boardSet_LED_YELLOW,
	boardSet_LED_GREEN

}boardSetComp_E;

typedef enum{
	boardGet_SW1 = 0x01,
	boardGet_SW2

}boardGetComp_E;

void BRD_Init();
void BRD_Set(boardSetComp_E boardSet, uint8_t val);
void BRD_Get(boardGetComp_E boardGet, uint8_t* val);


#endif /* BOARD_H_ */
