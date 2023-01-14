# Agon
## ESP32 sprite commands
Currently there are 16 8bit commands to VDU 23, extension 27. These commands are structured in a way to not require any return values from the VDU.

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
|             15            |          Refresh all sprites            |                                      |     
### Example commands
- Select sprite 7  - transmit byte sequence 23,27,4,7
- Send bitmap data - transmit byte sequence 23,27,1,WidthLSB,WidthMSB,HeightLSB,HeightMSB, width x height x 4 byte of data
- Show sprite      - transmit byte sequence 23,27,11 

A proper sequence of events in creating bitmaps and/or sprites should be:
1. select a bitmap - Any number 0-255 will do
2. send one or more bitmaps data in ABGR format
3. select a sprite, starting with id 0
4. Potentially clear out old frames on this sprite, left by previous programs
5. add one or more frames to a sprite, using previously created bitmaps
6. Set sprite to show immediately on activation, hidden by default
7. Activate NUMBER of adjacent sprites (starting at 0) to the GDU
8. Refresh sprites, also after changes to position

Bitmaps can be drawn on the background canvas by using bitmap_draw(id), regardless of being (re)used by any sprites on the foreground.

## AgonLight case
All case files are present in the repository.
Check out my [printables page](https://www.printables.com/model/235402-agonlight-case) for additional descriptions to print it yourself.