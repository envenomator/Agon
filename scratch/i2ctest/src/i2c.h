#ifndef _I2C_H_
#define _I2C_H_

#include <defines.h>


extern volatile	UINT24 timer2;

extern volatile char i2c_slarw;
extern volatile char i2c_error;
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

// I2C_CTL register bits
#define I2C_CTL_IEN		(1<<7)
#define I2C_CTL_ENAB	(1<<6)
#define I2C_CTL_STA		(1<<5)
#define I2C_CTL_STP		(1<<4)
#define I2C_CTL_IFLG	(1<<3)
#define I2C_CTL_AAK		(1<<2)

// ez80 PPD register bits
#define CLK_PPD_I2C_OFF	(1<<2)

// I2C return codes to caller
#define RET_OK			0x00
#define RET_SLA_NACK	0x01
#define RET_DATA_NACK	0x02
#define RET_ARB_LOST	0x04
#define RET_BUS_ERROR	0x08

// I2C constants
#define I2C_MAX_BUFFERLENGTH	32
#define I2C_TIMEOUTMS			25

// I2C role state
#define I2C_READY				0x00
#define I2C_MTX					0x01
#define I2C_MRX					0x02
#define I2C_SRX					0x04
#define I2C_STX					0x08

void init_I2C(void);
UINT8 I2C_write(UINT8 address, const unsigned char *bytearray, UINT8 length);
UINT8 I2C_read(UINT8 address, UINT8* data, UINT8 length);

#endif _I2C_H_