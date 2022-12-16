#ifndef FLASH_H
#define FLASH_H

extern void enableFlashKeyRegister(void);
extern void lockFlashKeyRegister(void);
extern void writeflash(UINT24);
extern void writerow(void);
extern void reset(void);

#endif FLASH_H