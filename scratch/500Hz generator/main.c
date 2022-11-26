#include <ez80.h>
#include <defines.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

void main(void)
{	
	PC_DDR = 0x0; // all output pins on port C
	
	while(1)
	{
		// Produces a 500Hz signal, with 1ms pulses on Port C pins
		
		PC_DR = 0x00; // output 0s
		delayms(1);
		PC_DR = 0xFF; // output 1s
		delayms(1);
	}
	
	return;
}