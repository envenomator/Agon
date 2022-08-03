#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sokobanprep.h"

#define LOADADDRESSIZE 2
#define HEADERSIZE 12
#define BUFFERSIZE 128

#define DEBUG   0

char linebuffer[BUFFERSIZE];

struct sokobanlevel levelbuffer;

int getplayerpos(char *string);
int get_goalsfromline(char *string);
int get_takengoalsfromline(char *string);
int get_cratesfromline(char *string);
bool isdataline(char *string);
void trim_validright(char *string, int length);     // replaces non-playfield characters at the right of the string to 0
void trim_validleft(char *string);      // replaces non-playfield characters at the left of the string to 0
void purge_string(char *string, int length);
void remove_lfcrchars(char *string);

int main(int argc, char *argv[])
{
    unsigned int numlevels = 0, level = 0,n;
    unsigned int fieldptr = 0;  // will point to the start of each field in memory
    unsigned int previouspayloadsize = 0;
    unsigned int outputlength = 0;
    unsigned int y = 0;
    unsigned int playerpos = 0;
    unsigned int * xpos;
    unsigned int * ypos;
    unsigned int * levelheight;
    unsigned int * levelwidth;
    unsigned int * levelgoals;
    unsigned int * levelgoalstaken; // the number of crates on goals in each level
    unsigned int * levelgoalsopen;  // the number of still to be completed goals in each level
    unsigned int * leveloffset;
    unsigned int * levelcrates;
    bool * validlevel;          // record if each level is valid or not
    unsigned int errorlevels;   // record number of error levels, due to too high or too wide
    bool playerfound = false;
    FILE *fptr,*outptr;
    
    if(argc <= 2)
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

    // Determine the number of levels in this file
    while(fgets(linebuffer, sizeof(linebuffer), fptr) != NULL)
    {
        if(strncmp(linebuffer,"Level",5) == 0) numlevels++;
    }
    rewind(fptr);

    
    // determine max width / height per level
    // prepare arrays to store counters per level
    levelheight = malloc(numlevels * sizeof(unsigned int));
    levelwidth = malloc(numlevels * sizeof(unsigned int));
    levelgoals = malloc(numlevels * sizeof(unsigned int));
    levelgoalstaken = malloc(numlevels * sizeof(unsigned int));
    levelgoalsopen = malloc(numlevels * sizeof(unsigned int));
    levelcrates = malloc(numlevels * sizeof(unsigned int));
    leveloffset = malloc(numlevels * sizeof(unsigned int));
    validlevel = malloc(numlevels * sizeof(bool));
    xpos = malloc(numlevels *sizeof(unsigned int));
    ypos = malloc(numlevels *sizeof(unsigned int));

    purge_string(linebuffer, BUFFERSIZE);
    while(fgets(linebuffer, sizeof(linebuffer), fptr) != NULL)
    {
        if(strncmp(linebuffer,"Level",5) == 0)
        {
            level++; // first level is 0, but marked at '1'
            levelheight[level-1] = 0;
            levelwidth[level-1] = 0;
            levelgoals[level-1] = 0;
            levelgoalstaken[level-1] = 0;
            levelgoalsopen[level-1] = 0;
            leveloffset[level-1] = 0;
            levelcrates[level-1] = 0;
        }
        else
        {
            remove_lfcrchars(linebuffer);
            outputlength = strlen(linebuffer);  // remove EOL character at the end of the string
            if(outputlength) // line has payload data
            {
                if(isdataline(linebuffer)) // line without comments, just the data
                {
                    // store maximum width at this level
                    if(DEBUG) printf("%d\n",outputlength);
                    if(levelwidth[level-1] < outputlength) levelwidth[level-1] = outputlength; // skip LF/CR

                    levelheight[level-1]++; // add another line to this level
                    levelgoals[level-1] += get_goalsfromline(linebuffer);
                    levelgoalstaken[level-1] += get_takengoalsfromline(linebuffer);
                    levelcrates[level-1] += get_cratesfromline(linebuffer);

                }
            }
        }
        purge_string(linebuffer, BUFFERSIZE);
    }
    rewind(fptr);

    // now determine the player's position as an offset at each level and store it in the array
    // we will calculate the actual load address later, after we know which levels are valid
    level = 0;
    playerfound = false;

    purge_string(linebuffer, BUFFERSIZE);
    while(fgets(linebuffer, sizeof(linebuffer), fptr) != NULL)
    {
        if(strncmp(linebuffer,"Level",5) == 0)
        {
            level++; // first level is 0, but marked at '1'
            leveloffset[level-1] = 0; // reset offset for the player at this level
            playerfound = false;
            y = 0;
        }
        else
        {
            remove_lfcrchars(linebuffer);
            outputlength = strlen(linebuffer); //compensate EOL / CR/LF
            if(outputlength) // line has payload
            {
                if(isdataline(linebuffer)) // line without comments, just the data
                {
                    if(playerfound == false)
                    {
                        playerpos = getplayerpos(linebuffer);
                        if(playerpos)
                        {
                            xpos[level-1] = playerpos - 1;
                            ypos[level-1] = y;
                            playerfound = true;
                        }
                    }
                    y++; // next line
                }
            }
        }
        purge_string(linebuffer, BUFFERSIZE);

    }
    rewind(fptr);

    printf("%d levels present in \"%s\"\n",numlevels,argv[1]);
    // check the validity of each level and record for later use in calculating headers and level data
    errorlevels = 0;
    for(int n = 0; n < numlevels; n++)
    {
        validlevel[n] = true; // set as default, unless either width or height is out of spec
        if((levelwidth[n] > MAXWIDTH) || (levelheight[n] > MAXHEIGHT))
        {
            printf("Removed level %d -",n+1);
            if(levelwidth[n] > MAXWIDTH)
            {
                printf(" wider than %d cols",MAXWIDTH);
                validlevel[n] = false;
            }
            if((levelwidth[n] > MAXWIDTH) && (levelheight[n] > MAXHEIGHT)) printf (" and");
            if(levelheight[n] > MAXHEIGHT)
            {
                printf(" higher than %d rows",MAXHEIGHT);
                validlevel[n] = false;
            }
            printf("\n");
        }
        if(levelgoals[n] > levelcrates[n])
        {
            printf("Removed level %d - too many goals\n",n+1);
            validlevel[n] = false;
        }
        if(validlevel[n] == false) errorlevels++; // record for later use in calculating resulting levels
    }
    if(errorlevels) printf("Removed %d invalid levels\n",errorlevels);
    printf("Written %d valid levels to \"%s\"\n",numlevels - errorlevels,argv[2]);

    // produce output file as array of struct sokobanlevel
    if((numlevels - errorlevels) > 0)
    {
        // now transform the input to the output file and pad memory space
        level = 0;
        purge_string(linebuffer, BUFFERSIZE);
        while(fgets(linebuffer, sizeof(linebuffer), fptr) != NULL)
        {
            if(strncmp(linebuffer,"Level",5) == 0)
            {
                if(level) fwrite(&levelbuffer, sizeof(levelbuffer),1, outptr); // write PREVIOUS record to out
                level++; // first level is 0, but marked at '1'
                levelbuffer.xpos = xpos[level-1];
                levelbuffer.ypos = ypos[level-1];
                levelbuffer.height = levelheight[level-1];
                levelbuffer.width = levelwidth[level-1];
                levelbuffer.goals = levelgoals[level-1];
                levelbuffer.goalstaken = levelgoalstaken[level-1];
                levelbuffer.crates = levelcrates[level-1];
                y = 0;
            }
            else
            {
                if((level > 0) && (validlevel[level-1])) // only output valid level(s) - ignore the rest
                {
                    remove_lfcrchars(linebuffer);
                    outputlength = strlen(linebuffer); //compensate EOL / CR/LF
                    if(outputlength) // line has payload
                    {
                        if(isdataline(linebuffer)) // line without comments, just the data
                        {
                            trim_validright(linebuffer, BUFFERSIZE);
                            trim_validleft(linebuffer);
                            for(n = 0; n < MAXWIDTH; n++) levelbuffer.data[y][n] = linebuffer[n];
                            //strncpy(levelbuffer.data[y],linebuffer,MAXWIDTH);
                            y++;
                        }
                    }
                }
            }
            purge_string(linebuffer, BUFFERSIZE);
        }
        fwrite(&levelbuffer, sizeof(levelbuffer),1, outptr); // write LAST record to out
    }
    else printf("No valid levels in input file\n");

    free(levelheight);
    free(levelwidth);
    free(levelgoals);
    free(levelgoalstaken);
    free(levelgoalsopen);
    free(leveloffset);
    free(validlevel);
    free(levelcrates);
    free(xpos);
    free(ypos);
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
    if(pos < length) return pos + 1; // non-zero based position
    else return 0;
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

bool isdataline(char *string)
{
    bool data;
    data = (*string == ' ') || (*string == '#') || (*string == '@') || (*string == '$') || (*string == '.') || (*string == '+') || (*string == '*');
    return data;
}

void trim_validright(char *string, int length)
{
    int n;
    //int length;
    char c;

    //length = strlen(string);

    n = length - 1;

    while(n >= 0)
    {
        c = string[n];
        if((c == '#') || (c == '@') || (c == '$') || (c == '.') || (c == '+') || (c == '*')) break; // valid character found
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
        if((c == '#') || (c == '@') || (c == '$') || (c == '.') || (c == '+') || (c == '*')) break; // valid character found
        string[n] = 0;
        n++;
    }
}

void purge_string(char *string, int length)
{
    for(int n = 0; n < length; n++) string[n] = 0;
    return;
}

void remove_lfcrchars(char *string)
{
    int length = strlen(string);
    int n = length - 1;
    char c;

    while(n >= 0)
    {
        c = string[n];
        switch(c)
        {
            case 0x0a: // LF
            case 0x0d: // CR
                string[n] = 0;
                n = n - 1;
                break;
            default:
                n = -1;
                break;
        }
    }
}