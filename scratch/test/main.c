#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"

UINT8 tm[6] = {43,3,25,14,35,0};

int main(int argc, char * argv[]) {

	char buffer[128];
	mos_setrtc(&tm[0]);
	mos_getrtc(buffer);
	printf("%s\r\n",buffer);
	return 0;
}

