/*====================================================================================================
 || Module: CONTROL_ECU Main																		||
 ||																									||
 || File Name: main.c																				||
 ||																									||
 || Description: Source file for the Application Module.											||
 ||																									||
 || Created on: Nov 5, 2021																			||
 ||																									||
 || Author: Ahmed Alaa																				||
 ====================================================================================================*/
#define F_CPU 8000000UL

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define HMI_ECU_IS_READY 			0xA5
#define CONTROL_ECU_IS_READY 		0x5A

#define ALERT 						0xBB

#define CORRECT 					0xF0
#define WRONG 						0x0F

#define EEPROM_SA 					0x0A
#define RecBefore 					0x11

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/
#include "timer.h"
#include "std_types.h"
#include "common_macros.h"
#include "eeprom.h"
#include "i2c.h"
#include "uart.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void TASK_doorOpen(void);
void TASK_doorClose(void);
void TASK_doorStop(void);
void TASK_alertOn(void);
void TASK_alertOff(void);
/* Function will be called back, It acts like seconds counter */
void TASK_TickTok(void);

/*******************************************************************************
 *                              Variables Declaration                          *
 *******************************************************************************/
/* Variable to receive key pressed on keypad */
volatile uint8 keyPressed;

/* Variable to decide whether there is a password recorded or not */
volatile uint8 IsRecorded;

/* Variable to hold the user's choice whether to change password or open door */
volatile uint8 User_Choice = 0;

/* String to store the 5-digits password received from user or EEPROM */
volatile uint8 CONTROL_ECU_Password[6];

/* Flag to make sure the 2 password attempts are matched */
volatile uint8 flag = 1;

/* Counters */
volatile uint8 i;

/*Variable to decide whether to open the door or turn on the buzzer*/
volatile uint8 Decision;

/* Counter to hold the seconds */
volatile uint16 g_seconds = 0;

int main (void)
{
	/*I2C initialization*/
	TWI_ConfigType TWI_Config = {FCPU_CLOCK,0x02,0x01};

	/*UART initialization*/
	UART_config uart_config = {
			9600,
			EIGHT_BITS,
			DISABLED,
			ONE_BIT
	};

	EEPROM_init(&TWI_Config);

	UART_init(&uart_config);

	while (1)
	{
		/* Check for every iteration if the password is stored in EEPROM or not */
		while(IsRecorded != RecBefore)
		{
			/* Wait until the HMI_ECU tell it is ready to receive */
			while (UART_recieveByte() != HMI_ECU_IS_READY);
			UART_sendByte(0);

			/* Tell HMI_ECU that I am ready to receive */
			UART_sendByte(CONTROL_ECU_IS_READY);
			UART_receiveString(CONTROL_ECU_Password);

			EEPROM_writeByte(EEPROM_SA,RecBefore);

			/* Store the 5 digits of the password */
			for (i=0; i<5 ; i++)
			{
				EEPROM_writeByte(EEPROM_SA+(i+1), CONTROL_ECU_Password[i]);
			}

			/* Set the flag to know there is a stored password in EEPROM */
			IsRecorded = RecBefore;
			EEPROM_writeByte(EEPROM_SA, IsRecorded);
		}

		/* If there is a stored password, and already will be, then read it */
		for (i=0; i<5 ;i++)
		{
			EEPROM_readByte(EEPROM_SA+(i+1), CONTROL_ECU_Password+i);
		}

		/* Wait until the HMI_ECU tell it is ready to receive */
		while (UART_recieveByte() != HMI_ECU_IS_READY);
		UART_sendByte(1);

		/* Tell HMI_ECU that I am ready to receive */
		UART_sendByte(CONTROL_ECU_IS_READY);
		User_Choice = UART_recieveByte();

		if (User_Choice == '-')
		{
			CONTROL_ECU_Password[5]='#';

			/* Wait until the HMI_ECU tell it is ready to receive */
			while (UART_recieveByte() != HMI_ECU_IS_READY);
			/* Sent the password to be checked */
			UART_sendString(CONTROL_ECU_Password);

			/* Tell HMI_ECU that I am ready to receive */
			UART_sendByte(CONTROL_ECU_IS_READY);
			/* To know if the user is knowing the password or he is a thief */
			Decision = UART_recieveByte();

			if (Decision == CORRECT)
			{
				UART_sendByte(CONTROL_ECU_IS_READY);
				UART_receiveString(CONTROL_ECU_Password);

				/* Store the new password */
				for (i=0; i<5 ; i++)
				{
					EEPROM_writeByte(EEPROM_SA+(i+1),CONTROL_ECU_Password[i]);
				}
			}
			else
			{
				/* Raise the alarm for 1 min */
				TASK_alertOn();
				TASK_alertOff();
			}
		}
		else if (User_Choice == '+')
		{
			CONTROL_ECU_Password[5]='#';

			while (UART_recieveByte() != HMI_ECU_IS_READY);
			UART_sendString(CONTROL_ECU_Password);

			UART_sendByte(CONTROL_ECU_IS_READY);
			Decision = UART_recieveByte();

			if (Decision == CORRECT)
			{
				/* Open the door in 30 sec and wait 3 sec, then close the door in 30 sec */
				SET_BIT(SREG,7);
				TASK_doorOpen();
				TASK_doorStop();
				TASK_doorClose();
			}
			else
			{
				/* Raise the alarm for 1 min */
				SET_BIT(SREG,7);
				TASK_alertOn();
				TASK_alertOff();
			}
		}
	}

}


void TASK_alertOn()
{
	BUZZER_init();
	BUZZER_on();
	g_seconds = g_ticks = 0;
	TIMER0_setCallBack(TASK_TickTok);

	Timer0_ConfigType timer0_config = {
				0,
				0,
				TIMER0_F_CPU_1024,
				TIMER_NORMAL_MODE,
				0
		};

	TIMER0_init(&timer0_config);

	/* 1 minute - Busy wait until 1830 overflows */
	while (g_seconds <= 60);
}

void TASK_alertOff(void)
{
	BUZZER_off();
	TIMER0_setCallBack(NULL_PTR);
	g_seconds = g_ticks = 0;
	TIMER0_deinit();
}

void TASK_doorOpen (void)
{
	DcMotor_Init();

	// Rotate the motor --> clock wise
	DcMotor_Rotate(0);

	TIMER0_setCallBack(TASK_TickTok);

	Timer0_ConfigType timer0_config = {
				0,
				0,
				TIMER0_F_CPU_1024,
				TIMER_NORMAL_MODE,
				0
		};

	TIMER0_init(&timer0_config);

	/* 15 seconds*/
	while (g_seconds <= 15);
}

void TASK_doorClose (void)
{
	// Rotate the motor --> Anti clock wise
	DcMotor_Rotate(1);

	g_seconds = g_ticks = 0;

	/* 15 seconds*/
	while (g_seconds <= 15);

	TIMER0_setCallBack(NULL_PTR);
	g_seconds = g_ticks = 0;
	TIMER0_deinit();
}

void TASK_doorStop (void)
{
	DcMotor_Rotate(2);

	g_seconds = g_ticks = 0;

	/* 3 seconds*/
	while (g_seconds <= 3);
}

void TASK_TickTok(void)
{
	g_ticks++;
	if (g_ticks == 31)
	{
		g_ticks = 0;
		g_seconds++;
	}
}
