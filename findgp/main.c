#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

void echoVDP(uint8_t value) {
	putch(23);
	putch(0);
	putch(0x80);
	putch(value);
	delayms(100);
}

uint8_t *getGPAddress(uint8_t *sysvarstart) {
	bool found = false;
	uint8_t *ptr, pattern;
	ptr = sysvarstart;
	pattern = 0xaa;

	echoVDP(pattern);
	while(!found) {
		while(1) {
			if(*ptr == pattern) break;
			ptr++;
		}
		pattern++;
		echoVDP(pattern);
		if(*ptr == pattern) found = true;
	}
	return ptr;
}

int main(int argc, char * argv[]) {
	uint8_t *gpptr;
	sysvar_t *sysvars = getsysvars();
	uint8_t *ptr = (uint8_t *)sysvars;

	gpptr = getGPAddress(ptr);
	printf("sysvars are at 0x%06X\r\n",sysvars);
	printf("      gp is at 0x%06X\r\n",gpptr);
	return 0;
}

