#ifndef GAME_H
#define GAME_H

#include <defines.h>
#include "sokobanprep.h"

#define LEVELDATA		0x60000
#define UNDOBUFFERSIZE	128

#define NOSPRITE		255
#define BITMAPNUMBER	7
#define BITMAP_WIDTH	16
#define BITMAP_HEIGHT	16	
#define MINIMAP_WIDTH	8
#define MINIMAP_HEIGHT	8
#define BITMAPSIZE		(BITMAP_WIDTH*BITMAP_HEIGHT)

#define HELP_XPOS_MODE1		44
#define HELP_YPOS_MODE1		10
#define HELP_XPOS_MODE0		30
#define HELP_YPOS_MODE0		13


#define TILE_WALL				0
#define TILE_PLAYER				1
#define TILE_PLAYERONGOAL		2
#define TILE_BOX				3
#define TILE_BOXONGOAL			4
#define TILE_GOAL				5
#define TILE_FLOOR				6

#define TILE_WALL_MINI			7
#define TILE_PLAYER_MINI		8
#define TILE_PLAYERONGOAL_MINI	9
#define TILE_BOX_MINI			10
#define TILE_BOXONGOAL_MINI		11
#define TILE_GOAL_MINI			12
#define TILE_FLOOR_MINI			13

#define TILE_MINIMAP_CLEAR		14

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

struct undoitem
{
	UINT8 movekey;	// the key pressed to initiate this move to potentially undo
	BOOL  pushed;	// an item was pushed in this move to potentially undo
};

void print_playfieldText(VOID);
//UINT16 read_numberoflevels(VOID);
UINT8 game_readLevels(char *filename);
void game_sendSpriteData(void);

void game_initLevel(UINT8 levelid);
void game_initSprites(void);
void game_resetSprites(void);
void game_displayLevel(void);
void game_displayMinimap(void);
BOOL game_handleKey(char key);
INT16 game_selectLevel(UINT8 levels, UINT16 levelnumber);
//BOOL game_checkQuit(char key);
char game_getResponse(char *message, char option1, char option2);
void game_displayHelp(UINT8 x, UINT8 y);
void game_handleUndoMove(void);
void game_splash_screen(void);

#endif 


