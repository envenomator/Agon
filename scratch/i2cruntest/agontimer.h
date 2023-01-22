/*
 * Title:			AGON timer interface
 * Author:			Jeroen Venema
 * Created:			06/11/2022
 * Last Updated:	06/11/2022
 * 
 * Modinfo:
 * 06/11/2022:		Initial version
 */

#include <defines.h>

#ifndef AGONTIMER_H
#define AGONTIMER_H

extern UINT24 timer0;
extern void	timer0_handler(void);

void timer0_begin(int interval);
void timer0_end(void);

void delayms(int ms);

#endif AGONTIMER_H
