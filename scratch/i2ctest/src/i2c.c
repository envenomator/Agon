#include <ez80.h>
#include "i2c.h"
#include <defines.h>
#include "timer.h"

extern volatile	UINT24 timer2;

#define CLK_PPD_I2C_OFF	(1<<2)

void init_I2C(void)
{
	CLK_PPD1 = CLK_PPD_I2C_OFF;		// Power Down I2C block before enabling it, avoid locking bug
	I2C_CTL = I2C_CTL_ENAB;			// Enable I2C block, don't enable interrupts yet
	CLK_PPD1 = 0x0;					// Power up I2C block
}

void I2C_handletimeout(void)
{
	// reset the interface
	I2C_CTL = 0;
	init_I2C();
}

UINT8 I2C_read(UINT8 address, UINT8* data, UINT8 length)
{
	UINT8 i;
	
	// receive maximum of 32 bytes in a single I2C transaction
	if(length > 32) return 0;
	
	// wait for READY status
	timer2 = 0;
	while(i2c_state)
	{
		// anything but ready (00)
		if(timer2 > 25)
		{
			I2C_handletimeout();
			return 10;
		}
	}
	// Set I2C clock and sampling frequency
	I2C_CCR = 0x1C;			// 7 reserved bit, M=3, N=2. 57.6Khz, 1.152Mhz sampling frequency
	
	i2c_state = 0x02;		// MRX mode
	i2c_sendstop = 0x01;	// send stops
	i2c_mbindex = 0;
	i2c_mbufferlength = length-1;
	//i2c_mbufferlength = length;
	i2c_slarw = 0x01;			// receive bit
	i2c_slarw |= address << 1;	// shift 7-bit address one bit left

	if(i2c_inrepstart == 1)
	{
		i2c_inrepstart = 0;
		timer2 = 0;
		do
		{
			I2C_DR = i2c_slarw;
			if(timer2 > 25)
			{
				I2C_handletimeout();
				return 20;
			}
		}
		while(I2C_CTL & I2C_CTL_STA); // while start isn't finished
		//delayms(1);
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB;
	}
	else
	{
		// send start condition
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STA;
	}

	timer2 = 0;
	while(i2c_state == 0x02)
	{
		if(timer2 > 25)
		{
			I2C_handletimeout();
			return 30;
			//return i2c_mbindex;
		}
	}
	
	for(i = 0; i < i2c_mbindex; i++) data[i] = i2c_mbuffer[i];
	return i2c_mbindex;
}

UINT8 I2C_write(UINT8 address, const char *bytearray, UINT8 length) {
	UINT8 n,sentbytes;
	
	// send maximum of 32 bytes in a single I2C transaction
	if(length > 32) sentbytes = 32;
	else sentbytes = length;
	
	// copy bytes to write buffer, set index and number
	for(n = 0; n < sentbytes; n++) i2c_mbuffer[n] = bytearray[n];
	i2c_mbindex = 0;											// interrupt routine will start sending from this index
	i2c_mbufferlength = sentbytes;
	
	// Set I2C clock and sampling frequency
	I2C_CCR = 0x1C;			// 7 reserved bit, M=3, N=2. 57.6Khz, 1.152Mhz sampling frequency

	i2c_state = 0x01;		// MTX - Master Transmit Mode
	i2c_sendstop = 0x01;	// Send stop at end-of-transmission
		
	i2c_slarw = address << 1;	// shift one bit left, 0 on bit 0 == write action on I2C
	if(i2c_inrepstart == 1)
	{
		timer2 = 0;
		do
		{
			i2c_inrepstart = 0;
			I2C_DR = i2c_slarw;
			if(timer2 > 25)
			{
				I2C_handletimeout();
				return 0;
			}
		}
		while(I2C_CTL & I2C_CTL_STA); // while start isn't finished
		//delayms(1);
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB;
	}
	else
	{
		// send start condition
		I2C_CTL = I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STA;
	}
	
	timer2 = 0;
	while(i2c_state == 0x01) // while MTX
	{
		if(timer2 > 24)
		{
			I2C_handletimeout();
			return i2c_mbindex;
		}
	}
	
	return i2c_mbindex;
}
