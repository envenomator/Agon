#include <stdint.h>
#include "ez80io.h"
#include "timer.h"
#include "eZ80F92.h"

#define TMR1_COUNTER_1ms	(unsigned short)(((18432000 / 1000) * 1) / 16)

void delayms(int ms)
{
	unsigned short rr;
	uint16_t timer1;

	rr = TMR1_COUNTER_1ms - 19;	// 1,7% correction for cycles during while(ms) loop
	
	set_port(TMR1_CTL, 0x00);	// disable timer0
	
	while(ms)
	{	
		set_port(TMR1_RR_H , (unsigned char)(rr >> 8));
		set_port(TMR1_RR_L , (unsigned char)(rr));
				
		set_port(TMR1_CTL , 0x87); // enable, single pass, stop at 0, start countdown immediately
		do
		{
			timer1 = get_port(TMR1_DR_L);
			timer1 = timer1 | (get_port(TMR1_DR_H) << 8);		
		}
		while(timer1);
		ms--;
	}
}
