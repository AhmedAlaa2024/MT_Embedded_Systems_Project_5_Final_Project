/*====================================================================================================
 || Module: Timer																					||
 ||																									||
 || File Name: timer.c																				||
 ||																									||
 || Description: Source file for AVR Timer Module.													||
 ||																									||
 || Created on: Oct 29, 2021																		||
 ||																									||
 || Author: Ahmed Alaa																				||
 ====================================================================================================*/

/*========================================================================================================
 ||                         				Needed Header Files              		                     ||
 ========================================================================================================*/
#include "timer.h"
#include <avr/interrupt.h> /* For Timer1 ISR */
#include "gpio.h"
/*========================================================================================================
 ||                        				Static Global Variables              			 	             ||
 ========================================================================================================*/
/* Global variable for timer 0 to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr_timer0)(void) = NULL_PTR;

volatile uint16 g_ticks = 0;
/*========================================================================================================
 ||                            			Interrupt Service Routines               	 	    	         ||
 ========================================================================================================*/
ISR(TIMER0_OVF_vect)
{
	if (g_callBackPtr_timer0)
		(*g_callBackPtr_timer0)();
}
/*========================================================================================================
 ||                              		 		Functions                	     	    	             ||
 =========================================================================================================*/
/**********************************************************************************************************
 * Description : Initialize the timer by the passed configurations								  		  *
 * Arguments:																							  *
 * 		Config_Ptr <Timer_ConfigType*> The timer's configurations								  		  *
 **********************************************************************************************************/
void TIMER0_init(Timer0_ConfigType * Config_Ptr)
{
	/* Initialize the counter */
	TCNT0 = Config_Ptr->initial_value;

	/* Initialize the compare match value */
	OCR0 = Config_Ptr->compare_value;

	/* Reset the timer settings */
	TCCR0 = 0;

	if ((Config_Ptr->timer_mode == NORMAL_MODE) || (Config_Ptr->timer_mode == OVERFLOW_MODE))
	{
		/* Disable the PWM Mode */
		SET_BIT(TCCR0, FOC0);

		/* Setup the timer mode */
		CLEAR_BIT(TCCR0, WGM01);
		CLEAR_BIT(TCCR0, WGM00);

		/* Feed the timer clock by setup the prescaler */
		TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->timer_clock & 0x07);
	}
	else if (Config_Ptr->timer_mode == COMPARE_MODE)
	{
		/* Disable the PWM Mode */
		SET_BIT(TCCR0, FOC0);

		/* Setup the timer mode */
		SET_BIT(TCCR0, WGM01);
		CLEAR_BIT(TCCR0, WGM00);

		/* Setup the compare output mode */
		TCCR0 = (TCCR0 & 0xCF) | ((Config_Ptr->com_mode & 0x03) << 4);

		/* Feed the timer clock by setup the prescaler */
		TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->timer_clock & 0x07);
	}

	SET_BIT(SREG, SREG_I);
	SET_BIT(TIMSK, TOIE0);
}
/**********************************************************************************************************
 * Description : Function to set the Call Back function address.										  *
 * Arguments:																							  *
 * 		a_ptr <void*> The function address which will be called back									  *
 **********************************************************************************************************/
void TIMER0_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr_timer0 = (volatile void*)a_ptr;
}
/**********************************************************************************************************
 * Description : De-initialize the timer														  		  *
 * Arguments:																							  *
 * 		NONE								 		  													  *
 **********************************************************************************************************/
void TIMER0_deinit()
{
	TCNT0 = OCR0 = TCCR0 = 0;
}
