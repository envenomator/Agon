#ifndef SOKOBANPREP_H
#define SOKOBANPREP_H

#include <defines.h>

#define MAXWIDTH 40
#define MAXHEIGHT 30

struct sokobanlevel
{
	UINT8 xpos;
	UINT8 ypos;
	UINT8 width;
	UINT8 height;
	UINT8 goals;
	UINT8 goalstaken;
	UINT8 crates;	
	UINT8 data[MAXHEIGHT][MAXWIDTH];
};

#endif 