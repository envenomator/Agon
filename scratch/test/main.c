#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"

UINT8 tm[6] = {43,3,25,14,35,0};

int main(int argc, char * argv[]) {
	//UINT8 tm[6] = {43,3,25,14,35,0};
	//UINT8 tm[6];
	char buffer[128];

	//tm[0] = 43;
	//tm[1] = 3;
	//tm[2] = 25;
	//tm[3] = 14;
	//tm[4] = 50;
	mos_setrtc(&tm[0]);
	mos_getrtc(buffer);
	printf("%s\r\n",buffer);
	return 0;
}

