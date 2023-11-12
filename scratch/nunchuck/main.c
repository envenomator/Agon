#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

#define DEFAULT_ZERO_JOY_X 124
#define DEFAULT_ZERO_JOY_Y 132
uint8_t zeroJoyX;
uint8_t zeroJoyY;
int lastJoyX;
int lastJoyY;
uint8_t joyX;
uint8_t joyY;
bool buttonZ;
bool buttonC;

int readJoyX() {
	return (int) joyX;// - zeroJoyX;
}

int readJoyY() {
	return (int)joyY;// - zeroJoyY;
}

void nunchuck_update(void) {
	unsigned char values[6];
	unsigned char zero = 0;

	if(mos_i2c_read(0x52, 6, values) == 0)
	{
		lastJoyX = readJoyX();
		lastJoyY = readJoyY();

		joyX = (values[0]);
		joyY = (values[1]);

		buttonZ = !( values[5] & 0x01);
		buttonC = !((values[5] & 0x02) >> 1);

		mos_i2c_write(0x52, 1, &zero);
	}
	else {
		printf("Error reading nunchuck\r\n");
	}
}

void nunchuck_begin(void) {
	uint8_t cnt;
	unsigned char init1[] = {0xF0, 0x55};
	unsigned char init2[] = {0xFb, 0x00};
	cnt = 0;
	//averagecounter = 0;
	mos_i2c_open(3);
	mos_i2c_write(0x52, 2, init1);
	delayms(1);
	mos_i2c_write(0x52, 2, init2);
	nunchuck_update();

	//zeroJoyX = joyX;
	//zeroJoyY = joyY;
}

int main(int argc, char * argv[]) {
	int x, y;

	nunchuck_begin();

	while(1) {
		delayms(100);

		nunchuck_update();
		x = readJoyX();
		y = readJoyY();

		//printf("X: %d Y: %d               \r", readJoyX(), readJoyY());
	
		vdp_cls();
		if(x < 100) printf("Left ");
		if(x > 136) printf("Right ");
		if(y > 136) printf("Up ");
		if(y < 100) printf("Down ");
		if(buttonC) printf("Button C ");
		if(buttonZ) printf("Button Z ");
	}
	return 0;
}

