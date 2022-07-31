# Agon
## Proposed ESP32 Video changes
In anticipation to Dean Belfeld's Quark GA version, I added a few routines to handle sprite VDU commands.
It looks like sprite VDU have always been platform dependent, so it wouldn't hurt compatiblity to add specific sprite commands to Agon.
My proposal is to add an additional command structure to existing VDU 23/0x17, where most SYS commands live. Extension 27/0x1B has been used by the Acorn BBC Basic, so this seems like a natural fit.

I have defined 15 8bit commands to VDU 23, extension 27. BBC Basic has no way to capture return values from a VDU command.
The following commands are structured in such a way to reflect this, and build up a bitmap/sprite in a series of commands in sequence.

### Command structure
| **    Command (8bit)   ** |          **    Description   **         |                    **    Parameters   **                    | **    Remarks   **                                                                                                    |
|:-------------------------:|:---------------------------------------:|:-----------------------------------------------------------:|-----------------------------------------------------------------------------------------------------------------------|
|           bitmaps         |                                         |                                                             |                                                                                                                       |
|              0            |              Select   bitmap            |                      bitmap_id   (8bit)                     |     Subsequent   bitmap commands are directed to selected bitmap id                                                   |
|              1            |            Send   bitmap data           |     width   (16bit), height (16bit), pixel_array (32bit)    |     Allocates   bitmap from VDU memory if necessary. Each pixel has 32bit color information   in ABGR format          |
|              2            |      Define   bitmap in single color    |         width   (16bit), height (16bit), color(32bit)       |     Allocates   bitmap from VDU memory if necessary. Color in ABGR format                                             |
|              3            |          Draw   bitmap to screen        |                    x   (16bit), y (16bit)                   |     Draws   bitmap to screen                                                                                          |
|           sprites         |                                         |                                                             |                                                                                                                       |
|              4            |              Select   sprite            |                      sprite_id   (8bit)                     |     Subsequent   sprite commands are directed to selected sprite id                                                   |
|              5            |              Clear   frames             |                               -                             |     Clears   out all frames from selected sprite. Do not clear  sprites in activated range                            |
|              6            |           Add   frame to sprite         |                      bitmap_id   (8bit)                     |     Adds   first or next frame to sprite, based on bitmap_id. Fails silently if   bitmap_id points to empty bitmap    |
|              7            |            Activate   sprites           |                        number   (8bit)                      |     Actives   sprites 0-(number-1). Sprites need to have at least one frame added to them,   else GDU crashes         |
|              8            |        Set   next frame on sprite       |                               -                             |                                                                                                                       |
|              9            |      Set   previous frame on sprite     |                               -                             |                                                                                                                       |
|             10            |     Set   current frame id on sprite    |                       frame_id   (8bit)                     |     frame_id   needs to be 0 - framecount-1. Fails silently if outside this range, or if   sprite is still empty.     |
|             11            |               Show   sprite             |                               -                             |                                                                                                                       |
|             12            |               Hide   sprite             |                               -                             |                                                                                                                       |
|             13            |             Move   sprite to            |                    x   (16bit), y (16bit)                   |     Move to   absolute coordinates. Sprites are at (0,0) by default                                                   |
|             14            |          Move   sprite by offset        |                    x   (16bit), y (16bit)                   |     Move to   relative coordinates                                                                                    |

### Example commands
- Select sprite 7  - transmit byte sequence 23,27,4,7
- Send bitmap data - transmit byte sequence 23,27,1,WidthLSB,WidthMSB,HeightLSB,HeightMSB, width x height x 4 byte of data
- Show sprite      - transmit byte sequence 23,27,11 

## Proposed C client library
Include vdu.c and vdu.h in your ez80 project, and include the latter in your main project files to use the necessary routines.

A proper sequence of events in creating bitmaps and/or sprites should be:
1. select a bitmap - bitmap_select(id). Any number 0-255 will do
2. send one or more bitmaps data in ABGR format - bitmap_sendData(width, height, *data)
3. select a sprite, starting with id 0 - sprite_select(id)
4. Potentially clear out old frames on this sprite, left by previous programs - sprite_clearFrames(id)
5. add one or more frames to a sprite - sprite_addFrame(id, bitmap_id), using previously created bitmaps
6. Set sprite to show immediately on activation, hidden by default - sprite_show(id), sprite_hide(id)
7. Activate NUMBER of adjacent sprites (starting at 0) to the GDU - sprite_activateTotal(number)

Bitmaps can be drawn on the background canvas by using bitmap_draw(id), regardless of being (re)used by any sprites on the foreground.