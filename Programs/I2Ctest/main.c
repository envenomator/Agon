#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "agontimer.h"
#include "1602.h"
#include "dht20.h"
#include <float.h>
#include <format.h>
#include <string.h>
int errno;

int dht(void) {

	int status;
	
	printf("Humidity Temp Status\r\n");
	status = DHT20_read();
	// DISPLAY DATA, sensor has only one decimal.
	printf("%8.1f ", DHT20_getHumidity());
	printf("%4.1f ", DHT20_getTemperature());
	switch (status)
	{
		case DHT20_OK:
		printf("OK");
		break;
		case DHT20_ERROR_CHECKSUM:
		printf("Checksum error");
		break;
		case DHT20_ERROR_CONNECT:
		printf("Connect error");
		break;
		case DHT20_MISSING_BYTES:
		printf("Missing bytes");
		break;
		case DHT20_ERROR_BYTES_ALL_ZERO:
		printf("All bytes read zero");
		break;
		case DHT20_ERROR_READ_TIMEOUT:
		printf("Read time out");
		break;
		default:
		printf("Unknown error");
		break;
	}
	printf("\r\n");
	return 0;
}

void lcd(char *text) {
	
	lcd1602Init(0x3F);
	delayms(100);
	lcd1602WriteString(text);
	//lcd1602SetCursor(0,1);
	//lcd1602WriteString("Test2");
	
	delayms(2000);
	lcd1602Clear();
}

bool isPresent(UINT8 i2c_address) {
	return (!mos_i2c_write(i2c_address, 0, 0));
}

void scanBus(void) {
	int n;

	printf("Scanning I2C bus:\r\n");
	for(n = 0; n < 128; n++) {
		if(isPresent(n)) {
			printf("Address 0x%02X\r\n", n);
		}
	}
}

int main(int argc, char * argv[]) {
	uint8_t frequency;
	uint8_t address;

	frequency = 3;
	mos_i2c_open(frequency);

	if(argc == 1) {
		scanBus();
		return 0;
	};

	if(strcmp(argv[1], "dht") == 0) dht();
	if(strcmp(argv[1], "lcd") == 0) {
		if(argc == 3) lcd(argv[2]);
		else lcd("Test");
	}

	mos_i2c_close();
	return 0;
}

