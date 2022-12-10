#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sokobanprep.h"
#define BUFFERSIZE 128

#define DEBUG   0

int getplayerpos(char *string);
int get_goalsfromline(char *string);
int get_takengoalsfromline(char *string);
int get_cratesfromline(char *string);
void trim_validright(char *string, int length);     // replaces non-playfield characters at the right of the string to 0
void trim_validleft(char *string);      // replaces non-playfield characters at the left of the string to 0
void purge_string(char *string, int length);
void remove_lfcrchars(char *string);
bool close_level(FILE *outptr, int levelwidth, int levelheight, bool playerfound, struct sokobanlevel *levelbuffer, int numlevels, int numlevels_valid);
bool isLevelLine(char *string);

int main(int argc, char *argv[])
{
    char linebuffer[BUFFERSIZE];
    struct sokobanlevel levelbuffer;

    bool levelline;
    int levelheight;
    int levelwidth;
    int n;
    int playerpos;
    bool playerfound;
    int numlevels, numlevels_valid;

    FILE *fptr,*outptr;
    
    if(argc != 3)
    {
        printf("Usage:\n\nsokobanprep inputfile outputfile\n");
        exit(1);
    }
    fptr = fopen(argv[1],"r");
    if(fptr == NULL)
    {
        printf("Error opening \"%s\"\n",argv[1]);   
        exit(1);             
    }
    outptr = fopen(argv[2],"wb");
    if(outptr == NULL)
    {
        printf("Error opening \"%s\"\n",argv[2]);
        fclose(fptr);
        exit(1);
    }

    printf("Inputfile \"%s\"\n", argv[1]);
    printf("Max width  : %d\n", MAXWIDTH);
    printf("Max height : %d\n", MAXHEIGHT);

    // total counters
    numlevels = 0;
    numlevels_valid = 0;

    // per level counters, reset for first level
    levelline = false;
    levelheight = 0;
    levelwidth = 0;
    playerfound = false;

    memset(&levelbuffer, 0, sizeof(struct sokobanlevel));        // clear out level before start
    memset(&linebuffer, 0, BUFFERSIZE);                           // clear out linebuffer

    while(fgets(linebuffer, sizeof(linebuffer), fptr) != NULL)
    {
        remove_lfcrchars(linebuffer);
        if(isLevelLine(linebuffer))
        {   
            levelline = true;
            levelwidth = strlen(linebuffer) > levelwidth? strlen(linebuffer):levelwidth;    // maximum of either
            // copy line to buffer at max length. invalidated level will be ignored later on
            if(levelheight < MAXHEIGHT)
            {
                // note stats from each line, while the buffer is still a 'normal' string
                playerpos = getplayerpos(linebuffer);
                levelbuffer.goals += get_goalsfromline(linebuffer);
                levelbuffer.goalstaken += get_takengoalsfromline(linebuffer);
                levelbuffer.crates += get_cratesfromline(linebuffer);

                // prepare line for output formatting, with leading and trailing 0's
                trim_validright(linebuffer, MAXWIDTH);
                trim_validleft(linebuffer);
                // store line
                for(n = 0; n < MAXWIDTH; n++) levelbuffer.data[levelheight][n] = linebuffer[n];
 
                // check if this line has a player in it
                if(playerpos != -1)
                {
                    levelbuffer.xpos = playerpos;
                    levelbuffer.ypos = levelheight;
                    playerfound = true;
                }
            }
            levelheight++;
        }
        else // this is a comment line. Do we need to close a previous level?
        {
            if(levelline)
            {
                if(close_level(outptr, levelwidth, levelheight, playerfound, &levelbuffer, numlevels, numlevels_valid)) numlevels_valid++;
                numlevels++;
                // reset for next level(s)
                memset(&levelbuffer, 0, sizeof(struct sokobanlevel));        // clear out level before start
                levelheight = 0;
                levelwidth = 0;
                playerfound = false;
            }
            levelline = false;
            // ignore comment line
        }
        memset(&linebuffer, 0, BUFFERSIZE); // clear out linebuffer for next line
    }
    // done
    if(levelline)
    {
        // close previous level, if valid
        {
            if(close_level(outptr, levelwidth, levelheight, playerfound, &levelbuffer, numlevels, numlevels_valid)) numlevels_valid++;
            numlevels++;
        }
    }

    printf("%d Total levels present, %d error levels\n", numlevels, numlevels - numlevels_valid);
    //printf("%d Error levels present\n", numlevels - numlevels_valid);
    printf("%d Valid levels output to \"%s\"\n", numlevels_valid, argv[2]);

    fclose(fptr);
    fclose(outptr);
    exit(EXIT_SUCCESS);
}

int getplayerpos(char *string)
{
    unsigned int pos;
    unsigned int length;
    bool found = false;

    length = strlen(string);
    pos = 0;
    while(pos < length)
    {
        if(string[pos] == '@' || string[pos] == '+') break;
        pos++;
    }
    if(pos < length) return pos;
    else return -1;
}
int get_goalsfromline(char *string)
{
    unsigned int goalnum = 0;
    while(*string)
    {
        if(*string == '.' || *string == '+' || *string == '*') goalnum++;
        string++;
    }
    return goalnum;
}

int get_takengoalsfromline(char *string)
{
    unsigned int takengoals = 0;
    while(*string)
    {
        if(*string == '*') takengoals++;
        string++;
    }
    return takengoals;
    
}

int get_cratesfromline(char *string)
{
    unsigned int cratenum = 0;
    while(*string)
    {
        if(*string == '$' || *string == '*') cratenum++;
        string++;
    }
    return cratenum;
}

bool isLevelLine(char *string)
{
    // 0-n characters ' ', followed by at least 1 character '#'
    int length = strlen(string);
    int n = 0;

    while(n < length)
    {
        switch (string[n])
        {
            case ' ':
                break;
            case '#':
                return true;
                break;
            default:
                return false;
        }
        n++;
    }
    return false;
}

void trim_validright(char *string, int length)
{
    int n;
    char c;
    n = length - 1;

    while(n >= 0)
    {
        c = string[n];
        if((c == '#') || (c == '@') || (c == '$') || (c == '.') || (c == '+') || (c == '*')) break; // valid character found. space is invalid outside the walls
        string[n] = 0;
        n--;
    }
}

void trim_validleft(char *string)
{
    int n;
    int length;
    char c;

    length = strlen(string);

    n = 0;

    while(n < length)
    {
        c = string[n];
        if((c == '#') || (c == '@') || (c == '$') || (c == '.') || (c == '+') || (c == '*')) break; // valid character found. space is invalid outside the walls
        string[n] = 0;
        n++;
    }
}

void remove_lfcrchars(char *string)
{
    int length = strlen(string);
    int n;

    for(n = length - 1; n >= 0; n--)
    {
        if((string[n] == 0x0a) || (string[n] == 0x0d)) string[n] = 0;
        else break;
    }
}

bool close_level(FILE *outptr, int levelwidth, int levelheight, bool playerfound, struct sokobanlevel *levelbuffer, int numlevels, int numlevels_valid)
{
    // output level, if valid
    if((levelwidth <= MAXWIDTH) && (levelheight <= MAXHEIGHT) && (playerfound) && (levelbuffer->goals) && (levelbuffer->crates) && (levelbuffer->goals <= levelbuffer->crates))
    {
        levelbuffer->width = levelwidth;
        levelbuffer->height = levelheight;
        fwrite(levelbuffer, sizeof(struct sokobanlevel),1, outptr);
        return true;
    }
    else // print error(s) for this level
    {
        if(levelwidth > MAXWIDTH) printf("Removed level %02d - too wide\n", numlevels);
        if(levelheight> MAXHEIGHT)printf("Removed level %02d - too high\n", numlevels);
        if((levelwidth < MAXWIDTH) && (levelheight < MAXHEIGHT))
        {
            // levelheight was correct, so we should have noted these stats, which could be in error now
            if(!playerfound) printf("Removed level %02d - no player found\n", numlevels);
            if(levelbuffer->goals == 0) printf("Removed level %02d - no goals found\n", numlevels);
            if(levelbuffer->crates == 0) printf("Removed level %02d - no crates found\n", numlevels);
            if(levelbuffer->goals > levelbuffer->crates) printf("Removed level %02d - number of crates (%d) doesn't match number of goals (%d)\n", numlevels,levelbuffer->crates,levelbuffer->goals);
        }
    }
    return false;
}
