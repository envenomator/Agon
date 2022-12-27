#ifndef _I2C_H_
#define _I2C_H_

#include <defines.h>

extern volatile char i2c_slarw;
extern volatile char i2c_state;
extern volatile char i2c_sendstop;
extern volatile char i2c_mbindex;
extern volatile char i2c_sendstop;
extern volatile char i2c_inrepstart;
extern volatile char i2c_mbuffer[];
extern volatile char i2c_mbufferlength;
extern volatile char i2c_debug[];
extern volatile char *i2c_debugptr;
extern volatile char i2c_debugcnt;

extern void		i2c_handler(void);

#define I2C_CTL_IEN		(1<<7)
#define I2C_CTL_ENAB	(1<<6)
#define I2C_CTL_STA		(1<<5)
#define I2C_CTL_STP		(1<<4)
#define I2C_CTL_IFLG	(1<<3)
#define I2C_CTL_AAK		(1<<2)

void init_I2C(void);
UINT8 I2C_write(UINT8 address, const char *bytearray, UINT8 length);
UINT8 I2C_read(UINT8 address, UINT8* data, UINT8 length);

#endif _I2C_H_