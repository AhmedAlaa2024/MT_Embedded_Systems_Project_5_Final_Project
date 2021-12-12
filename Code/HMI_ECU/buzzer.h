 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Ahmed Alaa
 *
 *******************************************************************************/

#ifndef HAL_BUZZER_H_
#define HAL_BUZZER_H_

#include "std_types.h"

#define BUZZER_PORT			PORTD_ID
#define BUZZER_PIN			PIN5_ID

/*******************************************************************************
 *                       Functions Prototypes                                  *
 *******************************************************************************/
/*
 * Description :
 * Initialize the connected pin
 */
void BUZZER_init(void);

/*
 * Description :
 * Force HIGH Logic in Buzzer connected pin
 */
void BUZZER_on(void);

/*
 * Description :
 * Force LOW Logic in Buzzer connected pin
 */
void BUZZER_off(void);

#endif /* HAL_BUZZER_H_ */
