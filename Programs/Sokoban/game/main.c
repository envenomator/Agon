#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "game.h"

#define FILE_LEVELS	"levels.bin"

int main(int argc, char * argv[]) {
	UINT8 levels;
	INT16 levelnumber = 0;
	BOOL quit;
	BOOL ingame;
	char key;
	
	game_splash_screen();
	
	levels = game_readLevels(FILE_LEVELS);
	
	if(levels)
	{			
		game_sendSpriteData();
		
		while(levelnumber >= 0)
		{
			levelnumber = game_selectLevel(levels, levelnumber); // returns -1 if abort, or valid number between 0-(levels-1)
			if(levelnumber >= 0)
			{
				// Start game
				ingame = TRUE;
				vdp_mode(0);	// 640x480 pixels - more screen estate
				vdp_cursorDisable();

				game_initLevel(levelnumber);	// initialize playing field data from memory or disk
				game_initSprites();				// position sprites in grid, according to level data						
				game_displayLevel();
				while(ingame)
				{
					key = getch();
					switch(key)
					{
						case 'q':
						case 'Q':
						case 27:
							game_resetSprites();
							ingame = (game_getResponse("Really QUIT level (y/n)?",'y','n') != 'y');
							if(ingame)
							{
								game_initSprites();
								game_displayLevel();
							}
							//game_displayLevel();
							break;
						case 'h':
							game_resetSprites();
							vdp_cls();
							game_displayHelp(HELP_XPOS_MODE0, HELP_YPOS_MODE0);
							getch();
							vdp_cls();
							game_initSprites();
							game_displayLevel();
							break;
						case 'u':
							game_handleUndoMove();
							break;
						default:
							ingame = !game_handleKey(key);	// handleKey returns TRUE when game is finished
							if(!ingame)
							{
								levelnumber++;
								if(levelnumber == levels) levelnumber = 0;
								game_resetSprites();
								game_getResponse("Level complete!",0xd,27);
							}
						break;
					}						
				}
			}
		}

		game_resetSprites();
		vdp_mode(1);	// 512x384 pixels - default console
	}
	else
	{
		vdp_cursorGoto(15,38);
		vdp_fgcolour(255,0,0);
		puts("No level available! - press any key");
		getch();
		vdp_cls();
		vdp_fgcolour(255,255,255);
	}
	puts("Thank you for playing Sokoban\r\n");
	return 0;
}

