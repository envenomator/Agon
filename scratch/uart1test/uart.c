/*
 * Title:			AGON - Loadable UART1 code
 * Author:			Jeroen Venema, original by Dean Belfeld
 * Created:			06/01/2023
 * Last Updated:	06/01/2023
 * 
 * The UART1 is on Port C
 *
 * - Port C0: TX
 * - Port C1: RX
 */
 
#include <stddef.h>
#include <stdio.h>
#include <eZ80.h>
#include <defines.h>
#include <gpio.h>

#include "uart.h"
 
// Set the Line Control Register for data, stop and parity bits
//
#define SETREG_LCR1(data, stop, parity) (UART1_LCTL = ((BYTE)(((data)-(BYTE)5)&(BYTE)0x3)|(BYTE)((((stop)-(BYTE)0x1)&(BYTE)0x1)<<(BYTE)0x2)|(BYTE)((parity)<<(BYTE)0x3)))

VOID init_UART1() {
	PC_DR = PORTC_DRVAL_DEF;
	PC_DDR = PORTC_DDRVAL_DEF;
	PC_ALT1 = PORTC_ALT1VAL_DEF;
	PC_ALT2 = PORTC_ALT2VAL_DEF;
	return ;
}

// Open UART
// Parameters:
// - pUART: Structure containing the initialisation data
//
UCHAR open_UART1(UART *pUART) {
	UINT16	br = MASTERCLOCK / (CLOCK_DIVISOR_16 * pUART->baudRate);//! Compute the baudrate generator value;
	UCHAR	pins = PORTPIN_ZERO | PORTPIN_ONE;						//! The transmit and receive pins
	
	SETREG(PC_DDR, pins);											//! Set Port C bits 0, 1 (TX. RX) for alternate function.
	RESETREG(PC_ALT1, pins);
	SETREG(PC_ALT2, pins);
	
	SETREG(PC_DDR, PORTPIN_THREE);									//! Set Port C bit 3 (CTS) for input
	RESETREG(PC_ALT1, PORTPIN_THREE);
	RESETREG(PC_ALT2, PORTPIN_THREE);
	
	UART1_LCTL |= UART_LCTL_DLAB ;									//! Select DLAB to access baud rate generators
	UART1_BRG_L = (br & 0xFF) ;										//! Load divisor low
	UART1_BRG_H = (CHAR)(( br & 0xFF00 ) >> 8) ;					//! Load divisor high
	UART1_LCTL &= (~UART_LCTL_DLAB) ; 								//! Reset DLAB; dont disturb other bits
	UART1_MCTL = 0x00 ;												//! Bring modem control register to reset value.
	UART1_FCTL = 0x07 ;												//! Disable hardware FIFOs.
	UART1_IER = UART_IER_RECEIVEINT ;
	
	SETREG_LCR1(pUART->dataBits, pUART->stopBits, pUART->parity);	//! Set the line status register.
	
	return UART_ERR_NONE ;
}

VOID uart1_puts(CHAR *str)
{
	while(*str)	uart1_putch(*str++);
}

