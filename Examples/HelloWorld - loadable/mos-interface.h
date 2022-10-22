/*
 * Title:			AGON MOS - MOS c header interface
 * Author:			Jeroen Venema
 * Created:			15/10/2022
 * Last Updated:	15/10/2022
 * 
 * Modinfo:
 * 15/10/2022:		Added putch, getch
 * 22/10/2022:		Added waitvblank, mos_f* functions
 */

#ifndef MOS_H
#define MOS_H

#include <defines.h>

// File access modes
#define fa_read				0x01
#define fa_write			0x02
#define fa_open_existing	0x00
#define fa_create_new		0x04
#define fa_create_always	0x08
#define fa_open_always		0x10
#define fa_open_append		0x30

extern void  putch(char a);
extern char  getch(void);
extern void  puts(char *str);
extern void  waitvblank(void);

extern UINT8 mos_fopen(char * filename, UINT8 mode); // returns filehandle, or 0 on error
extern UINT8 mos_fclose(UINT8 fh);					 // returns number of still open files
extern char	 mos_fgetc(UINT8 fh);					 // returns character from file
extern void	 mos_fputc(UINT8 fh, char c);			 // writes character to file
extern UINT8 mos_feof(UINT8 fh);					 // returns 1 if EOF, 0 otherwise

#endif MOS_H