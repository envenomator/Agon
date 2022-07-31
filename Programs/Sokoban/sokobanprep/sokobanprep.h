#include <stdint.h>

#define MAXWIDTH 20
#define MAXHEIGHT 15

struct sokobanlevel
{
	uint8_t xpos;
	uint8_t ypos;
	uint8_t width;
	uint8_t height;
	uint8_t goals;
	uint8_t goalstaken;
	uint8_t crates;	
	uint8_t data[MAXHEIGHT][MAXWIDTH];
};
