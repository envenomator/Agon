#ifndef VDU_H
#define VDU_H

#include <defines.h>

VOID uart_writeWord(UINT16 w);
VOID uart_writeLong(UINT32 l);
INT16 uart_readWord(VOID);
INT32 uart_readLong(VOID);

VOID bitmap_select(UINT8 n);													// select bitmap
VOID bitmap_sendData_selected(UINT16 width, UINT16 height, UINT32 *data);		// sends data to selected bitmap;
VOID bitmap_sendData(UINT8 id, UINT16 width, UINT16 height, UINT32 *data);		// sends data to bitmap id
VOID bitmap_createSolidColor_selected(UINT16 width, UINT16 height, UINT32 abgr);// create bitmap in solid color
VOID bitmap_createSolidColor(UINT8 id, UINT16 width, UINT16 height, UINT32 abgr);// create bitmap id in solid color
VOID bitmap_draw(UINT8 id, UINT16 x, UINT16 y);									// draws bitmap id to x,y
VOID bitmap_draw_selected(UINT16 x, UINT16 y);									// draws previously selected bitmap to x,y

VOID sprite_activateTotal(UINT8 n);
VOID sprite_clearFrames(UINT8 id);
VOID sprite_clearFrames_selected(VOID);
VOID sprite_select(UINT8 n);
VOID sprite_addFrame_selected(UINT8 bitmapid);
VOID sprite_addFrame(UINT8 id, UINT8 bitmapid);
VOID sprite_nextFrame(VOID);
VOID sprite_previousFrame(VOID);
VOID sprite_setFrame(UINT8 n);
VOID sprite_show(VOID);
VOID sprite_hide(VOID);
VOID sprite_moveTo(UINT16 x, UINT16 y);
VOID sprite_moveBy(INT16 x, INT16 y);

VOID vdu_gotoxy(UINT16 x, UINT16 y);
VOID vdu_lineto(UINT16 x, UINT16 y);
VOID vdu_plotPoint(UINT16 x, UINT16 y);
VOID vdu_cls(VOID);
VOID vdu_gcol(UINT8 mode, UINT8 r, UINT8 g, UINT8 b);
VOID vdu_box(UINT16 topleftx, UINT16 toplefty, UINT16 bottomrightx, UINT16 bottomrighty);
VOID vdu_setmode(UINT8 mode);

#endif 