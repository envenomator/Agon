#include <ez80.h>
#include "i2c.h"
#include <defines.h>
#include "timer.h"

// Set I2C master clock frequency
void I2C_setfrequency(UINT8 id)
{
	// Set I2C clock and sampling frequency
	switch(id)
	{
		case(I2C_SPEED_115200):
			I2C_CCR = (0x01<<3) | 0x03;	// 115.2KHz fast-mode (400KHz max), sampling at 4.6MHz
			break;
		case(I2C_SPEED_230400):
			I2C_CCR = (0x01<<3) | 0x02;	// 230.4KHz fast-mode (400KHz max), sampling at 2.3Mhz
			break;
		case(I2C_SPEED_57600):
		default:
			id = 0;
			I2C_CCR = (0x01<<3) | 0x04;	// 57.6KHz default standard-mode (100KHz max), sampling at 1.15Mhz
	}
	i2c_frequencyset = id;
}

// Internal function
void I2C_handletimeout(void)
{
	// reset the interface
	I2C_CTL = 0;
	init_I2C();
}

// Initializes the I2C bus
void init_I2C(void)
{
	CLK_PPD1 = CLK_PPD_I2C_OFF;			// Power Down I2C block before enabling it, avoid locking bug
	I2C_CTL = I2C_CTL_ENAB;				// Enable I2C block, don't enable interrupts yet
	I2C_setfrequency(i2c_frequencyset);	// Illegal values handled as default frequency
	CLK_PPD1 = 0x0;						// Power up I2C block
}

// Read length bytes from the I2C bus
// stored to data buffer from the caller
// using slave address
//
// returns 0 if OK, or specific error code
UINT8 I2C_read(UINT8 address, UINT8* data, UINT8 length)
{
	UINT8 i;
	
	// receive maximum of 32 bytes in a single I2C transaction
	if(length > I2C_MAX_BUFFERLENGTH) return 0;

	if(i2c_frequencyset == 0) I2C_setfrequency(I2C_SPEED_230400);

	// wait for READY status
	timer2 = 0;
	while(i2c_state)
	{
		// anything but ready (00)
		if(timer2 > I2C_TIMEOUTMS)
		{
			I2C_handletimeout();
			return RET_ARB_LOST;
		}
	}
	
	i2c_state = I2C_MRX;		// MRX mode
	i2c_sendstop = 0x01;	// send stops
	i2c_error = RET_OK;
	i2c_mbindex = 0;
	i2c_mbufferlength = length-1;
	//i2c_mbufferlength = length;
	i2c_slarw = (1<<0);			// receive bit 0
	i2c_slarw |= address << 1;	// shift 7-bit address one bit left

	if(i2c_inrepstart == 1)
	{
		i2c_inrepstart = 0;
		timer2 = 0;
		do
		{
			I2C_DR = i2c_slarw;
			if(timer2 > I2C_TIMEOUTMS)
			{
				I2C_handletimeout();
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

	timer2 = 0;
	while(i2c_state == I2C_MRX)
	{
		if(timer2 > I2C_TIMEOUTMS)
		{
			I2C_handletimeout();
			return RET_ARB_LOST;
		}
	}
	
	for(i = 0; i < i2c_mbindex; i++) data[i] = i2c_mbuffer[i];
	return i2c_error;
}

// Write length bytes to the I2C bus
// using slave address
//
// returns 0 if OK, or specific error code
UINT8 I2C_write(UINT8 address, const unsigned char *bytearray, UINT8 length) {
	UINT8 n,sentbytes;
	
	// send maximum of 32 bytes in a single I2C transaction
	if(length > I2C_MAX_BUFFERLENGTH) sentbytes = I2C_MAX_BUFFERLENGTH;
	else sentbytes = length;
	
	// copy bytes to write buffer, set index and number
	for(n = 0; n < sentbytes; n++) i2c_mbuffer[n] = bytearray[n];
	i2c_mbindex = 0;											// interrupt routine will start sending from this index
	i2c_mbufferlength = sentbytes;

	if(i2c_frequencyset == 0) I2C_setfrequency(I2C_SPEED_230400);

	i2c_state = I2C_MTX;		// MTX - Master Transmit Mode
	i2c_sendstop = 0x01;	// Send stop at end-of-transmission
	i2c_error = RET_OK;
		
	i2c_slarw = address << 1;	// shift one bit left, 0 on bit 0 == write action on I2C
	if(i2c_inrepstart == 1)
	{
		timer2 = 0;
		do
		{
			i2c_inrepstart = 0;
			I2C_DR = i2c_slarw;
			if(timer2 > I2C_TIMEOUTMS)
			{
				I2C_handletimeout();
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
	
	timer2 = 0;
	while(i2c_state == I2C_MTX) // while MTX
	{
		if(timer2 > I2C_TIMEOUTMS)
		{
			I2C_handletimeout();
			return RET_DATA_NACK;
		}
	}
	
	return i2c_error;
}

