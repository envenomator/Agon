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
		PC_DR = 0x00; // output 0s
		delay100us(5);
		PC_DR = 0xFF; // output 1s
		delay100us(5);
	}
	
	return;
}