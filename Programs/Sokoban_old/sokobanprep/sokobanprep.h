#include <stdint.h>

#define MAXWIDTH 40
#define MAXHEIGHT 30

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
