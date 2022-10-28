/*
 * Title:			AGON MOS - MOS hexload code
 * Author:			Jeroen Venema
 * Created:			23/10/2022
 * Last Updated:	23/10/2022
 * 
 * Modinfo:
 */

#ifndef MOSHEXLOAD_H
#define MOSHEXLOAD_H

#include <defines.h>

int mos_cmdHEXLOAD(char *ptr);
UINT32 crc32(const char *s, UINT32 length);

extern UINT24 receive_bytestream(UINT8 *addr);
extern char getTransparentByte(void);

#endif MOSHEXLOAD_H