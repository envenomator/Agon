/*
 * Title:			AGON timer interface
 * Author:			Jeroen Venema
 * Created:			06/11/2022
 * Last Updated:	22/01/2023
 * 
 * Modinfo:
 * 06/11/2022:		Initial version
 * 22/01/2023:		Interrupt-based freerunning functions added for timer0
 * 10/04/2023:		Using mos_setintvector
 */

#include <defines.h>
#include <ez80.h>
#include "agontimer.h"
#include "mos-interface.h"

#define TMR1_COUNTER_1ms	(unsigned short)(((18432000 / 1000) * 1) / 16)

void *_timer1_prevhandler;						// used to (re)store the previous handler for the interrupt

// start timer0 on a millisecond interval
// this function registers an interrupt handler and requires timer0_end to de-register the handler after use
void timer1_begin(int interval)
{
	unsigned char tmp;
	unsigned short rr;
	
	_timer1_prevhandler = mos_setintvector(PRT0_IVECT, timer1_handler);

	timer1 = 0;
	TMR1_CTL = 0x00;
	rr = (unsigned short)(((18432000 / 1000) * interval) / 16);
	TMR1_RR_H = (unsigned char)(rr >> 8);
	TMR1_RR_L = (unsigned char)(rr);
	tmp = TMR1_CTL;
    TMR1_CTL = 0x57;
}

void timer1_end(void)
{
	TMR1_CTL = 0;
	mos_setintvector(PRT1_IVECT, _timer1_prevhandler);
}

// delay for number of ms using TMR1
// this routine doesn't use the interrupt handler for TMR1
void delayms(int ms)
{
	unsigned short rr;
	UINT16 timer1;

	rr = TMR1_COUNTER_1ms - 19;	// 1,7% correction for cycles during while(ms) loop
	
	TMR1_CTL = 0x00;	// disable timer0
	
	while(ms)
	{	
		TMR1_RR_H = (unsigned char)(rr >> 8);
		TMR1_RR_L = (unsigned char)(rr);
				
		TMR1_CTL = 0x87; // enable, single pass, stop at 0, start countdown immediately
		do
		{
			timer1 = TMR1_DR_L;
			timer1 = timer1 | (TMR1_DR_H << 8);		
		}
		while(timer1);
		ms--;
	}
}