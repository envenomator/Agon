#include <ez80.h>
#include <defines.h>
#include <stddef.h>
#include "i2c.h"
#include "agontimer.h"
#include "timer.h"

#define PULSE_PERIOD 500
#define CMD_PERIOD 4100
#define PERIOD100US	100

#define BACKLIGHT 8
#define DATA 1
static UINT8 iBackLight = BACKLIGHT;
static UINT8 address = 0x27;			// default address

static void I2C_writeCommand(unsigned char ucCMD)
{
	unsigned char ucmd;

	ucmd = (ucCMD & 0xf0) | iBackLight; // most significant nibble sent first
	I2C_write(address, &ucmd, 1);
	delay100us(PULSE_PERIOD/PERIOD100US); // manually pulse the clock line
	ucmd |= 4; // enable pulse
	I2C_write(address, &ucmd, 1);
	delay100us(PULSE_PERIOD/PERIOD100US);
	ucmd &= ~4; // toggle pulse
	I2C_write(address, &ucmd, 1);
	delay100us(CMD_PERIOD/PERIOD100US);
	ucmd = iBackLight | (ucCMD << 4); // least significant nibble
	I2C_write(address, &ucmd, 1);
	delay100us(PULSE_PERIOD/PERIOD100US);
        ucmd |= 4; // enable pulse
        I2C_write(address, &ucmd, 1);
        delay100us(PULSE_PERIOD/PERIOD100US);
        ucmd &= ~4; // toggle pulse
        I2C_write(address, &ucmd, 1);
	delay100us(CMD_PERIOD/PERIOD100US);

} 
int lcd1602Control(UINT8 bBacklight, UINT8 bCursor, UINT8 bBlink)
{
unsigned char ucCMD = 0xc; // display control

	iBackLight = (bBacklight) ? BACKLIGHT : 0;
	if (bCursor)
		ucCMD |= 2;
	if (bBlink)
		ucCMD |= 1;
	I2C_writeCommand(ucCMD);
 	
	return 0;
} /* lcd1602Control() */

//
// I2C_write an ASCII string (up to 16 characters at a time)
// 
UINT8 lcd1602WriteString(char *text)
{
unsigned char ucTemp[2];
int i = 0;

	if (text == NULL)
		return 1;

	while (i<16 && *text)
	{
		ucTemp[0] = iBackLight | DATA | (*text & 0xf0);
		I2C_write(address, ucTemp, 1);
		delay100us(PULSE_PERIOD/PERIOD100US);
		ucTemp[0] |= 4; // pulse E
		I2C_write(address, ucTemp, 1);
		delay100us(PULSE_PERIOD/PERIOD100US);
		ucTemp[0] &= ~4;
		I2C_write(address, ucTemp, 1);
		delay100us(PULSE_PERIOD/PERIOD100US);
		ucTemp[0] = iBackLight | DATA | (*text << 4);
		I2C_write(address, ucTemp, 1);
		ucTemp[0] |= 4; // pulse E
                I2C_write(address, ucTemp, 1);
                delay100us(PULSE_PERIOD/PERIOD100US);
                ucTemp[0] &= ~4;
                I2C_write(address, ucTemp, 1);
                delay100us(CMD_PERIOD/PERIOD100US);
		text++;
		i++;
	}
	return 0;
}

UINT8 lcd1602Clear(void)
{
	I2C_writeCommand(0x0E); // clear the screen
	return 0;

} 

UINT8 lcd1602Init(UINT8 iAddr)
{
	address = iAddr;
	delayms(20);
	iBackLight = BACKLIGHT; // turn on backlight
	I2C_writeCommand(0x02); // Set 4-bit mode of the LCD controller
	I2C_writeCommand(0x28); // 2 lines, 5x8 dot matrix
	I2C_writeCommand(0x0c); // display on, cursor off
	I2C_writeCommand(0x06); // inc cursor to right when writing and don't scroll
	I2C_writeCommand(0x80); // set cursor to row 1, column 1
	lcd1602Clear();	    // clear the memory

	return 0;
}

UINT8 lcd1602SetCursor(UINT8 x, UINT8 y)
{
unsigned char cCmd;

	if (x < 0 || x > 15 || y < 0 || y > 1)
		return 1;

	cCmd = (y==0) ? 0x80 : 0xc0;
	cCmd |= x;
	I2C_writeCommand(cCmd);
	return 0;

}

void lcd1602Shutdown(void)
{
	iBackLight = 0; // turn off backlight
	I2C_writeCommand(0x08); // turn off display, cursor and blink	
}