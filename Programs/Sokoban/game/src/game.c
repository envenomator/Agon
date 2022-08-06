#include "game.h"
#include "sokobanprep.h"
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>
#include <CTYPE.H>
#include "ff.h"
#include "mos.h"
#include "vdu.h"
#include "String.h"
#include "timer.h"
#include "uart.h"

#define SENDDELAY	2
#define FRAMEDELAY  15

UINT32 bitmapbuffer[BITMAPSIZE];				// will hold one bitmap at a time, to transmit to the VDU
UINT8 sprites[MAXHEIGHT][MAXWIDTH]; // will contain all sprites on-screen
struct sokobanlevel currentlevel;	// will contain the currentlevel;
UINT8 spritenumber;

static const UINT32 wall_data[1][256] = {
{
0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 
0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 
0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff404040, 0xff008080, 0xff000080, 0xff808080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 
0xff000080, 0xff000080, 0xff808080, 0xff808080, 0xff000080, 0xff000080, 0xff808080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 
0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 
0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff808080, 0xff000080, 
0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff404040, 0xff008080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 
0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 
0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 
0xff404040, 0xff008080, 0xff008080, 0xff000080, 0xff808080, 0xff808080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff808080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 0xff000080, 0xff808080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff808080, 
0xff404040, 0xff008080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 0xff808080, 
0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040
}
};

static const UINT32 player_data[1][256] = {
{
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff404040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff404040, 0xff000000, 0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff404040, 0xff000000, 0xff404040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff404040, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff404040, 0xff808080, 0xff000000, 0xff000000, 0xff008080, 0xff000000, 0xff000000, 0xff808080, 0xff404040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff808080, 0xff808080, 0xff808080, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff000080, 0xff000000, 0xff000000, 0xff000000, 0xff000080, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff000080, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff000080, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff000000, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff404040, 0xff000080, 0xff0000ff, 0xff000080, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff000080, 0xff0000ff, 0xff000080, 0xff404040, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0xff000000, 0xff008080, 0xff008080, 0xff000080, 0xff000080, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff000080, 0xff000080, 0xff008080, 0xff008080, 0xff000000, 0x00000000, 0x00000000, 
0x00000000, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff800000, 0xff800000, 0xff800000, 0xff800000, 0xff800000, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0xff000000, 0xff000000, 0xff800000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xff800000, 0xff000000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff800000, 0xffff0000, 0xffff0000, 0xff800000, 0xffff0000, 0xffff0000, 0xff800000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xffff0000, 0xffff0000, 0xff800000, 0xff000000, 0xffff0000, 0xffff0000, 0xffff0000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
}
};

static const UINT32 goal_data[1][256] = {
{
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0xff00ff00, 0xff00ff00, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0xff00ff00, 0xff00ff00, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0xff008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
}
};

static const UINT32 box_data[1][256] = {
{
0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 
0xff00ffff, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff00ffff, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080
}
};

static const UINT32 boxongoal_data[1][256] = {
{
0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 
0xff00ffff, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff008000, 0xff008000, 0xff008000, 0xff008000, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ff00, 0xff00ff00, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff00ff00, 0xff000000, 0xff008080, 0xff00ff00, 0xff00ff00, 0xff008080, 0xff000000, 0xff00ff00, 0xff008000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff00ff00, 0xff000000, 0xff00ffff, 0xff00ff00, 0xff00ff00, 0xff00ffff, 0xff000000, 0xff00ff00, 0xff008000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 0xff008000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ff00, 0xff00ff00, 0xff000000, 0xff008080, 0xff00ffff, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff008000, 0xff008000, 0xff008000, 0xff008000, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 
0xff00ffff, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080
}
};

const chartotile lookup[] =
{
	{CHAR_WALL, TILE_WALL},
	{CHAR_PLAYER, TILE_PLAYER},
	{CHAR_PLAYERONGOAL, TILE_PLAYERONGOAL},
	{CHAR_BOX, TILE_BOX},
	{CHAR_BOXONGOAL, TILE_BOXONGOAL},
	{CHAR_GOAL, TILE_GOAL},
	{CHAR_FLOOR, TILE_FLOOR}
};

BOOL game_init(void)
{
	// needs to be called once, to load all bitmaps and sprites
	FIL	   	fil;
	UINT   	br;	
	FSIZE_t fSize;
	UINT16	levels;
	FRESULT	fr;
	UINT8	n;
	
	/*
	// load all bitmaps and send them to the VDU
	fr = f_open(&fil, FILENAME_BITMAPS, FA_READ);
	if(fr == FR_OK) {
		fSize = f_size(&fil);
		if(fSize != BITMAPNUMBER * BITMAPSIZE)
		{
			f_close(&fil);
			return FALSE;
		}
		for(n = 0; n < BITMAPNUMBER; n++)
		{
			fr = f_read(&fil, bitmapbuffer, BITMAPSIZE*4, &br);
			bitmap_select(n);
			bitmap_sendData_selected(BITMAP_WIDTH, BITMAP_HEIGHT, bitmapbuffer);
		}
	}
	else {
		mos_fileError(fr);
		return FALSE;
	}
	f_close(&fil);
	*/
	
	bitmap_sendData(TILE_WALL, BITMAP_WIDTH, BITMAP_HEIGHT, wall_data[0]);
	bitmap_sendData(TILE_PLAYER, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);
	bitmap_sendData(TILE_PLAYERONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);	
	bitmap_sendData(TILE_BOX, BITMAP_WIDTH, BITMAP_HEIGHT, box_data[0]);
	bitmap_sendData(TILE_BOXONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, boxongoal_data[0]);
	bitmap_sendData(TILE_GOAL, BITMAP_WIDTH, BITMAP_HEIGHT, goal_data[0]);
	bitmap_createSolidColor(TILE_FLOOR, BITMAP_WIDTH, BITMAP_HEIGHT, 0xff000000);
	
	return TRUE;
	
}
void game_resetlevel(void)
{
	UINT8 x,y;
	UINT8 spriteid;
	UINT8 n;
	
	// disable all sprites
	for(n = 0; n < spritenumber; n++)
	{
		sprite_select(n);
		sprite_hide();
		sprite_setFrame(0);
		sprite_clearFrames_selected();
		delayms(SENDDELAY);
	}
	sprite_activateTotal(0);
	spritenumber = 0;

	// reset current level, everything zeroed out
	memset(&currentlevel, 0, sizeof(struct sokobanlevel));
	
	// reset all sprite positions and clear out any sprites
	memset(sprites, 255, MAXHEIGHT*MAXWIDTH);
}

BOOL canmove(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2)
{
	BOOL can = FALSE;
	UINT8 n1, n2;
	
	n1 = currentlevel.data[yn1][xn1];
	n2 = currentlevel.data[yn2][xn2];
	
	switch(n1)
	{
		case CHAR_WALL:
			return FALSE;
			break;
		case CHAR_FLOOR:
		case CHAR_GOAL:
			return TRUE;
			break;
	}
	// either BOX or BOXONGOAL next to player
	switch(n2)
	{
		case CHAR_WALL:
			return FALSE;
			break;
		case CHAR_BOX:
		case CHAR_BOXONGOAL:
			return FALSE;
			break;
	}
	// only FLOOR or empty GOAL remaining at n2
	return TRUE;
}

void move_sprites(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2)
{
	// move can happen, no need to check again
	UINT8 spriteid = sprites[yn1][xn1];
	UINT8 n;
	INT16 dx, dy;
	UINT8 n2;
		
	dx = xn2 - xn1;
	dy = yn2 - yn1;
	
	for(n = 0; n < BITMAP_WIDTH; n++)
	{
		if(spriteid != NOSPRITE)
		{
			sprite_select(spriteid);
			sprite_moveBy(dx, dy);
		}
		sprite_select(0);
		sprite_moveBy(dx, dy);
		delayms(FRAMEDELAY);
	}

	// set destination sprite frame
	if(spriteid != NOSPRITE)
	{
		sprite_select(spriteid);
		// Check if the sprite moved to a goal or floor
		n2 = currentlevel.data[yn2][xn2];
		switch(n2)
		{
			case CHAR_FLOOR:
				sprite_setFrame(0);
				break;
			case CHAR_GOAL:
				sprite_setFrame(1);
				break;
		}
	}
	// update sprite number matrix
	if(spriteid != NOSPRITE)
	{
		// player shoves a box here
		sprites[yn2][xn2] = sprites[yn1][xn1];
	}
	sprites[yn1][xn1] = NOSPRITE; // player's sprite isn't handled by using a box spriteid
	sprites[currentlevel.ypos][currentlevel.xpos] = NOSPRITE;
}

void move_updatelevel(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2)
{
	// move can happen, no need to check again
	UINT8 n1, n2;
	BOOL onlyplayermoves;
	
	n1 = currentlevel.data[yn1][xn1];
	n2 = currentlevel.data[yn2][xn2];

	
	switch(n1)
	{
		case CHAR_FLOOR:
			onlyplayermoves = TRUE;
			currentlevel.data[yn1][xn1] = CHAR_PLAYER;
			break;
		case CHAR_GOAL:
			onlyplayermoves = TRUE;
			currentlevel.data[yn1][xn1] = CHAR_PLAYERONGOAL;
			break;
		default:
			onlyplayermoves = FALSE;
			break;
	}
	if(!onlyplayermoves)
	{
		switch(n2)
		{
			case CHAR_FLOOR:
				currentlevel.data[yn2][xn2] = CHAR_BOX;
				break;
			case CHAR_GOAL:
				currentlevel.data[yn2][xn2] = CHAR_BOXONGOAL;
				currentlevel.goalstaken++;
				break;
			default:
				break; // ignore the rest
		}
		switch(n1)
		{
			case CHAR_BOX:
				currentlevel.data[yn1][xn1] = CHAR_PLAYER;				
				break;
			case CHAR_BOXONGOAL:
				currentlevel.data[yn1][xn1] = CHAR_PLAYERONGOAL;				
				currentlevel.goalstaken--;
				break;
			case CHAR_GOAL:
				currentlevel.data[yn1][xn1] = CHAR_PLAYERONGOAL;
				break;
		}
	}
	
	
	// check where the player was standing on
	if(currentlevel.data[currentlevel.ypos][currentlevel.xpos] == CHAR_PLAYERONGOAL) currentlevel.data[currentlevel.ypos][currentlevel.xpos] = CHAR_GOAL;
	else currentlevel.data[currentlevel.ypos][currentlevel.xpos] = CHAR_FLOOR;
	
	// update player position
	currentlevel.xpos = xn1;
	currentlevel.ypos = yn1;
}

void game_play()
{
	BOOL done = FALSE, validmove;
	char key;
	INT16	xn1 = 0,xn2 = 0,yn1 = 0,yn2 = 0;
	
	while(!done)
	{
		//debug_print_playfieldText();
		validmove = TRUE;
		key = getch();
		switch(key)
		{
			case 'a':
				xn1 = currentlevel.xpos - 1;
				yn1 = currentlevel.ypos;
				xn2 = currentlevel.xpos - 2;
				yn2 = currentlevel.ypos;
				break;
			case 'w':
				xn1 = currentlevel.xpos;
				yn1 = currentlevel.ypos - 1;
				xn2 = currentlevel.xpos;
				yn2 = currentlevel.ypos - 2;
				break;
			case 's':
				xn1 = currentlevel.xpos;
				yn1 = currentlevel.ypos + 1;
				xn2 = currentlevel.xpos;
				yn2 = currentlevel.ypos + 2;
				break;
			case 'd':
				xn1 = currentlevel.xpos + 1;
				yn1 = currentlevel.ypos;
				xn2 = currentlevel.xpos + 2;
				yn2 = currentlevel.ypos;
				break;
			default:
				validmove = FALSE;
				break;
		}
		//printf("xn1:%d yn1:%d xn2:%d yn2:%d\n\r",xn1,yn1,xn2,yn2);
		if(validmove)
		{
			if(canmove(xn1,yn1,xn2,yn2))
			{
				move_sprites(xn1,yn1,xn2,yn2);
				move_updatelevel(xn1,yn1,xn2,yn2);
			}
		}
		done = (currentlevel.goals == currentlevel.goalstaken);
	}
}


void debug_print_playfieldText(void)
{
	UINT16 width, height;
	char c;
	
	for(height = 0; height < currentlevel.height; height++)
	{
		for(width = 0; width < currentlevel.width; width++)
		{
			c = currentlevel.data[height][width];
			printf("%c",c?c:' ');
		}
		printf(" ");
		for(width = 0; width < currentlevel.width; width++)
		{
			c = sprites[height][width] + '0';
			if(c > '9') c = 'X';
			printf("%c",c);
		}
		printf("\n\r");
	}
	
}

void game_displayLevel(void)
{
	UINT16 width, height;
	UINT16 ystart,xstart,x,y;
	char c;
	
	// calculate on-screen base coordinates
	xstart = ((MAXWIDTH - currentlevel.width) / 2) * BITMAP_WIDTH;
	ystart = ((MAXHEIGHT - currentlevel.height) / 2) *BITMAP_HEIGHT;
	
	y = ystart;
	for(height = 0; height < currentlevel.height; height++)
	{
		x = xstart;
		for(width = 0; width < currentlevel.width; width++)
		{
			c = currentlevel.data[height][width];
			switch(c)
			{
				case CHAR_WALL:
					bitmap_draw(TILE_WALL, x, y);
					break;
				case CHAR_PLAYER:
				case CHAR_PLAYERONGOAL:
					sprite_select(0);
					sprite_moveTo(x,y);
					sprite_show();
					break;
				case CHAR_BOX:
				case CHAR_BOXONGOAL:
					sprite_select(sprites[height][width]);
					sprite_moveTo(x,y);
					sprite_show();
					if(c == CHAR_BOXONGOAL)
					{
						bitmap_draw(TILE_GOAL, x, y); // don't forget to draw the goal beneath
						delayms(SENDDELAY);
					}
					break;
				case CHAR_GOAL:
					bitmap_draw(TILE_GOAL, x, y);					
					break;
				case CHAR_FLOOR:
					bitmap_draw(TILE_FLOOR, x, y);			
					break;
				default:
					break;
			}
			delayms(SENDDELAY);
			x += BITMAP_WIDTH;
		}
		y += BITMAP_HEIGHT;
	}	
}


UINT16 read_numberoflevels(void)
{
	// returns the number of levels in the standard file
	FIL	   	fil;
	UINT   	br;	
	FSIZE_t fSize;
	UINT16	levels;
	FRESULT	fr;

	fr = f_open(&fil, FILENAME_LEVELS, FA_READ);
	if(fr == FR_OK) {
		fSize = f_size(&fil);
		levels = fSize / sizeof(struct sokobanlevel);
	}
	else {
		mos_fileError(fr);
		return 0;
	}
	f_close(&fil);
	return levels;	
}

BOOL read_level(UINT8 levelid)
{
	FIL	   	fil;
	UINT   	br;	
	FRESULT	fr;
	BOOL	done = FALSE;
	
	fr = f_open(&fil, FILENAME_LEVELS, FA_READ);
	if(fr == FR_OK) {
		fr = f_lseek(&fil, levelid*sizeof(struct sokobanlevel));
		if(fr == FR_OK)
		{
			fr = f_read(&fil, &currentlevel, sizeof(struct sokobanlevel), &br);
			if(fr != FR_OK)
			{
				mos_fileError(fr);
				return FALSE;
			}
		}
		else
		{
			mos_fileError(fr);
			return FALSE;
		}
	}
	else {
		mos_fileError(fr);
		return FALSE;
	}
	f_close(&fil);
	return TRUE;	
}

void game_createSprites(void)
{
	UINT8 x,y;
	UINT8 boxsprite = 1;
	
	sprite_activateTotal(0);
	
	// Player sprite
	sprite_select(0);
	sprite_clearFrames_selected();
	sprite_addFrame_selected(TILE_PLAYER);
	sprite_hide();

	// set sprite positions
	for(y = 0; y < currentlevel.height; y++)
	{
		for(x = 0; x < currentlevel.width; x++)
		{
			switch(currentlevel.data[y][x])
			{
				case CHAR_BOX:
				case CHAR_BOXONGOAL:
					sprite_select(boxsprite);
					sprite_clearFrames_selected();
					sprite_addFrame_selected(TILE_BOX);
					sprite_addFrame_selected(TILE_BOXONGOAL);
					if(currentlevel.data[y][x] == CHAR_BOXONGOAL) sprite_setFrame(1);
					sprite_hide();
					sprites[y][x] = boxsprite;
					//printf("X:%d, Y:%d, box id: %d\n\r",x,y,sprites[y][x]);
					boxsprite++;
					delayms(SENDDELAY);
					break;
				default:
					sprites[y][x] = NOSPRITE;
					break;
			}	
		}
	}

	// activate all sprites. boxsprite is allready set to one extra, we use that extra to account for the player sprite
	sprite_activateTotal(boxsprite);
	//printf(" %d sprites activated\n\r",boxsprite);
	
	spritenumber = boxsprite;
}
	