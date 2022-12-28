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
#include <ez80.h>

void delay100us(UINT16 n)
{
	n = n * 28;
	while(n)
	{
		n--;
	}
}