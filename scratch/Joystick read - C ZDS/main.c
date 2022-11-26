#include <ez80.h>
#include <defines.h>
#include "mos-interface.h"
#include "vdp.h"
#include "joystick.h"

void printbin16(UINT16 n)
{
	UINT24 i;
	for(i = 1 << 15; i > 0; i = i / 2) (n & i) ? putch('1') : putch('0');
	putch('\r');
}


void main(void)
{
	UINT8 c;

	vdp_cls();	
	initjoystickpins();

	while(1)
	{
		printbin16(joyread());
		for(c = 0; c < 10; c++) wait1ms();
	}
	return;
}