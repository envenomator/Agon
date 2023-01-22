#include <ez80.h>
#include "i2c.h"
#include <defines.h>
#include "agontimer.h"
#include "mos-setvector.h"

void *_i2c_prevhandler;						// used to (re)store the previous handler for the interrupt
UINT8 _i2c_address;							// either master (0) or slave node address
//void (*i2c_onSlaveReceive)(UINT8*, UINT8);	// used to store slave callback function
//void (*i2c_onSlaveTransmit)(void);			// used to store slave callback function

extern void i2c_init(void);			// assembly function, power cycle the I2C block and reset it

void _i2c_handletimeout(void)
{
	// reset the interface
	I2C_CTL = 0;
	i2c_init();
}

// User function to begin using the I2C bus
// Starts out as a Master until slave address set later on
void i2c_begin(UINT8 frequency_id)
{
	// Start out as a MASTER
	_i2c_address = 0;
	// start timer0
	timer0_begin(1);
	// register interrupt handler
	_i2c_prevhandler = mos_setvector(I2C_IVECT, i2c_handler);
	// Set I2C clock and sampling frequency
	switch(frequency_id)
	{
		case(I2C_SPEED_115200):
			I2C_CCR = (0x01<<3) | 0x03;	// 115.2KHz fast-mode (400KHz max), sampling at 4.6MHz
			break;
		case(I2C_SPEED_230400):
			I2C_CCR = (0x01<<3) | 0x02;	// 230.4KHz fast-mode (400KHz max), sampling at 2.3Mhz
			break;
		case(I2C_SPEED_57600):
		default:
			I2C_CCR = (0x01<<3) | 0x04;	// 57.6KHz default standard-mode (100KHz max), sampling at 1.15Mhz
	}
	// initialize the I2C bus
	i2c_init();
}

void i2c_end(void)
{
	_i2c_address = 0;							// reset any slave address used
	I2C_CTL = 0;								// disable I2C control interface and disable interrupt
	mos_setvector(I2C_IVECT, _i2c_prevhandler);	// restore previous handler
	timer0_end();
}

void i2c_setAddress(UINT8 slave_address)
{
	_i2c_address = slave_address;
}

BOOL i2c_available(void)
{
	return(i2c_mbindex != i2c_mbufferend);	// if these two aren't the same, there is (still) data in the buffer
}

/*
void i2c_setReceiveHandler(void (*function)(UINT8*, UINT8))
{
	i2c_onSlaveReceive = function;
}

void i2c_setTransmitHandler(void (*function)(void))
{
	i2c_onSlaveTransmit = function;
}
*/

// Read length bytes from the I2C bus
// stored to data buffer from the caller
// using slave address or 0 if reading from master
//
// returns 0 if OK, or specific error code
UINT8 I2C_readFrom(UINT8 address, UINT8* data, UINT8 length)
{
	UINT8 i;

	// Check if I2C has been started using begin()
	if(I2C_CTL == 0) return 0;
	// receive maximum of 32 bytes in a single I2C transaction
	if(length > I2C_MAX_BUFFERLENGTH) return 0;
	// wait for READY status
	timer0 = 0;
	while(i2c_state)
	{
		// anything but ready (00)
		if(timer0 > I2C_TIMEOUTMS)
		{
			_i2c_handletimeout();
			return RET_ARB_LOST;
		}
	}
	
	i2c_state = I2C_MRX;		// MRX mode
	i2c_sendstop = 0x01;	// send stops
	i2c_error = RET_OK;
	i2c_mbindex = 0;
	i2c_mbufferend = length-1;
	i2c_slarw = (1<<0);			// receive bit 0
	i2c_slarw |= address << 1;	// shift 7-bit address one bit left

	if(i2c_inrepstart == 1)
	{
		i2c_inrepstart = 0;
		timer0 = 0;
		do
		{
			I2C_DR = i2c_slarw;
			if(timer0 > I2C_TIMEOUTMS)
			{
				_i2c_handletimeout();
				return RET_SLA_NACK;
			}
		}
		while(I2C_CTL & I2C_CTL_STA); // while start isn't finished
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB;
	}
	else
	{
		// send start condition
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STA;
	}

	timer0 = 0;
	while(i2c_state == I2C_MRX)
	{
		if(timer0 > I2C_TIMEOUTMS)
		{
			_i2c_handletimeout();
			return RET_ARB_LOST;
		}
	}
	
	for(i = 0; i < i2c_mbindex; i++) data[i] = i2c_mbuffer[i];
	return i2c_error;
}

// Write length bytes to the I2C bus
// using slave address or 0 if writing to master
//
// returns 0 if OK, or specific error code
UINT8 i2c_writeTo(UINT8 address, const unsigned char *bytearray, UINT8 length) {
	UINT8 n,sentbytes;
	
	// Check if I2C has been started using begin()
	if(I2C_CTL == 0) return 0;
	// send maximum of 32 bytes in a single I2C transaction
	if(length > I2C_MAX_BUFFERLENGTH) sentbytes = I2C_MAX_BUFFERLENGTH;
	else sentbytes = length;
	
	// copy bytes to write buffer, set index and number
	for(n = 0; n < sentbytes; n++) i2c_mbuffer[n] = bytearray[n];
	i2c_mbindex = 0;											// interrupt routine will start sending from this index
	i2c_mbufferend = sentbytes - 1;

	i2c_state = I2C_MTX;		// MTX - Master Transmit Mode
	i2c_sendstop = 0x01;	// Send stop at end-of-transmission
	i2c_error = RET_OK;
		
	i2c_slarw = address << 1;	// shift one bit left, 0 on bit 0 == write action on I2C
	if(i2c_inrepstart == 1)
	{
		timer0 = 0;
		do
		{
			i2c_inrepstart = 0;
			I2C_DR = i2c_slarw;
			if(timer0 > I2C_TIMEOUTMS)
			{
				_i2c_handletimeout();
				return RET_SLA_NACK;
			}
		}
		while(I2C_CTL & I2C_CTL_STA); // while start isn't finished
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB;
	}
	else
	{
		// send start condition
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STA;
	}
	
	timer0 = 0;
	while(i2c_state == I2C_MTX) // while MTX
	{
		if(timer0 > I2C_TIMEOUTMS)
		{
			_i2c_handletimeout();
			return RET_DATA_NACK;
		}
	}
	
	return i2c_error;
}

