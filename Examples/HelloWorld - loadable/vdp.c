#include "mos-interface.h"

// Generic functions
void vdp_mode(unsigned char mode)
{
    putch(22);
    putch(mode);
}

// Text functions
void vdp_cls()
{
    putch(12);
}

void vdp_cursorHome()
{
    putch(30);
}

void vdp_cursorUp()
{
    putch(11);
}

void vdp_cursorGoto(unsigned char x, unsigned char y)
{
    putch(31); // TAB
    putch(x);
    putch(y);
}

void vdp_colour(unsigned char foreground, unsigned char r, unsigned char g, unsigned char b)
{
    putch(17); // COLOUR
    putch(!foreground);
    putch(r);
    putch(g);
    putch(b);
}

void vdp_fgcolour(unsigned char r, unsigned char g, unsigned char b)
{
    vdp_colour(1,r,g,b);
}

void vdp_bgcolour(unsigned char r, unsigned char g, unsigned char b)
{
    vdp_colour(0,r,g,b);
}

//
// Graphics functions
//

void vdp_clearGraphics()
{
    putch(16);    
}

void vdp_plotColour(unsigned char r, unsigned char g, unsigned char b)
{
    putch(18); // GCOL
    putch(0);
    putch(r);
    putch(g);
    putch(b);
}

// internal function
void vdp_plot(unsigned char mode, unsigned int x, unsigned int y)
{
    putch(25); // PLOT
    putch(mode);
    putch(x & 0xFF);
    putch(x >> 8);
    putch(y & 0xFF);
    putch(y >> 8);
}

void vdp_plotMoveTo(unsigned int x, unsigned int y)
{
	vdp_plot(0x04,x,y);
}

void vdp_plotLineTo(unsigned int x, unsigned int y)
{
	vdp_plot(0x05,x,y);
}

void vdp_plotPoint(unsigned int x, unsigned int y)
{
	vdp_plot(0x40,x,y);
}

void vdp_plotTriangle(unsigned int x, unsigned int y)
{
	vdp_plot(0x50,x,y);
}

void vdp_plotCircleRadius(unsigned int r)
{
	vdp_plot(0x90,r,0);
}

void vdp_plotCircleCircumference(unsigned int x, unsigned int y)
{
	vdp_plot(0x95,x,y);
}

void vdp_plotSetOrigin(unsigned int x, unsigned int y)
{
    putch(29); //Graphics ORIGIN
    putch(x & 0xF);
    putch(x >> 8);
    putch(y & 0xF);
    putch(y >> 8);
}

