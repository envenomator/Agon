#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sokobanprep.h"

#define BUFFERSIZE 128

char linebuffer[BUFFERSIZE];

struct sokobanlevel levelbuffer;

int main(int argc, char *argv[])
{
    FILE *fptr;
    int x,y;
    int n;
    int level;
    char c;

    if(argc <= 1)
    {
        printf("Usage:\n\nlevelreader inputfile \n");
        exit(1);
    }
    fptr = fopen(argv[1],"rb");
    if(fptr == NULL)
    {
        printf("Error opening \"%s\"\n",argv[1]);   
        exit(1);             
    }

    level = 0;
    while(1)
    {
        fread(&levelbuffer, sizeof(levelbuffer),1,fptr);
        if(feof(fptr)) break;

        printf("Level %d\n", level);
        printf("Level width: %d, height: %d\n",levelbuffer.width, levelbuffer.height);
        printf("Level  xpos: %d,   ypos: %d\n",levelbuffer.xpos, levelbuffer.ypos);
        printf("Level goals: %d\n",levelbuffer.goals);
        printf("Level goals taken: %d\n",levelbuffer.goalstaken);
        printf("Level crates: %d\n",levelbuffer.crates);
        printf("Level data:\n");
        for(y = 0; y < levelbuffer.height; y++)
        {
            for(x = 0; x < levelbuffer.width; x++)
            {
                c = levelbuffer.data[y][x];
                printf("%c",c?c:' ');
            }
            printf("\n");
        }
        level++;
    }
    fclose(fptr);
    exit(EXIT_SUCCESS);
}