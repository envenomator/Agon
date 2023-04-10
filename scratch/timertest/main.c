#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

int main(int argc, char * argv[]) {

	int i;
	
	timer0_begin(1);

	for(i = 0; i < 10; i++) {
		printf("Iteration %d\r\n",i);
		delayms(500);
	}
	timer0_end();
	
	return 0;
}

