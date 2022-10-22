/*
 * Title:			AGON VDP - VDP c header interface
 * Author:			Jeroen Venema
 * Created:			20/10/2022
 * Last Updated:	20/10/2022
 * 
 * Modinfo:
 * 20/10/2022:		Initial version: text/graphics functions
 * 22/10/2022:		Bitmap/Sprite functions added
 */

#include <defines.h>

#ifndef VDP_H
#define VDP_H

// Generic
void vdp_mode(unsigned char mode);

// Text VDP functions
void vdp_cls();
void vdp_cursorHome();
void vdp_cursorUp();
void vdp_cursorGoto(unsigned char x, unsigned char y);
void vdp_fgcolour(unsigned char r, unsigned char g, unsigned char b);
void vdp_bgcolour(unsigned char r, unsigned char g, unsigned char b);

// Graphic VDP functions
void vdp_clearGraphics();
void vdp_plotColour(unsigned char r, unsigned char g, unsigned char b);
void vdp_plotSetOrigin(unsigned int x, unsigned int y);
void vdp_plotMoveTo(unsigned int x, unsigned int y);
void vdp_plotLineTo(unsigned int x, unsigned int y);
void vdp_plotPoint(unsigned int x, unsigned int y);
void vdp_plotTriangle(unsigned int x, unsigned int y);
void vdp_plotCircleRadius(unsigned int r);
void vdp_plotCircleCircumference(unsigned int x, unsigned int y);

// Bitmap VDP functions
void vdp_bitmapSelect(UINT8 id);
void vdp_bitmapSendDataSelected(UINT16 width, UINT16 height, UINT32 *data);
void vdp_bitmapSendData(UINT8 id, UINT16 width, UINT16 height, UINT32 *data);
void vdp_bitmapDrawSelected(UINT16 x, UINT16 y);
void vdp_bitmapDraw(UINT8 id, UINT16 x, UINT16 y);
void vdp_bitmapCreateSolidColorSelected(UINT16 width, UINT16 height, UINT32 abgr);
void vdp_bitmapCreateSolidColor(UINT8 id, UINT16 width, UINT16 height, UINT32 abgr);

// Sprite VDP functions
void vdp_spriteSelect(UINT8 id);
void vdp_spriteClearFramesSelected(void);
void vdp_spriteClearFrames(UINT8 bitmapid);
void vdp_spriteAddFrameSelected(UINT8 bitmapid);
void vdp_spriteAddFrame(UINT8 id, UINT8 bitmapid);
void vdp_spriteNextFrameSelected(void);
void vdp_spriteNextFrame(UINT8 id);
void vdp_spritePreviousFrameSelected(void);
void vdp_spritePreviousFrame(UINT8 id);
void vdp_spriteSetFrameSelected(UINT8 framenumber);
void vdp_spriteSetFrame(UINT8 id, UINT8 framenumber);
void vdp_spriteShowSelected(void);
void vdp_spriteShow(UINT8 id);
void vdp_spriteHideSelected(void);
void vdp_spriteHide(UINT8 id);
void vdp_spriteMoveToSelected(UINT16 x, UINT16 y);
void vdp_spriteMoveTo(UINT8 id, UINT16 x, UINT16 y);
void vdp_spriteMoveBySelected(UINT16 x, UINT16 y);
void vdp_spriteMoveBy(UINT8 id, UINT16 x, UINT16 y);
void vdp_spriteActivateTotal(UINT8 number);
void vdp_spriteRefresh(void);

#endif VDP_H
