#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "game.h"

#define FILE_LEVELS	"levels.bin"

int main(int argc, char * argv[]) {
	UINT8 levels;
	INT16 levelnumber;
	BOOL doneplaying = FALSE;
	BOOL quit;
	BOOL ingame;
	char key;
	
	printf("Reading levels.bin\r\n");
	levels = game_readLevels(FILE_LEVELS);
	if(levels)
	{
		vdp_mode(0);	// 640x480 pixels
		vdp_cursorDisable();
		
		game_sendSpriteData();
		
		while(!doneplaying)
		{
			levelnumber = game_selectLevel(levels); // returns -1 if abort, or valid number between 0-(levels-1)
			if(levelnumber >= 0)
			{
				// Start game
				ingame = TRUE;
				game_resetSprites();			// clear out any onscreen sprites
				game_initLevel(levelnumber);	// initialize playing field data from memory or disk
				game_initSprites();				// position sprites in grid, according to level data
				
				vdp_cls();	// clear out any text
				
				game_displayLevel();
				while(ingame)
				{
					key = getch();
					if((key == 'q') || (key == 'Q') || (key == 27)) // quit pressed
					{
						game_resetSprites();
						ingame = (game_getResponse("Really QUIT level (y/n)?",'y','n') != 'y');
						if(ingame)
						{
							game_initSprites();
							game_displayLevel();
						}
						game_displayLevel();
					}
					else ingame = !game_handleKey(key);	// handleKey returns TRUE when game is finished
				}
			}
			else doneplaying = TRUE;
		}

		game_resetSprites();
		vdp_mode(1);	// 512x384 pixels - default console
	}
	else printf("No level available\r\n");
	return 0;
}

