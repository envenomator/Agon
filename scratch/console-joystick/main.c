#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"
#include <ez80f92.h>

extern void setsysvar_keyascii(UINT8);

int main(int argc, char * argv[]) {
	uint8_t fire, direction;
	PC_DDR = 0xFF; // both joysticks
	PD_DDR |= 0xF0; // both joysticks, leave non-joystick bits 0-3 as they are

	vdp_cls();
	printf("Console8 joystick test\r\n\r\n");

	while(1) {
		fire = PD_DR;
		direction = PC_DR;
		vdp_cursorGoto(0, 2);
		printf("  PORT1     PORT2\r\n\r\n");
		printf("   Up : %d    Up : %d\r\n", ((direction&0x02)?0:1), ((direction&0x01)?0:1));
		printf(" Down : %d  Down : %d\r\n", ((direction&0x08)?0:1), ((direction&0x04)?0:1));
		printf(" Left : %d  Left : %d\r\n", ((direction&0x20)?0:1), ((direction&0x10)?0:1));
		printf("Right : %d Right : %d\r\n", ((direction&0x80)?0:1), ((direction&0x40)?0:1));
		printf("BTN 1 : %d BTN 1 : %d\r\n", ((fire&0x20)?0:1), ((fire&0x10)?0:1));
		printf("BTN 2 : %d BTN 2 : %d\r\n", ((fire&0x80)?0:1), ((fire&0x40)?0:1));

		printf("\r\nPress ESC key to quit");
		if(getsysvar_keyascii() == 0x1B) break;
		delayms(10);
	}
	printf("\r\n");
	return 0;
}

