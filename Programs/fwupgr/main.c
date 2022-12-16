#include <ez80.h>
#include <stdio.h>
#include <stdlib.h>
#include <ERRNO.H>
#include <LIMITS.H>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "flash.h"
#include "agontimer.h"

#define BUFFER		0x60000

int errno; // needed by standard library

void putch_UART0(CHAR pData) {
	UINT8 lsr,temt;
	
	while((UART0_LSR & 0x40) == 0);
	UART0_THR = pData;
}

void puts_UART0(char *string)
{
	while(*string)
	{
		putch_UART0(*string);
		string++;
	}
}

/*
 * Convert a string to a UINT32 integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
 
UINT32
strtoll(const char *nptr)
{
	const char *s = nptr;
	UINT32 acc;
	int c;
	int digits = 0;

	do {
		c = *s++;
	} while (isspace(c));
	if (
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
	}

	for (acc = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= 16)
			break;
		else {
			acc *= 16;
			acc += c;
			digits++;
		}
	}
	if(digits > 8) errno = ERANGE;
	return (acc);
}


// Calculate a CRC32 over a block of memory
// Parameters:
// - s: Pointer to a memory location
// - length: Size of memoryblock in bytes
UINT32 crc32(const char *s, UINT24 length)
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


int main(int argc, char * argv[]) {
	UINT32 crcuser,crcresult;
	UINT24 size = 0,mod;
	UINT8 file;
	char* ptr = (char*)BUFFER;
	UINT8 response;
	UINT8 value;
	char counter;

	printf("MOS firmware upgrade utility\n\r\n\r");
	
	if(argc != 3)
	{
		printf("Usage: FWUPGR <filename> <crc32>\n\r");
		return 0;
	}

	file = mos_fopen(argv[1], fa_read);
	if(!file)
	{
		printf("Error opening \"%s\"\n\r",argv[1]);
		return 0;
	}
	
	crcuser = strtoll(argv[2]);
	if(errno)
	{
		printf("Incorrect crc32 format\n\r");
		return 0;
	}

	printf("Loading file : %s\n\r",argv[1]);
	printf("File size    : %d byte(s)", size);

	// Read file to memory
	mod = 0;
	while(!mos_feof(file))
	{
		*ptr = mos_fgetc(file);
		ptr++;
		size++;
		mod++;
		if(mod > 1024)
		{
			mod = 0;
			printf("\rFile size    : %d byte(s)", size);

		}
	}		
	mos_fclose(file);
	printf("\rFile size    : %d byte(s)\n\r", size);
	printf("CRC32 to test: 0x%08lx\n\r",crcuser);

	crcresult = crc32((char*)BUFFER, size);

	printf("CRC32 result : 0x%08lx\n\r",crcresult);

	if(crcuser != crcresult)
	{
		printf("\n\rMismatch - aborting\n\r");
		return 0;
	}
	printf("\n\rOK\n\r\n\r");
	printf("Erase and program flash (y/n)? ");

	// Ask user to continue
	response = 0;
	while((response != 'y') && (response != 'n')) response = getch();
	if(response == 'y')
	{
		printf("\n\r");
				
		printf("Erasing flash...\n\r");
		
		/*
		enableFlashKeyRegister();	// unlock Flash Key Register, so we can write to the Flash Write/Erase protection registers
		FLASH_PROT = 0;				// disable protection on all 8x16KB blocks in the flash

		enableFlashKeyRegister();	// will need to unlock again after previous write to the flash protection register
		FLASH_FDIV = 0x5F;			// Ceiling(18Mhz * 5,1us) = 95, or 0x5F
		
		FLASH_PAGE = 112;			// test: first page of block 7, start address 0x1C000
		FLASH_PGCTL = 0x02;			// Page erase bit enable, start erase

		do
		{
			value = FLASH_PGCTL;
		}
		while(value & 0x02);// wait for completion of erase

		*/
		printf("Writing new firmware\n\r");
		printf("This will take less than 10s...");
		       
		di();						// no more access to the old MOS firmware from now on
		
		/*
		writerow();
		
		lockFlashKeyRegister();

		*/
		delayms(6000);
		puts_UART0("\rDone                           \n\r\n\r");
		for(counter = 5; counter >0; counter--)
		{
			puts_UART0("\rReset in ");
			putch_UART0(counter + '0');
			putch_UART0('s');
			delayms(1000);
		}

		reset();
		
	}
	
	else printf("\n\rUser abort\n\r");
	
	return 0;
}

