#include "game.h"
#include "sokobanprep.h"
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>

#include "ff.h"
#include "mos.h"

const chartotile lookup[] =
{
	{CHAR_WALL, TILE_WALL},
	{CHAR_PLAYER, TILE_PLAYER},
	{CHAR_PLAYERONGOAL, TILE_PLAYERONGOAL},
	{CHAR_BOX, TILE_BOX},
	{CHAR_BOXONGOAL, TILE_BOXONGOAL},
	{CHAR_GOAL, TILE_GOAL},
	{CHAR_FLOOR, TILE_FLOOR}
};

void print_playfieldText(struct sokobanlevel *level)
{
	UINT16 width, height;
	char c;
	
	for(height = 0; height < level->height; height++)
	{
		for(width = 0; width < level->width; width++)
		{
			c = level->data[height][width];
			printf("%c",c?c:' ');
		}
		printf("\n\r");
	}
	
}

UINT16 read_levels(char *filename, struct sokobanlevel **levelbuffer)
{
	FIL	   	fil;
	UINT   	br;	
	FSIZE_t fSize;
	UINT16	levels;
	FRESULT	fr;

	fr = f_open(&fil, filename, FA_READ);
	if(fr == FR_OK) {
		fSize = f_size(&fil);
		*levelbuffer = (struct sokobanlevel *)malloc(fSize);
		fr = f_read(&fil, *levelbuffer, fSize, &br);
		levels = fSize / sizeof(struct sokobanlevel);	
	}
	else {
		mos_fileError(fr);
		free(levelbuffer);
		return 0;
	}
	f_close(&fil);
	return levels;	
}
