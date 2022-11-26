/*
 * Title:			AGON Hexload code
 * Author:			Jeroen Venema
 * Created:			22/10/2022
 * Last Updated:	26/11/2022
 * 
 * Modinfo:
 * 22/10/2022:		Initial version MOS patch
 * 23/10/2022:		Receive_bytestream in assembly
 * 26/11/2022:		MOS commandline version
 */

#define MOS_defaultLoadAddress 0x40000		// if no address is given from the transmitted Hex file

#include "mos-interface.h"

UINT32 crc32(const char *s, UINT32 length);
extern UINT24 receive_bytestream(UINT8 *addr);
extern char getTransparentByte(void);

// Receive a bytestream from the VDU, in chunks at a time
// Accepts an extended address from the Intel HEX file. 
// If the received address is 0, the default load address is used
int main(int argc, char * argv[]) {
	UINT32 crc;
	UINT24 addressvalue;
	UINT8 *addr,*start;
	UINT8 n,value, count;
	UINT8 done = 0;
	
	// set vdu 23/28 to start HEXLOAD code at VDU
	putch(23);
	putch(28);
	// We can't transmit any text during bytestream reception, so the VDU handles this
	
	addressvalue = getTransparentByte(); 			// LSB
	addressvalue |= (getTransparentByte() << 8);
	addressvalue |= (getTransparentByte() << 16);	// MSB
	
	if(addressvalue) addr = (UINT8 *)addressvalue;
	else addr = (UINT8 *)MOS_defaultLoadAddress;

	start = addr;
	addr += receive_bytestream(addr);
	
	crc = crc32((const char *)start, (addr - start));
	// VDP will match this to it's own CRC32 and show the result
	putch(crc & 0xFF); // LSB
	putch((crc & 0xFF00) >> 8);
	putch((crc & 0xFF0000) >> 16);
	putch((crc & 0xFF000000) >> 24); // MSB
		
	return 0;
}

// Calculate a CRC32 over a block of memory
// Parameters:
// - s: Pointer to a memory location
// - length: Size of memoryblock in bytes
UINT32 crc32(const char *s, UINT32 length)
{
  static UINT32 crc;
  static UINT32 crc32_table[256];
  UINT32 i,ch,j,b,t; 

  // init a crc32 lookup table, fastest way to process the entire block
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
