#include "game.h"
#include "sokobanprep.h"
#include "vdp.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

struct undoitem undomove[UNDOBUFFERSIZE];
UINT8 undo_head;
UINT8 num_undomoves;

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

UINT32 boxmini_data[1][64] = {
{
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 
0xff000000, 0xff00ffff, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff008080, 0xff000000, 0xff000000, 0xff008080, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff000000, 
0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000
}
};

UINT32 wallmini_data[1][64] = {
{
0xff008080, 0xff008080, 0xff008080, 0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 
0xff000080, 0xff000080, 0xff000080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff808080, 
0xff000080, 0xff000080, 0xff000080, 0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 
0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 
0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 0xff008080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff808080, 0xff000080, 0xff000080, 0xff000080, 
0xff404040, 0xff008080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 0xff000080, 
0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040
}
};

UINT32 goalmini_data[1][64] = {
{
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff00ff00, 0xff000000, 0xff000000, 0xff00ff00, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff00ff00, 0xff00ff00, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff00ff00, 0xff00ff00, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff00ff00, 0xff000000, 0xff000000, 0xff00ff00, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000
}
};

UINT32 boxongoalmini_data[1][64] = {
{
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 
0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 
0xff000000, 0xff00ffff, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff008080, 0xff00ff00, 0xff00ff00, 0xff008080, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff008080, 0xff00ff00, 0xff00ff00, 0xff008080, 0xff00ffff, 0xff000000, 
0xff000000, 0xff00ffff, 0xff000000, 0xff008080, 0xff008080, 0xff000000, 0xff00ffff, 0xff000000, 
0xff000000, 0xff008080, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff00ffff, 0xff008080, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000
}
};

UINT32 playermini_data[1][64] = {
{
0xff000000, 0xff000000, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff404040, 0xff000000, 
0xff000000, 0xff000000, 0xff404040, 0xff008080, 0xff008080, 0xff008080, 0xff404040, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff008080, 0xff008080, 0xff008080, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff404040, 0xff000080, 0xff404040, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff008080, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff008080, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xff000000, 0xffff0000, 0xff800000, 0xffff0000, 0xff000000, 0xff000000, 
0xff000000, 0xff000000, 0xffff0000, 0xffff0000, 0xff000000, 0xffff0000, 0xffff0000, 0xff000000
}
};

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

void game_sendSpriteData(void)
{
	// needs to be called once, to load all bitmaps and sprites to the VDU
	
	vdp_bitmapSendData(TILE_WALL, BITMAP_WIDTH, BITMAP_HEIGHT, wall_data[0]);
	vdp_bitmapSendData(TILE_PLAYER, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);
	vdp_bitmapSendData(TILE_PLAYERONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, player_data[0]);	
	vdp_bitmapSendData(TILE_BOX, BITMAP_WIDTH, BITMAP_HEIGHT, box_data[0]);
	vdp_bitmapSendData(TILE_BOXONGOAL, BITMAP_WIDTH, BITMAP_HEIGHT, boxongoal_data[0]);
	vdp_bitmapSendData(TILE_GOAL, BITMAP_WIDTH, BITMAP_HEIGHT, goal_data[0]);
	vdp_bitmapCreateSolidColor(TILE_FLOOR, BITMAP_WIDTH, BITMAP_HEIGHT, 0xff000000);
	
	vdp_bitmapSendData(TILE_WALL_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, wallmini_data[0]);
	vdp_bitmapSendData(TILE_PLAYER_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, playermini_data[0]);
	vdp_bitmapSendData(TILE_PLAYERONGOAL_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, playermini_data[0]);
	vdp_bitmapSendData(TILE_BOX_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, boxmini_data[0]);
	vdp_bitmapSendData(TILE_BOXONGOAL_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, boxongoalmini_data[0]);
	vdp_bitmapSendData(TILE_GOAL_MINI, MINIMAP_WIDTH, MINIMAP_HEIGHT, goalmini_data[0]);
	vdp_bitmapCreateSolidColor(TILE_FLOOR_MINI,MINIMAP_WIDTH, MINIMAP_HEIGHT, 0xff000000);
	vdp_bitmapCreateSolidColor(TILE_MINIMAP_CLEAR,MINIMAP_WIDTH, MINIMAP_HEIGHT, 0xff000000);
	
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
		vdp_spriteSelect(0); // player
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

void undomove_sprites(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2)
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
		if((undomove[undo_head].pushed) && (spriteid != NOSPRITE))
		{
			vdp_spriteSelect(spriteid);
			vdp_spriteMoveBySelected(dx, dy);
		}
		vdp_spriteSelect(0); // player
		vdp_spriteMoveBySelected(dx, dy);
	}

	// set destination sprite frame
	if((undomove[undo_head].pushed) && (spriteid != NOSPRITE))
	{
		vdp_spriteSelect(spriteid);
		// Check if the sprite moved to a goal or floor
		n2 = currentlevel.data[yn2][xn2];
		switch(n2)
		{
			case CHAR_PLAYER:
				vdp_spriteSetFrameSelected(0);
				break;
			case CHAR_PLAYERONGOAL:
				vdp_spriteSetFrameSelected(1);
				break;
		}
	}
	// update sprite number matrix
	if(undomove[undo_head].pushed)
	{
		if(spriteid != NOSPRITE)
		{
			// player shoved a box here
			sprites[yn2][xn2] = sprites[yn1][xn1];
		}
		sprites[yn1][xn1] = NOSPRITE; // player's sprite isn't handled by using a box spriteid			
	}
	
	vdp_spriteRefresh();
}

void move_updatelevel(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2)
{
	// move can happen, no need to check again
	UINT8 n1, n2;
	BOOL onlyplayermoves;
	
	// move n1 => n2
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

void undomove_updatelevel(UINT16 xn1, UINT16 yn1, UINT16 xn2, UINT16 yn2, UINT16 xn3, UINT16 yn3)
{
	UINT8 n1, n2, n3;
	
	// move n1 => n2 => n3
	n1 = currentlevel.data[yn1][xn1]; // Source / from
	n2 = currentlevel.data[yn2][xn2]; // This is the curent player's position
	n3 = currentlevel.data[yn3][xn3]; // Destination / to
	
	switch(n3)
	{
		case CHAR_FLOOR:
			currentlevel.data[yn3][xn3] = CHAR_PLAYER;
			break;
		case CHAR_GOAL:
			currentlevel.data[yn3][xn3] = CHAR_PLAYERONGOAL;
			break;
		default:
			break;
	}
	
	if(undomove[undo_head].pushed)
	{
		switch(n1)
		{
			case CHAR_BOX:
				currentlevel.data[yn1][xn1] = CHAR_FLOOR;
				break;
			case CHAR_BOXONGOAL:
				currentlevel.data[yn1][xn1] = CHAR_GOAL;
				currentlevel.goalstaken--;
				break;
			default:
				break;
		}
		switch(n2) // revert push to box
		{
			case CHAR_PLAYERONGOAL:
				currentlevel.data[yn2][xn2] = CHAR_BOXONGOAL;
				currentlevel.goalstaken++;
				break;
			default:
				currentlevel.data[yn2][xn2] = CHAR_BOX;
				break;
		}
	}
	else // only the player switched position, nothing was pushed
	{
		switch(n2)
		{
			case CHAR_PLAYERONGOAL:
				currentlevel.data[yn2][xn2] = CHAR_GOAL;
				break;
			default:
				currentlevel.data[yn2][xn2] = CHAR_FLOOR;
				break;
		}
	}
	
	// update player position
	currentlevel.xpos = xn3;
	currentlevel.ypos = yn3;
}

void game_handleUndoMove(void)
{
	INT16 xn1 = 0, xn2 = 0, yn1 = 0, yn2 = 0, xn3 = 0, yn3 = 0;
	
	if(num_undomoves)
	{
		xn2 = currentlevel.xpos;
		yn2 = currentlevel.ypos;

		// pop one move off circular buffer
		if(undo_head) undo_head--;
		else undo_head = UNDOBUFFERSIZE-1;		
		// undo_head no points to previous move				
		
		switch(undomove[undo_head].movekey)
		{
			case 0x8: // undo LEFT
				yn1 = yn2;
				yn3 = yn2;
				xn1 = xn2 - 1;
				xn3 = xn2 + 1;
				break;
			case 0xb: // undo UP
				xn1 = xn2;
				xn3 = xn2;
				yn1 = yn2 - 1;
				yn3 = yn2 + 1;
				break;
			case 0xa: // undo DOWN
				xn1 = xn2;
				xn3 = xn2;
				yn1 = yn2 + 1;
				yn3 = yn2 - 1;
				break;
			case 0x15: // undo RIGHT
				yn1 = yn2;
				yn3 = yn2;
				xn1 = xn2 + 1;
				xn3 = xn2 - 1;
				break;
		}
		undomove_sprites(xn1,yn1,xn2,yn2);			
		undomove_updatelevel(xn1, yn1, xn2, yn2, xn3, yn3);
		
		num_undomoves--;
	}
}

BOOL game_handleKey(char key)
{
	BOOL done = FALSE;
	BOOL move = FALSE;
	INT16	xn1 = 0,xn2 = 0,yn1 = 0,yn2 = 0;
	
	switch(key)
	{
		case 0x8: // LEFT
			xn1 = currentlevel.xpos - 1;
			yn1 = currentlevel.ypos;
			xn2 = currentlevel.xpos - 2;
			yn2 = currentlevel.ypos;
			move = TRUE;
			break;
		case 0xb:
			xn1 = currentlevel.xpos;
			yn1 = currentlevel.ypos - 1;
			xn2 = currentlevel.xpos;
			yn2 = currentlevel.ypos - 2;
			move = TRUE;
			break;
		case 0xa:
			xn1 = currentlevel.xpos;
			yn1 = currentlevel.ypos + 1;
			xn2 = currentlevel.xpos;
			yn2 = currentlevel.ypos + 2;
			move = TRUE;
			break;
		case 0x15: // RIGHT
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
			undomove[undo_head].movekey = key;
			undomove[undo_head].pushed = (currentlevel.data[yn1][xn1] == CHAR_BOX) || (currentlevel.data[yn1][xn1] == CHAR_BOXONGOAL);
			// rotate buffer always
			if(++undo_head == UNDOBUFFERSIZE) undo_head = 0;
			// maximize number of undo
			if(++num_undomoves > UNDOBUFFERSIZE) num_undomoves = UNDOBUFFERSIZE;
			
			move_sprites(xn1,yn1,xn2,yn2);
			move_updatelevel(xn1,yn1,xn2,yn2);
			done = (currentlevel.goals == currentlevel.goalstaken);
		}
	}
	vdp_cursorGoto(0,0);
	//debug_print_playfieldText();
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

void game_splash_screen()
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
	//vdp_cursorGoto(25,38);
	//vdp_fgcolour(255,255,255);
	//puts("Reading levels...");

}

void game_displayHelp(UINT8 xpos, UINT8 ypos)
{
	UINT16 gxpos = xpos * MINIMAP_WIDTH;
	UINT16 gypos = (ypos * 8) + 72;
	vdp_cursorGoto(xpos,ypos);
	vdp_fgcolour(255,255,255);
	puts("Game objective");
	vdp_cursorGoto(xpos,ypos+2);
	vdp_fgcolour(128,128,128);
	puts("Push all boxes");
	vdp_cursorGoto(xpos,ypos+3);
	puts("in this warehouse");
	vdp_cursorGoto(xpos,ypos+4);
	puts("to the target goals.");
	
	vdp_cursorGoto(xpos,ypos+7);
	vdp_fgcolour(255,255,255);
	puts("Legend");
	
	vdp_bitmapDraw(TILE_PLAYER_MINI,gxpos,gypos);
	vdp_bitmapDraw(TILE_BOX_MINI, gxpos, gypos + 16);
	vdp_bitmapDraw(TILE_BOXONGOAL_MINI, gxpos, gypos + 32);
	vdp_bitmapDraw(TILE_GOAL_MINI, gxpos, gypos + 56);
	
	vdp_fgcolour(128,128,128);
	vdp_cursorGoto(xpos+2,ypos+9);
	puts("You, hard at work");
	vdp_cursorGoto(xpos+2,ypos+11);
	puts("Boxes with stuff");
	vdp_cursorGoto(xpos+2,ypos+13);
	puts("Boxes in shipping");
	vdp_cursorGoto(xpos+2,ypos+14);
	puts("position");
	vdp_cursorGoto(xpos+2,ypos+16);
	puts("Shipping goal");
	
	vdp_fgcolour(255,255,255);
	vdp_cursorGoto(xpos,ypos+19);
	puts("Game controls");
	
	vdp_fgcolour(128,128,128);
	vdp_cursorGoto(xpos,ypos+21);
	puts("Cursor");
	vdp_cursorGoto(xpos,ypos+22);
	puts(" keys  - move player");
	vdp_cursorGoto(xpos,ypos+23);
	puts("    u  - undo move");
	vdp_cursorGoto(xpos,ypos+24);
	puts("esc/q  - quit level");
	vdp_cursorGoto(xpos,ypos+26);
	puts("ENTER  - start level");

	return;
}

INT16 game_selectLevel(UINT8 levels, UINT16 previouslevel)
{
	INT16 lvl;
	BOOL selected = FALSE;
	vdp_mode(1); // standard console mode
	vdp_cls();
	vdp_cursorDisable();
	
	lvl = previouslevel;
	
	game_resetSprites();			// clear out any onscreen sprites

	while(!selected)
	{
		
		game_initLevel(lvl);			// initialize playing field data from memory or disk
		vdp_clearGraphics();
		game_displayMinimap();			// display 'current' level

		vdp_cursorGoto(0,4);
		vdp_fgcolour(255,255,255);
		printf("Level %03d / %03d",lvl+1,levels); // user level# starts at 1, internally this is level 0

		vdp_cursorGoto(5,39);
		puts("Select level with cursor keys");
		vdp_cursorGoto(14,41);
		vdp_fgcolour(128,128,128);
		puts("ESC to quit");

		vdp_plotMoveTo(328,48);
		vdp_plotColour(0,128,128);
		vdp_plotLineTo(328,328);
		

		game_displayHelp(HELP_XPOS_MODE1, HELP_YPOS_MODE1);
		
		
		switch(getch())
		{
			case 0x8:
			case 0x0a:
				if(lvl > 0) lvl --;
				else lvl = levels-1;
				break;
			case 0x0b:
			case 0x15:
				if(lvl < levels-1) lvl++;
				else lvl = 0;
				break;
			case 0xd:
				selected = TRUE;
				break;
			case 27:
				lvl = -1;
				selected = TRUE;
				break;
			default:
				break;
		}
	}
	return lvl;
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

void game_displayMinimap(void)
{
	UINT16 width, height;
	UINT16 ystart,xstart,x,y;
	char c;
	
	// calculate on-screen base coordinates
	xstart = ((MAXWIDTH - currentlevel.width) / 2) * MINIMAP_WIDTH;
	ystart = (((MAXHEIGHT - currentlevel.height) / 2) * MINIMAP_HEIGHT) + 56;
	
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
					vdp_bitmapDraw(TILE_WALL_MINI, x, y);
					break;
				case CHAR_PLAYER:
					vdp_bitmapDraw(TILE_PLAYER_MINI, x, y);
					break;
				case CHAR_PLAYERONGOAL:
					vdp_bitmapDraw(TILE_PLAYERONGOAL_MINI, x, y);
					break;
				case CHAR_BOX:
					vdp_bitmapDraw(TILE_BOX_MINI, x, y);
					break;
				case CHAR_BOXONGOAL:
					vdp_bitmapDraw(TILE_BOXONGOAL_MINI, x, y);
					break;
					break;
				case CHAR_GOAL:
					vdp_bitmapDraw(TILE_GOAL_MINI, x, y);					
					break;
				case CHAR_FLOOR:
					vdp_bitmapDraw(TILE_FLOOR_MINI, x, y);			
					break;
				default:
					break;
			}
			x += MINIMAP_WIDTH;
		}
		y += MINIMAP_HEIGHT;
	}
}


UINT8 game_readLevels(char *filename)
{
	// Reads all the levels in the standard file into memory and returns the number of levels
	UINT8 numlevels = 0;
	UINT16 size;
	UINT8 file;
	char* ptr = (char*)LEVELDATA;

	vdp_fgcolour(255,255,255);

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
			vdp_cursorGoto(20,38);
			printf("Loading levels (%03d)....",numlevels);

		}		
		mos_fclose(file);
	}
	return numlevels;
}	


void game_initLevel(UINT8 levelid)
{
	memcpy(&currentlevel, (void*)(LEVELDATA+(sizeof(struct sokobanlevel))*levelid), sizeof(struct sokobanlevel));
	// initialize undo buffer
	undo_head = 0;
	num_undomoves = 0;
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
	