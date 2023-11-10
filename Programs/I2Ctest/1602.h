#ifndef LCD1602_H
#define LCD1602_H
#include <defines.h>

UINT8 lcd1602Init(UINT8 iAddr);
UINT8 lcd1602SetCursor(UINT8 x, UINT8 y);
UINT8 lcd1602Control(UINT8 bBacklight, UINT8 bCursor, UINT8 bBlink);
UINT8 lcd1602WriteString(char *szText);
UINT8 lcd1602Clear(void);
void lcd1602Shutdown(void);
#endif // LCD1602_H