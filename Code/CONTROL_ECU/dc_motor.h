 /******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.h
 *
 * Description: DC Motor Driver
 *
 * Author: Ahmed Alaa
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define LEFT_NODE_PORT_ID	PORTB_ID
#define LEFT_NODE_PIN_ID	PIN0_ID

#define RIGHT_NODE_PORT_ID	PORTB_ID
#define RIGHT_MODE_PIN_ID	PIN1_ID
/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum {
	CW, ACW, OFF
} DcMotor_State;
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void);
/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Input:
 * 			state: The required DC Motor state, it should be CW or A-CW or stop. DcMotor_State data type should be declared as enum or uint8.
 */
void DcMotor_Rotate(DcMotor_State state);
#endif /* DC_MOTOR_H_ */
