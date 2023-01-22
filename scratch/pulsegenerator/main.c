#include <ez80.h>
#include <defines.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

int main(int argc, char * argv[])
{	
	PC_DDR = 0x0; // all output pins on port C
	
	/*
	// countdown timer generator
	while(1)
	{
		// Produces a 500Hz signal, with 1ms pulses on Port C pins
		
		PC_DR = 0x00; // output 0s
		delayms(1);
		PC_DR = 0xFF; // output 1s
		delayms(1);
	}
	*/
	
	// freerunning timer generator
	timer0_begin(1);
	while(1)
	{
		// Produces a 500Hz signal, with 1ms pulses on Port C pins
		PC_DR = 0x00; // output 0s
		timer0 = 0;
		while(timer0 < 1);
		PC_DR = 0xFF; // output 1s
		timer0 = 0;
		while(timer0 < 1);
	}
	
	timer0_end();
	
	return 0;
}