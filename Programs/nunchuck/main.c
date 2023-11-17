#include <stdio.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"

uint8_t joyX;
uint8_t joyY;
bool buttonZ;
bool buttonC;
int angles[3];
int zero_angles[3];

int readJoyX() {
	return joyX;
}

int readJoyY() {
	return joyY;
}

bool readButtonC() {
	return buttonC;
}

bool readButtonZ() {
	return buttonZ;
}

int readAngleX() {
	return angles[0] - zero_angles[0];
}
int readAngleY() {
	return angles[1] - zero_angles[1];
}
int readAngleZ() {
	return angles[2] - zero_angles[2];
}

bool nunchuck_update(void) {
	int i;
	unsigned char values[6];
	unsigned char zero = 0;

	if(mos_i2c_read(0x52, 6, values) != 0) return false;

	joyX = (values[0]);
	joyY = (values[1]);
	for (i = 0; i < 3; i++) {
		angles[i] = (values[i+2] << 2) + ((values[5] & (3 << ((i+1)*2) ) >> ((i+1)*2))); 
	}
	buttonZ = !( values[5] & 0x01);
	buttonC = !((values[5] & 0x02) >> 1);

	mos_i2c_write(0x52, 1, &zero);
	return true;
}

void calibrate_angles(void) {
	int i;
	
	for(i = 0; i < 3; i++) {
		zero_angles[i] = angles[i];
	}
}

bool nunchuck_begin(void) {
	uint8_t cnt;
	unsigned char init1[] = {0xF0, 0x55};
	unsigned char init2[] = {0xFb, 0x00};
	cnt = 0;

	mos_i2c_open(3);
	if(mos_i2c_write(0x52, 2, init1) != 0) return false;
	delayms(1);
	if(mos_i2c_write(0x52, 2, init2) != 0) return false;
	if(!nunchuck_update()) return false;
	return true;
}

void commErrorMsg(void) {
	printf("Error connecting to Nunchuck\r\n");
}

int main(int argc, char * argv[]) {
	int xpos, ypos, btnc, btnz;

	if(!nunchuck_begin()) {
		commErrorMsg();
		return 0;
	}
	vdp_cls();
	printf("Nunchuck buttons:\r\n");
	printf("     Z - calibrate angles\r\n");
	printf(" C & Z - exit program\r\n\r\n");
	printf("Press any key to continue");
	while(getch() == 0);

	while(1) {
		delayms(100);

		if(!nunchuck_update()) {
			commErrorMsg();
			return 0;
		}
		xpos = readJoyX();
		ypos = readJoyY();
		btnc = readButtonC();
		btnz = readButtonZ();

		vdp_cls();
		printf("Angle X:%d Y:%d Z:%d\r\n", readAngleX(), readAngleY(), readAngleZ());
		if(xpos < 100) printf("Left ");
		if(xpos > 136) printf("Right ");
		if(ypos > 136) printf("Up ");
		if(ypos < 100) printf("Down ");
		if(btnc & btnz) break;
		if(btnc) {
			printf("Button C\r\n");
		}
		if(btnz) {
			printf("Button Z - calibrating angles");
			calibrate_angles();
		}
	}
	return 0;
}

