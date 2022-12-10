#include "game.h"
#include "sokobanprep.h"
#include "vdp.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

UINT32 bitmapbuffer[BITMAPSIZE];	// will hold one bitmap at a time, to transmit to the VDU
UINT8 sprites[MAXHEIGHT][MAXWIDTH]; // will contain all sprites on-screen
struct sokobanlevel currentlevel;	// will contain the currentlevel;
UINT8 spritenumber;					// the current number of sprites in the current level

UINT32 wall_data[1][256] = {
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

UINT32 player_data[1][256] = {
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

UINT32 goal_data[1][256] = {
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

UINT32 box_data[1][256] = {
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

UINT32 boxongoal_data[1][256] = {
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

chartotile lookup[] =
{
	{CHAR_WALL, TILE_WALL},
	{CHAR_PLAYER, TILE_PLAYER},
	{CHAR_PLAYERONGOAL, TILE_PLAYERONGOAL},
	{CHAR_BOX, TILE_BOX},
	{CHAR_BOXONGOAL, TILE_BOXONGOAL},
	{CHAR_GOAL, TILE_GOAL},
	{CHAR_FLOOR, TILE_FLOOR}
};

void game_sendSpriteData(void)
{
	// needs to be called once, to load all bitmaps and sprites
	
	vdp_bitmapSendData(TILE_WALL, BITMAP_WIDTH, BITMAP_HEIGHT, wall_data[0]);
	vdp_bitmapSendData(TILE_PLAYER, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);
	vdp_bitmapSendData(TILE_PLAYERONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);	
	vdp_bitmapSendData(TILE_BOX, BITMAP_WIDTH, BITMAP_HEIGHT, box_data[0]);
	vdp_bitmapSendData(TILE_BOXONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, boxongoal_data[0]);
	vdp_bitmapSendData(TILE_GOAL, BITMAP_WIDTH, BITMAP_HEIGHT, goal_data[0]);
	vdp_bitmapCreateSolidColor(TILE_FLOOR, BITMAP_WIDTH, BITMAP_HEIGHT, 0xff000000);
	
	return;
}

void game_resetSprites(void)
{
	UINT8 n;
	
	// disable all sprites
	for(n = 0; n < spritenumber; n++)
	{
		vdp_spriteSelect(n);
		vdp_spriteHide(n);
		vdp_spriteSetFrame(n,0);
		vdp_spriteClearFramesSelected();
	}
	vdp_spriteActivateTotal(0);
	spritenumber = 0;

	// reset all sprite positions and clear out any sprites
	memset(sprites, 255, MAXHEIGHT*MAXWIDTH);

	vdp_spriteRefresh();
	return;
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
			vdp_spriteSelect(spriteid);
			vdp_spriteMoveBySelected(dx, dy);
		}
		vdp_spriteSelect(0);
		vdp_spriteMoveBySelected(dx, dy);
	}

	// set destination sprite frame
	if(spriteid != NOSPRITE)
	{
		vdp_spriteSelect(spriteid);
		// Check if the sprite moved to a goal or floor
		n2 = currentlevel.data[yn2][xn2];
		switch(n2)
		{
			case CHAR_FLOOR:
				vdp_spriteSetFrameSelected(0);
				break;
			case CHAR_GOAL:
				vdp_spriteSetFrameSelected(1);
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
	
	vdp_spriteRefresh();
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

BOOL game_handleKey(char key)
{
	BOOL done = FALSE;
	BOOL move = FALSE;
	INT16	xn1 = 0,xn2 = 0,yn1 = 0,yn2 = 0;
	
	switch(key)
	{
		case 'a':
			xn1 = currentlevel.xpos - 1;
			yn1 = currentlevel.ypos;
			xn2 = currentlevel.xpos - 2;
			yn2 = currentlevel.ypos;
			move = TRUE;
			break;
		case 'w':
			xn1 = currentlevel.xpos;
			yn1 = currentlevel.ypos - 1;
			xn2 = currentlevel.xpos;
			yn2 = currentlevel.ypos - 2;
			move = TRUE;
			break;
		case 's':
			xn1 = currentlevel.xpos;
			yn1 = currentlevel.ypos + 1;
			xn2 = currentlevel.xpos;
			yn2 = currentlevel.ypos + 2;
			move = TRUE;
			break;
		case 'd':
			xn1 = currentlevel.xpos + 1;
			yn1 = currentlevel.ypos;
			xn2 = currentlevel.xpos + 2;
			yn2 = currentlevel.ypos;
			move = TRUE;
			break;
		default:
			move = FALSE;
			break;
	}
	if(move)
	{
		if(canmove(xn1,yn1,xn2,yn2))
		{
			move_sprites(xn1,yn1,xn2,yn2);
			move_updatelevel(xn1,yn1,xn2,yn2);
			done = (currentlevel.goals == currentlevel.goalstaken);
		}
	}
	return done;
}

char game_getResponse(char *message, char option1, char option2)
{
	UINT8 n;
	UINT8 len = strlen(message);
	UINT8 start = (80 - len) / 2;
	char ret = 0;
	
	
	// vertical spacing
	for(n = start - 1; n < (start+len+1); n++)
	{
		vdp_cursorGoto(n,27);
		putch(' ');
		vdp_cursorGoto(n,29);
		putch(' ');
	}
	// horizontal spacing
	vdp_cursorGoto(start-1,28);
	putch(' ');
	printf("%s ", message);

	while((ret != option1) && (ret != option2)) ret = getch();
	
	vdp_cls();

	return ret;
}


INT16 game_selectLevel(UINT8 levels)
{
	static INT16 lvl = -1;
	
	lvl++;
	if(lvl == levels) lvl = -1;
	return lvl;
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
					vdp_bitmapDraw(TILE_WALL, x, y);
					break;
				case CHAR_PLAYER:
				case CHAR_PLAYERONGOAL:
					vdp_spriteSelect(0);
					vdp_spriteMoveToSelected(x,y);
					vdp_spriteShowSelected();
					break;
				case CHAR_BOX:
				case CHAR_BOXONGOAL:
					vdp_spriteSelect(sprites[height][width]);
					vdp_spriteMoveToSelected(x,y);
					vdp_spriteShowSelected();
					if(c == CHAR_BOXONGOAL)
					{
						vdp_bitmapDraw(TILE_GOAL, x, y); // don't forget to draw the goal beneath
					}
					break;
				case CHAR_GOAL:
					vdp_bitmapDraw(TILE_GOAL, x, y);					
					break;
				case CHAR_FLOOR:
					vdp_bitmapDraw(TILE_FLOOR, x, y);			
					break;
				default:
					break;
			}
			x += BITMAP_WIDTH;
		}
		y += BITMAP_HEIGHT;
	}

	vdp_spriteRefresh();
}


UINT8 game_readLevels(char *filename)
{
	// Reads all the levels in the standard file into memory and returns the number of levels
	UINT8 numlevels = 0;
	UINT16 size;
	UINT8 file;
	char* ptr = (char*)LEVELDATA;
	
	file = mos_fopen(filename, fa_read);
	if(file)
	{
		numlevels = 0;
		
		while(!mos_feof(file))
		{
			size = sizeof(struct sokobanlevel);
			while(size)
			{
				*ptr = mos_fgetc(file);
				ptr++;
				size--;
			}
			numlevels++;
		}		
		mos_fclose(file);
	}
	return numlevels;
}	


void game_initLevel(UINT8 levelid)
{
	memcpy(&currentlevel, (void*)(LEVELDATA+(sizeof(struct sokobanlevel))*levelid), sizeof(struct sokobanlevel));
}

void game_initSprites(void)
{
	UINT8 x,y;
	UINT8 boxsprite = 1;
	
	// Now build up the current level
	// Player sprite
	vdp_spriteSelect(0);
	vdp_spriteClearFramesSelected();
	vdp_spriteAddFrameSelected(TILE_PLAYER);
	vdp_spriteHideSelected();

	// set sprite positions
	for(y = 0; y < currentlevel.height; y++)
	{
		for(x = 0; x < currentlevel.width; x++)
		{
			switch(currentlevel.data[y][x])
			{
				case CHAR_BOX:
				case CHAR_BOXONGOAL:
					vdp_spriteSelect(boxsprite);
					vdp_spriteClearFramesSelected();
					vdp_spriteAddFrameSelected(TILE_BOX);
					vdp_spriteAddFrameSelected(TILE_BOXONGOAL);
					if(currentlevel.data[y][x] == CHAR_BOXONGOAL) vdp_spriteSetFrameSelected(1);
					else vdp_spriteSetFrameSelected(0);
					vdp_spriteHideSelected();
					sprites[y][x] = boxsprite;
					boxsprite++;
					break;
				default:
					sprites[y][x] = NOSPRITE;
					break;
			}	
		}
	}

	// activate all sprites. boxsprite is allready set to one extra, we use that extra to account for the player sprite
	spritenumber = boxsprite;
	vdp_spriteActivateTotal(spritenumber);
}
	