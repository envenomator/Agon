/*
 * Title:			AGON VDP - VDP c header interface
 * Author:			Jeroen Venema
 * Created:			20/10/2022
 * Last Updated:	20/10/2022
 * 
 * Modinfo:
 * 20/10/2022:		Initial version: toDo - sprite functions next
 */
 
#ifndef VDP_H
#define VDP_H

// Generic functions
void vdp_mode(unsigned char mode);

// Text VDU functions
void vdp_cls();
void vdp_cursorHome();
void vdp_cursorUp();
void vdp_cursorGoto(unsigned char x, unsigned char y);
void vdp_fgcolour(unsigned char r, unsigned char g, unsigned char b);
void vdp_bgcolour(unsigned char r, unsigned char g, unsigned char b);

// Graphic VDU functions
void vdp_clearGraphics();
void vdp_plotColour(unsigned char r, unsigned char g, unsigned char b);
void vdp_plotSetOrigin(unsigned int x, unsigned int y);
void vdp_plotMoveTo(unsigned int x, unsigned int y);
void vdp_plotLineTo(unsigned int x, unsigned int y);
void vdp_plotPoint(unsigned int x, unsigned int y);
void vdp_plotTriangle(unsigned int x, unsigned int y);
void vdp_plotCircleRadius(unsigned int r);
void vdp_plotCircleCircumference(unsigned int x, unsigned int y);

#endif VDP_H