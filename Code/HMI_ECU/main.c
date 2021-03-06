/*====================================================================================================
 || Module: HMI_ECU Main																			||
 ||																									||
 || File Name: main.c																				||
 ||																									||
 || Description: Source file for the Application Module.											||
 ||																									||
 || Created on: Nov 5, 2021																			||
 ||																									||
 || Author: Ahmed Alaa																				||
 ====================================================================================================*/

/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/
#define F_CPU 8000000UL
#include "lcd.h"
#include "keypad.h"
#include "std_types.h"
#include "common_macros.h"
#include "timer.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define HMI_ECU_IS_READY 			0xA5
#define CONTROL_ECU_IS_READY 		0x5A

#define ALERT 						0xBB

#define CORRECT 					0xF0
#define WRONG 						0x0F

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void TASK_optionsDisplay(void);
void TASK_passwordChange(void);
void TASK_newPasswordEnter(void);
void TASK_passwordCheck(void);
void TASK_doorOpen(void);
void TASK_doorStop(void);
void TASK_doorClose(void);
void TASK_alert(void);
void TASK_TickTok(void);
/*******************************************************************************
 *                              Global Variables	                           *
 *******************************************************************************/
/* Variable to receive key pressed on keypad */
volatile uint8 keyPressed;

/* Variable to decide whether there is a password recorded or not */
volatile uint8 IsRecorded;

/* Variable to hold the user's choice whether to change password or open door */
volatile uint8 User_Choice = 0;

/* String to store the 5-digits password received from user or EEPROM */
volatile uint8 HMI_ECU_Password[6];

/* Flag to make sure the 2 password attempts are matched */
volatile uint8 flag = 1;

/*two counters for loops*/
volatile uint8 i;
volatile uint8 j;

/* Counter to hold the seconds */
volatile uint16 g_seconds = 0;


int main (void)
{
	BUZZER_init();
	/*LCD initialization*/
	LCD_init();

	/*UART initialization*/
	UART_config uart_config = {
			9600,
			EIGHT_BITS,
			DISABLED,
			ONE_BIT
	};

	UART_init(&uart_config);

	while(1)
	{
		UART_sendByte(HMI_ECU_IS_READY);
		IsRecorded = UART_recieveByte();

		if (IsRecorded == 0) // There is no recorded password; New User!
		{
			TASK_newPasswordEnter();
		}
		else if (IsRecorded == 1) // There is a recorded password
		{
			TASK_optionsDisplay();
		}
	}
}

void TASK_optionsDisplay (void)
{
	LCD_clearScreen();

	LCD_moveCursor(0, 0);
	LCD_displayString("+: Open door");

	LCD_moveCursor(1, 0);
	LCD_displayString("-: Change password");

	/* User_Choice stored the button pressed on the keypad */
	User_Choice = KEYPAD_getPressedKey();

	/* while loop for ignoring any inputs other than the + and - */
	while (User_Choice != '+' && User_Choice != '-')
	{
		User_Choice = KEYPAD_getPressedKey();
	}

	if (User_Choice == '-')
	{
		_delay_ms(500);

		while (UART_recieveByte() != CONTROL_ECU_IS_READY);
		UART_sendByte(User_Choice);

		TASK_passwordChange();
	}

	else if (User_Choice == '+')
	{
		_delay_ms(500);

		while (UART_recieveByte() != CONTROL_ECU_IS_READY) {}
		UART_sendByte(User_Choice);

		TASK_passwordCheck();
	}
}

void TASK_passwordChange(void)
{
	flag = 1;

	LCD_clearScreen();

	LCD_moveCursor(0, 0);
	LCD_displayString("Current password: ");

	LCD_moveCursor(1, 0);

	UART_sendByte(HMI_ECU_IS_READY);
	UART_receiveString(HMI_ECU_Password);
	HMI_ECU_Password[5]='#';

	i = 0;
	while (i < 5)
	{
		 /* get the pressed key value */
		keyPressed = KEYPAD_getPressedKey();

		/* Check every entered digit with the real password.
		 * Note: We add '0' to change the value from decimal to ascii
		 */
		if (HMI_ECU_Password[i] != keyPressed)
			flag = 0;

		LCD_displayCharacter('*');

		_delay_ms(500); /* Press time */
		i++;
	}

	/* User entered the correct password */
	if (flag == 1)
	{
		while (UART_recieveByte() != CONTROL_ECU_IS_READY);
		UART_sendByte(CORRECT);

		TASK_newPasswordEnter();
	}
	/* User enter the wrong answer */
	else if (flag == 0)
	{
		j = 0;
		do {
			flag = 1;

			LCD_clearScreen();
			LCD_displayString("Wrong Password");

			_delay_ms(500);

			LCD_clearScreen();
			LCD_displayString("Current Password:");
			LCD_moveCursor(1, 0);

			i = 0;
			while (i<5)
			{
				/* Get the pressed key value */
				keyPressed = KEYPAD_getPressedKey();

				/* Check every entered digit with the real password.
				 * Note: We add '0' to change the value from decimal to ascii
				 */
				if (HMI_ECU_Password[i] != keyPressed + '0')
					flag = 0;

				LCD_displayCharacter('*');

				_delay_ms(500); /* Press time */
				i++;
			}

			j++;

		} while ((j < 2) && (flag == 0));

		/* If the user enters 3 consecutive wrong passwords, the CONTROL_ECU should raises an alarm! */
		if (flag == 0)
		{
			TASK_alert();
		}
		else
		{
			while (UART_recieveByte() != CONTROL_ECU_IS_READY);
			UART_sendByte(CORRECT);

			TASK_newPasswordEnter();
		}
	}
}

void TASK_newPasswordEnter(void)
{
	while (1)
	{
		flag = 1;
		LCD_clearScreen();
		LCD_displayString("Enter new pass:");
		LCD_moveCursor(1, 0);

		i = 0;
		while (i < 5)
		{
			 /* Get the pressed key number */
			keyPressed = KEYPAD_getPressedKey();

			if ((keyPressed >= '0') && (keyPressed <= '9'))
			{
				HMI_ECU_Password[i] = keyPressed;

				LCD_displayCharacter('*');

				_delay_ms(500); /* Press time */
				i++;
			}
		}

		LCD_clearScreen();
		LCD_displayString("Re-enter the pass:");
		LCD_moveCursor(1, 0);

		i = 0;
		while (i < 5)
		{
			/* Get the pressed key number */
			keyPressed = KEYPAD_getPressedKey();

			if ((keyPressed >= '0') && (keyPressed <= '9'))
			{
				if (HMI_ECU_Password[i] != (keyPressed))
					flag = 0;

				LCD_displayCharacter('*');

				_delay_ms(500); /* Press time */
				i++;
			}
		}

		if (flag == 1)
		{
			/*Adding the last string character # for UART receiving and sending*/
			HMI_ECU_Password[5]= '#';

			while(UART_recieveByte() != CONTROL_ECU_IS_READY);
			UART_sendString(HMI_ECU_Password);

			IsRecorded = 1;

			LCD_clearScreen();
			LCD_displayString("Password is Recorded!");

			_delay_ms(1000);
			break;
		}
		else
		{
			LCD_clearScreen();
			LCD_displayString("Not matched!");
			_delay_ms(1000);
		}
	}
}

void TASK_passwordCheck(void)
{
	flag = 1;

	LCD_clearScreen();
	LCD_displayString("Current password:");
	LCD_moveCursor(1,0);

	UART_sendByte(HMI_ECU_IS_READY);
	UART_receiveString(HMI_ECU_Password);
	HMI_ECU_Password[5] = '#';

	i = 0;
	while (i < 5)
	{
		/* Get the pressed key number */
		keyPressed = KEYPAD_getPressedKey();
		if (HMI_ECU_Password[i] != keyPressed)
			flag = 0;

		LCD_displayCharacter('*');

		_delay_ms(500); /* Press time */
		i++;
	}

	if (flag == 1)
	{
		TASK_doorOpen();
	}
	else if (flag == 0)
	{
		j = 0;
		do {
			flag = 1;

			LCD_clearScreen();
			LCD_displayString("Wrong Password");

			_delay_ms(1000);

			LCD_clearScreen();
			LCD_displayString("Current Password:");

			LCD_moveCursor(1,0);

			i = 0;
			while (i < 5)
			{
				/* Get the pressed key number */
				keyPressed = KEYPAD_getPressedKey();
				if (HMI_ECU_Password[i] != keyPressed)
					flag = 0;

				LCD_displayCharacter('*');

				_delay_ms(500); /* Press time */
				i++;
			}
			j++;
		}while ((j < 2) && (flag == 0));

		if (flag==0)
			TASK_alert();
		else
		{
			TASK_doorOpen();
			TASK_doorStop();
			TASK_doorClose();
		}
	}
}

void TASK_doorOpen(void)
{
	while (UART_recieveByte() != CONTROL_ECU_IS_READY);
	UART_sendByte(CORRECT);

	LCD_clearScreen();
	LCD_displayString ("Opening Door");

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
	while (g_seconds <= 15 || (UART_recieveByte() != CONTROL_ECU_IS_READY));
}

void TASK_doorStop(void)
{
	LCD_clearScreen();
	LCD_displayString ("Welcome");

	g_seconds = g_ticks = 0;

	/* 3 seconds*/
	while (g_seconds <= 3);
}

void TASK_doorClose(void)
{
	LCD_clearScreen();
	LCD_displayString ("Closing Door");

	g_seconds = g_ticks = 0;

	/* 15 seconds*/
	while (g_seconds <= 15);

	TIMER0_deinit();
	g_seconds = g_ticks = 0;
}

void TASK_alert (void)
{
	while(UART_recieveByte() != CONTROL_ECU_IS_READY);
	UART_sendByte(ALERT);

	LCD_clearScreen();
	LCD_displayString("!!!Thief!!!");
	LCD_moveCursor(1, 0);
	LCD_displayString("Calling Police!!");

	TIMER0_setCallBack(TASK_TickTok);

	Timer0_ConfigType timer0_config = {
				0,
				0,
				TIMER0_F_CPU_1024,
				TIMER_NORMAL_MODE,
				0
		};

	TIMER0_init(&timer0_config);

	/* 60 seconds*/
	while (g_seconds <= 60);

	TIMER0_deinit();
	g_seconds = g_ticks = 0;
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
