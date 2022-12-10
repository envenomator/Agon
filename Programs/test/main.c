
#include "mos-interface.h"
#include "vdp.h"
#include "sokobanprep.h"

struct level
{
	UINT8 data[16];
};

void f_read(UINT8 *ptr, UINT16 size, UINT8 fh)
{
	UINT8 n = sizeof(struct level);
	
	while(n)
	{
		*ptr = mos_fgetc(fh);
		ptr++;
		n--;
	}
}

int main(int argc, char * argv[]) {

	UINT8 file,n;
	char t;
	struct level lvl;
	
	file = mos_fopen("levels.bin", fa_read);
	
	if(file)
	{
		puts("opened\r\n");
		
		while(!mos_feof(file))
		{
			f_read((UINT8*)&lvl, sizeof(struct level), file);
			for(n = 0; n < sizeof(struct level); n++)
			{
				putch(lvl.data[n]);
			}
			puts("\r\n");
		}
		puts("\r\n");
		
		mos_fclose(file);
	}
	else puts("Error opening file\r\n");
	return 0;
}
