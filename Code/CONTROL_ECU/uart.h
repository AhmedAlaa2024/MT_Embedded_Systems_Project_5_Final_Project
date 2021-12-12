 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the AVR UART driver
 *
 * Author: Ahmed Alaa
 *
 *******************************************************************************/

#ifndef MCAL_UART_H_
#define MCAL_UART_H_

#define F_CPU 8000000UL

#include "std_types.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum {
	DISABLED, PAR_RESERVED, EVEN, ODD
} PARITY_TYPE;

typedef enum {
	FIVE_BITS = 0, SIX_BITS = 1, SEVEN_BITS = 2, EIGHT_BITS = 3, NINE_BITS = 7
} NUMBER_OF_BITS_IN_FRAME;

typedef enum {
	ONE_BIT, TWO_BIT
} NUMBER_OF_STOP_BITS;

typedef struct {
	uint32 baud_rate;
	NUMBER_OF_BITS_IN_FRAME number_of_bits_per_frame;
	PARITY_TYPE parity_type;
	NUMBER_OF_STOP_BITS number_of_stop_bits;
} UART_config;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(UART_config *Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);
/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #
#endif /* MCAL_UART_H_ */
