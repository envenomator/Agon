#include <ez80.h>
#include <defines.h>
#include "mos-interface.h"
#include "vdp.h"

void main(void)
{
	
	UINT8 filehandle;
	UINT8 xpos,ypos;
	
	char c;
	
	
	vdp_cls();
	
	vdp_fgcolour(255,0,0);
	vdp_cursorGoto(10,2);
	xpos = vdp_cursorGetXpos() + 30;
	ypos = vdp_cursorGetYpos() + 30;
	vdp_cursorGoto(xpos,ypos);
	puts("#AgonLight - Hello world!\r\n");
	puts("Dit is een test, om te kijken hoeveel karakters we naar het scherm kunnen schrijven voordat de boel crashed\r\n");
	
	//vdp_cursorGoto(1,3);
	/*
	filehandle = mos_fopen("autoexec.txt",fa_read);
	if(filehandle)
	{
		puts("autoexec.txt contents:");
		vdp_cursorGoto(1,4);
		while(!mos_feof(filehandle))
		{
			putch(mos_fgetc(filehandle));
		}
	}	
	else puts("Couldn't open 'autoexec.txt'");
	mos_fclose(filehandle);
	*/
	//vdp_fgcolour(0,0,255);
	//vdp_bgcolour(0,255,0);
	vdp_cursorGoto(10,20);
	puts("Press any key to return to MOS:");	
	
	getch();
	//vdp_bgcolour(0,0,0);
	//vdp_fgcolour(0,0,0);
	vdp_cls();
	
	return;
}