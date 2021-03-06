/*====================================================================================================
 || Module: Timer																					||
 ||																									||
 || File Name: timer.h																				||
 ||																									||
 || Description: Header file for AVR Timer Module.													||
 ||																									||
 || Created on: Oct 29, 2021																		||
 ||																									||
 || Author: Ahmed Alaa																				||
 ====================================================================================================*/
#ifndef MCAL_INC_8BIT_TIMER_H_
#define MCAL_INC_8BIT_TIMER_H_

/*========================================================================================================
 ||                         				Needed Header Files              		                     ||
 ========================================================================================================*/
#include "common_macros.h"
#include "std_types.h"
/*========================================================================================================
 ||                         		Definitions and Static Configurations              			         ||
 ========================================================================================================*/
#define NUMBER_OF_OVERFLOWS_PER_HALF_SECOND 2
/*========================================================================================================
 ||                              		 Types Declaration                    	    		        	 ||
 ========================================================================================================*/
typedef enum
{
	TIMER0_NO_CLOCK, TIMER0_F_CPU_CLOCK, TIMER0_F_CPU_8, TIMER0_F_CPU_64, TIMER0_F_CPU_256, TIMER0_F_CPU_1024
} Timer_Clock;

typedef enum {
	TIMER_NORMAL_MODE = 0, OVERFLOW_MODE = 0, COMPARE_MODE = 1, FAST_PWM_MODE = 2
} Timer_Mode;

typedef enum {
	TIMER_POLLING, TIMER_INTERRUPT
} Process_Mode;

typedef enum {
	NORMAL_MODE = 0, DISCONNECT_MODE = 0, TOGGLE = 1, RESERVED = 1,
	CLEAR = 2, NON_INVERTING = 2, SET = 3, INVERTING = 3
} COM_Mode;

/**********************************************************************************************************
 * Description: Structure of all dynamic configurations for the timer									  *
 **********************************************************************************************************/
typedef struct {
	uint32 initial_value;
	uint32 compare_value;
	Timer_Clock timer_clock;
	Timer_Mode timer_mode;
	COM_Mode com_mode;
} Timer0_ConfigType;
/**********************************************************************************************************
 * Description: Structure of time info.																	  *
 * Expected Size: 5 Byte																				  *
 **********************************************************************************************************/
typedef struct {
	uint16 days;
	uint8 hours;
	uint8 minutes;
	uint8 seconds;
} TimeType;

extern volatile uint16 g_ticks;
/*=========================================================================================================
 ||                              		 Functions Prototypes                    	    	        	  ||
 =========================================================================================================*/
/**********************************************************************************************************
 * Description : Initialize the timer by the passed configurations								  		  *
 * Arguments:																							  *
 * 		Config_Ptr <Timer_ConfigType*> The timer's configurations								  		  *
 **********************************************************************************************************/
void TIMER0_init(Timer0_ConfigType * Config_Ptr);
/**********************************************************************************************************
 * Description : Function to set the Call Back function address.										  *
 * Arguments:																							  *
 * 		a_ptr <void*> The function address which will be called back									  *
 **********************************************************************************************************/
void TIMER0_setCallBack(void(*a_ptr)(void));
/**********************************************************************************************************
 * Description : De-initialize the timer														  		  *
 * Arguments:																							  *
 * 		NONE								 		  													  *
 **********************************************************************************************************/
void TIMER0_deinit();

#endif /* MCAL_INC_8BIT_TIMER_H_ */
