#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "mos-setvector.h"
#include "vdp.h"

int main(int argc, char * argv[]) {

	if(MOS_SV_Check())
	{
		printf("Compatible\r\n");
	}
	else
	{
		printf("Not compatible\r\n");
	}
	return 0;
}

