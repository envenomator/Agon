/*
 * Title:			AGON MOS - MOS Hexload code
 * Author:			Jeroen Venema
 * Created:			23/10/2022
 * Last Updated:	23/10/2022
 * 
 * Modinfo:
 * 23/10/2022:		Initial version receive_bytestream in assembly
 */
 
#include "mos_hexload.h"
#include "uart.h"
#include "config.h"

// Calculate a CRC32 over a block of memory
// Parameters:
// - s: Pointer to a memory location
// - length: Size of memoryblock in bytes
//
UINT32 crc32(const char *s, UINT32 length)
{
  static UINT32 crc;
  static UINT32 crc32_table[256];
  UINT32 i,ch,j,b,t; 

  // init a crc32 lookup table, fastest way to process the block
  for(i = 0; i < 256; i++)
  {
    ch = i;
    crc = 0;
    for(j = 0; j < 8; j++)
    {
	  b=(ch^crc)&1;
	  crc>>=1;
	  if(b) crc=crc^0xEDB88320;
	  ch>>=1;
    }
    crc32_table[i] = crc;
  }        

  // calculate the crc using the table
  crc = 0xFFFFFFFF;
  for(i=0;i<length;i++)
  {
    char ch=s[i];
    t=(ch^crc)&0xFF;
    crc=(crc>>8)^crc32_table[t];
  }

  return ~crc;
}

// Receive a bytestream from the VDU, in chunks at a time
// Parameters:
// - ptr: Pointer to the argument string in the line edit buffer (not used currently)
//
// Function accepts an extended address from the VDU. 
// When the received address is 0, the MOS_defaultLoadAddress is used
//
int mos_cmdHEXLOAD(char *ptr){
	UINT32 crc;
	UINT24 addressvalue;
	UINT8 *addr,*start;
	UINT8 n,value, count;
	UINT8 done = 0;
	
	// set vdu 23/28 to start HEXLOAD code at VDU
	putch(23);
	putch(28);
	
	addressvalue = getTransparentByte(); 			// LSB
	addressvalue |= (getTransparentByte() << 8);
	addressvalue |= (getTransparentByte() << 16);	// MSB
	
	if(addressvalue) addr = (UINT8 *)addressvalue;
	else addr = (UINT8 *)MOS_defaultLoadAddress;

	start = addr;
	addr += receive_bytestream(addr);
	
	crc = crc32((const char *)start, (addr - start));
	
	putch(crc & 0xFF); // LSB
	putch((crc & 0xFF00) >> 8);
	putch((crc & 0xFF0000) >> 16);
	putch((crc & 0xFF000000) >> 24); // MSB
	
	return 0; // VDP will match and show the result
}
