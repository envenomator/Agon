#include <stdio.h>
#include <ctype.h>
#include "mos-interface.h"
#include "vdp.h"
#include "game.h"

#define FILE_LEVELS	"levels.bin"

void splash_screen()
{
	vdp_cls();
	vdp_cursorGoto(0,10);
	vdp_cursorDisable();
	vdp_fgcolour(255,0,0);
	puts("             _____       _         _                 \r\n");
	vdp_fgcolour(255,255,0);
	puts("            / ____|     | |       | |                \r\n");
	vdp_fgcolour(0,255,255);
	puts("           | (___   ___ | | _____ | |__   __ _ _ __  \r\n");
	vdp_fgcolour(0,0,255);
	puts("            \\___ \\ / _ \\| |/ / _ \\| '_ \\ / _. | '_ \\ \r\n");
	vdp_fgcolour(255,0,255);
	puts("            ____) | (_) |   < (_) | |_) | (_| | | | |\r\n");
	vdp_fgcolour(255,255,255);
	puts("           |_____/ \\___/|_|\\_\\___/|_.__/ \\__,_|_| |_|\r\n");
	puts("\r\n");
	puts("\r\n");
	vdp_fgcolour(0,255,0);
	puts("                           For Agon (TM)\r\n");
	puts("\r\n");
	vdp_fgcolour(128,128,128);
	puts("                     (c) 2023 Jeroen Venema\r\n");
	vdp_cursorGoto(25,38);
	vdp_fgcolour(255,255,255);
	puts("Reading levels...");

}
int main(int argc, char * argv[]) {
	UINT8 levels;
	INT16 levelnumber;
	BOOL doneplaying = FALSE;
	BOOL quit;
	BOOL ingame;
	char key;
	
	splash_screen();
	
	levels = game_readLevels(FILE_LEVELS);
	if(levels)
	{
		vdp_cursorGoto(20,38);
		puts("Press any key to continue ");
		getch();
		
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

