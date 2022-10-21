#include <ez80.h>
#include "stdint.h"
#include "mos-interface.h"
#include "vdp.h"

void puts_slow(char *str)
{
	uint16_t x;
	
	while(*str)
	{
		x = 32000;
		putch(*str);
		str++;
		while(x--);
	}
}

void puts(char *str)
{
	while(*str)
	{
		putch(*str);
		str++;
	}
}

void main(void)
{
	char c;
	unsigned int y;
	
	vdp_cls();
	vdp_fgcolour(255,0,0);
	vdp_cursorGoto(10,2);
	puts("#AgonLight - Hello world!\r\n");
	
	vdp_fgcolour(0,0,255);
	vdp_bgcolour(0,255,0);
	vdp_cursorGoto(10,4);
	puts("Press any key");
	
	
	getch();

	vdp_mode(0);
		
	vdp_plotSetOrigin(100,100);
	vdp_plotMoveTo(100,100);
	vdp_plotLineTo(200,200);
	vdp_plotColour(0,255,0);
	vdp_plotCircleRadius(100);
	
	vdp_plotMoveTo(300,300);
	vdp_plotColour(255,0,0);
	vdp_plotCircleCircumference(350,350);
	
	vdp_plotColour(0,0,255);
	vdp_plotMoveTo(400,100);
	vdp_plotTriangle(400,100);
	vdp_plotTriangle(400,400);
	vdp_plotTriangle(600,400);
	
	vdp_plotColour(255,255,0);
	for(y = 0; y < 480; y+=5) vdp_plotPoint(50,y);
	getch();

	vdp_fgcolour(255,255,255);
	vdp_bgcolour(0,0,0);
	vdp_mode(1);
	vdp_cls();
}