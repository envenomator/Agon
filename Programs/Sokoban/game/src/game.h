#ifndef GAME_H
#define GAME_H

#include <defines.h>
#include "sokobanprep.h"

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

void print_playfieldText(struct sokobanlevel *level);
UINT16 read_levels(char *filename, struct sokobanlevel **levelbuffer);

#endif 


