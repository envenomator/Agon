/*
 * Title:			AGON VDU
 * Author:			Jeroen Venema
 * Created:			23/07/2022
 * Last Updated:	23/07/2022
 *
 * Modinfo:
 * 
 */

#include "vdu.h"
#include "uart.h"
#include "timer.h"

VOID uart_writeWord(UINT16 w)
{
	putch(w & 0xFF); // write LSB
	putch(w >> 8);	 // write MSB
}

VOID uart_writeLong(UINT32 l)
{
	UINT32 temp = l;
	
	putch(temp & 0xFF); // write LSB
	temp = temp >> 8;
	putch(temp & 0xFF);
	temp = temp >> 8;
	putch(temp & 0xFF);
	temp = temp >> 8;
	putch(temp & 0xFF);
	return;
}

INT16 uart_readWord(VOID)
{
	UINT16 w = 0;
	
	w |= getch();
	w |= (getch() << 8);

	return w;
}

INT32 uart_readLong(VOID)
{
	UINT32 lsw,msw;
	UINT32 r;
	
	lsw = uart_readWord();
	msw = uart_readWord();
	
	r = lsw | msw << 16;
	return r;
}


VOID bitmap_select(UINT8 id)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(0); // select command
	putch(id);  // bitmap_id
	return;
}

VOID bitmap_sendData_selected(UINT16 width, UINT16 height, UINT32 *data)
{
	UINT16 n;
	
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(1); // send data to selected bitmap
	
	uart_writeWord(width);
	uart_writeWord(height);
	
	for(n = 0; n < (width*height); n++)
	{
		uart_writeLong(data[n]);
		delayms(1);
	}
	return;	
}

VOID bitmap_sendData(UINT8 id, UINT16 width, UINT16 height, UINT32 *data)
{
	bitmap_select(id);
	bitmap_sendData_selected(width, height, data);
	return;
}

VOID bitmap_draw_selected(UINT16 x, UINT16 y)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(3); // draw selected bitmap
	
	uart_writeWord(x);
	uart_writeWord(y);
	
	return;
}

VOID bitmap_draw(UINT8 id, UINT16 x, UINT16 y)
{
	bitmap_select(id);
	bitmap_draw_selected(x,y);
	return;
}

VOID bitmap_createSolidColor_selected(UINT16 width, UINT16 height, UINT32 abgr)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(2); // define in single color command
	
	uart_writeWord(width);
	uart_writeWord(height);
	uart_writeLong(abgr);
	return;	
}

VOID bitmap_createSolidColor(UINT8 id, UINT16 width, UINT16 height, UINT32 abgr)
{
	bitmap_select(id);
	bitmap_createSolidColor_selected(width, height, abgr);
	return;
}

VOID sprite_select(UINT8 n)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(4); // select sprite
	putch(n);
	return;		
}

VOID sprite_clearFrames_selected(VOID)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(5); // clear frames
	return;			
}

VOID sprite_clearFrames(UINT8 id)
{
	sprite_select(id);
	sprite_clearFrames_selected();
	return;			
}

VOID sprite_addFrame_selected(UINT8 bitmapid)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(6); // add frame
	putch(bitmapid);
	return;
}

VOID sprite_addFrame(UINT8 id, UINT8 bitmapid)
{
	sprite_select(id);
	sprite_addFrame_selected(bitmapid);
	return;
}

VOID sprite_activateTotal(UINT8 n)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(7); // set number of sprites
	putch(n);
	return;
}

VOID sprite_nextFrame(VOID)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(8); // next frame
	return;	
}

VOID sprite_previousFrame(VOID)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(9); // previous frame
	return;

}

VOID sprite_setFrame(UINT8 n)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(10); // set current frame
	putch(n);
	return;
}

VOID sprite_show(VOID)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(11); // show sprite
	return;		
}

VOID sprite_hide(VOID)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(12); // hide sprite
	return;	
}

VOID sprite_moveTo(UINT16 x, UINT16 y)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(13); // move to
	uart_writeWord(x);
	uart_writeWord(y);
	return;
}

VOID sprite_moveBy(INT16 x, INT16 y)
{
	putch(23); // vdu_sys
	putch(27); // sprite command
	putch(14); // move by
	uart_writeWord(x);
	uart_writeWord(y);
	return;
}

VOID vdu_gotoxy(UINT16 x, UINT16 y)
{
	putch(25);
	putch(4);
	uart_writeWord(x);
	uart_writeWord(y);
	return;
}

VOID vdu_lineto(UINT16 x, UINT16 y)
{
	putch(25);
	putch(5);
	uart_writeWord(x);
	uart_writeWord(y);
	return;
}

VOID vdu_plotPoint(UINT16 x, UINT16 y)
{
	putch(25);
	putch(0x40);
	uart_writeWord(x);
	uart_writeWord(y);
	return;	
}

VOID vdu_cls(VOID)
{
	putch(0x0C);
	return;
}

VOID vdu_gcol(UINT8 mode, UINT8 r, UINT8 g, UINT8 b)
{
	// ignore mode for now
	putch(0x12);
	putch(mode);
	putch(r);
	putch(g);
	putch(b);
	return;
}

VOID vdu_box(UINT16 topleftx, UINT16 toplefty, UINT16 bottomrightx, UINT16 bottomrighty)
{
	vdu_gotoxy(topleftx, toplefty);
	vdu_lineto(bottomrightx, toplefty);
	vdu_lineto(bottomrightx, bottomrighty);
	vdu_lineto(topleftx, bottomrighty);
	vdu_lineto(topleftx, toplefty);
	return;
}