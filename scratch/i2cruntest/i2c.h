#ifndef _I2C_H_
#define _I2C_H_

#include <defines.h>


extern volatile	UINT24 timer2;

extern volatile char i2c_slarw;
extern volatile char i2c_error;
extern volatile char i2c_state;
extern volatile char i2c_frequencyset;
extern volatile char i2c_sendstop;
extern volatile char i2c_mbindex;
extern volatile char i2c_sendstop;
extern volatile char i2c_inrepstart;
extern volatile char i2c_mbuffer[];
extern volatile char i2c_mbufferend;

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
#define I2C_SPEED_NOTSET		0x00
#define I2C_SPEED_57600			0x01
#define I2C_SPEED_115200		0x02
#define I2C_SPEED_230400		0x03

// I2C role state
#define I2C_READY				0x00
#define I2C_MTX					0x01
#define I2C_MRX					0x02
#define I2C_SRX					0x04
#define I2C_STX					0x08

void i2c_begin(UINT8 frequency_id);
void i2c_end(void);
void i2c_setAddress(UINT8 slave_address);
BOOL i2c_available(void);
void i2c_setReceiveHandler(void (*function)(UINT8*, UINT8));
void i2c_setTransmitHandler(void (*function)(void));

UINT8 i2c_writeTo(UINT8 address, UINT8 *data, UINT8 length);
UINT8 i2c_readFrom(UINT8 address, UINT8* data, UINT8 length);

#endif _I2C_H_