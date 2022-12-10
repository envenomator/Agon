#ifndef GAME_H
#define GAME_H

#include <defines.h>
#include "sokobanprep.h"

#define LEVELDATA		0x80000

#define NOSPRITE		255
#define BITMAPNUMBER	7
#define BITMAP_WIDTH	16
#define BITMAP_HEIGHT	16		
#define BITMAPSIZE		(BITMAP_WIDTH*BITMAP_HEIGHT)

#define FILENAME_BITMAPS	"bitmaps.bin"
#define FILENAME_LEVELS		"levels.bin"


#define TILE_WALL			0
#define TILE_PLAYER			1
#define TILE_PLAYERONGOAL	2
#define TILE_BOX			3
#define TILE_BOXONGOAL		4
#define TILE_GOAL			5
#define TILE_FLOOR			6

#define CHAR_WALL			'#'
#define CHAR_PLAYER			'@'
#define CHAR_PLAYERONGOAL	'+'
#define CHAR_BOX			'$'
#define CHAR_BOXONGOAL		'*'
#define CHAR_GOAL			'.'
#define CHAR_FLOOR			' '

typedef struct
{
	char ch;
	UINT8 tile_id;
} chartotile;

void print_playfieldText(VOID);
//UINT16 read_numberoflevels(VOID);
UINT8 game_readLevels(char *filename);
void game_sendSpriteData(void);

void game_initLevel(UINT8 levelid);
void game_initSprites(void);
void game_resetSprites(void);
void game_displayLevel(void);
BOOL game_handleKey(char key);
INT16 game_selectLevel(UINT8 levels);
//BOOL game_checkQuit(char key);
char game_getResponse(char *message, char option1, char option2);

#endif 


