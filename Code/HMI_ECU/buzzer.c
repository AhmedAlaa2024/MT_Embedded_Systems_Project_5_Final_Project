 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the buzzer driver
 *
 * Author: Ahmed Alaa
 *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Initialize the direction of the connected pin
 */
void BUZZER_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}

/*
 * Description :
 * Force HIGH Logic in Buzzer connected pin
 */
void BUZZER_on(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);
}

/*
 * Description :
 * Force LOW Logic in Buzzer connected pin
 */
void BUZZER_off(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
