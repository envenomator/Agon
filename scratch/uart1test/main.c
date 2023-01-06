#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "uart.h"
#include <string.h>

typedef void * rom_set_vector(unsigned int vector, void(*handler)(void));

CHAR message[] = "Testing UART1, testing...\r\n";

int main(int argc, char * argv[]) {
	CHAR c;
	rom_set_vector *set_vector = (rom_set_vector *)0x000956;	// as assembled in MOS 1.02, until set_vector becomes a API call in a later MOS version
	UART 	pUART;

	pUART.baudRate = 115200;
	pUART.dataBits = 8;
	pUART.stopBits = 1;
	pUART.parity = PAR_NOPARITY;

	set_vector(UART1_IVECT, uart1_handler);
	init_UART1();
	open_UART1(&pUART);								// Open the UART 

	printf("Writing message to UART1\r\n");
	uart1_puts(message);
	
	while(1)
	{
		c = uart1_getch();
		putch(c);
		//putch_uart1(c);
		uart1_putch(c);
		if(c == 0x0d)
		{
			putch(0x0a);
			uart1_putch(c);
			//putch_uart1(0x0a);
		}
	}
	return 0;
}

