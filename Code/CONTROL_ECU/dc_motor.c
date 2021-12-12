 /******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.c
 *
 * Description: DC Motor Driver
 *
 * Author: Ahmed Alaa
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "gpio.h"
/*******************************************************************************
 *                         	    		Functions	                  	       *
 *******************************************************************************/
/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void)
{
	/* Setup the pin controlling the direction of dc motor */
	GPIO_setupPinDirection(LEFT_NODE_PORT_ID, LEFT_NODE_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(RIGHT_NODE_PORT_ID, RIGHT_MODE_PIN_ID, PIN_OUTPUT);

	/* Stop the dc motor */
	GPIO_writePin(LEFT_NODE_PORT_ID, LEFT_NODE_PIN_ID, LOGIC_LOW);
	GPIO_writePin(RIGHT_NODE_PORT_ID, RIGHT_MODE_PIN_ID, LOGIC_LOW);
}
/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Input:
 * 			state: The required DC Motor state, it should be CW or A-CW or stop. DcMotor_State data type should be declared as enum or uint8.
 */
void DcMotor_Rotate(DcMotor_State state)
{
	if (state == CW)
	{
		GPIO_writePin(LEFT_NODE_PORT_ID, LEFT_NODE_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(RIGHT_NODE_PORT_ID, RIGHT_MODE_PIN_ID, LOGIC_LOW);
	}
	else if (state == ACW)
	{
		GPIO_writePin(LEFT_NODE_PORT_ID, LEFT_NODE_PIN_ID, LOGIC_LOW);
		GPIO_writePin(RIGHT_NODE_PORT_ID, RIGHT_MODE_PIN_ID, LOGIC_HIGH);
	}
	else if (state == OFF)
	{
		GPIO_writePin(LEFT_NODE_PORT_ID, LEFT_NODE_PIN_ID, LOGIC_LOW);
		GPIO_writePin(RIGHT_NODE_PORT_ID, RIGHT_MODE_PIN_ID, LOGIC_LOW);
	}
}
