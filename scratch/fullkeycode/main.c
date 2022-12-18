#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "mos-keycode.h"
#include "agontimer.h"

int main(int argc, char * argv[]) {
	UINT16 key;
	
	printf("Press any key:\r\n\r\n");
	while(1)
	{
		key = getfullkeycode();
		delayms(5);
		printf("\rKeycode %2x, mod %2x",(key&0xFF),(key&0xFF00)>>8);
	}
	return 0;
}

