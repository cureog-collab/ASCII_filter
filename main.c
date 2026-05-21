#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core.h"

#define MAX_FILTERS 16

int naming(char *outName, char *name, bool flags[256], int contrastLevel);

// define valid filter flags
const char *filtersList = "npidmec:";

int main(int argc, char *argv[])
{

    // extract the input flag and check its validity
    int opt;
    int contrastLevel = 3;
    bool flags[256] = {false}; 
    bool hasFlag = false;
    while ((opt = getopt(argc, argv, filtersList)) != -1)
    {
        if (opt == '?')
        {
            printf("Invalid filter!\n");
            return 1;
        }

        // signify the user has used at least one flag
        hasFlag = true;

        // turn on the called flags
        flags[opt] = true;

        // check if the user wants to modify contrast level
        if (opt == 'c')
        {
            int contrastCalled = atoi(optarg);
            if (contrastCalled < 1 || contrastCalled > 5)
            {
                printf("Invalid contrast level, please choose a level from 1 through 5.\n");
                return 1;
            }
            contrastLevel = contrastCalled;
        }
    }

    // if no flags are used, do normal ASCII filter
    if (!hasFlag)
    {
        flags['n'] = true;
    }
    
    // check if the user has entered an image
    if (optind >= argc)
    {
        printf("Usage: %s [flags]... <image_name>\n", argv[0]);
        return 1;
    }

    // make a copy of the input image for use
    int width, height, channels;
    char *name = argv[optind];
    int length = strlen(name);
    unsigned char *img = stbi_load(name, &width, &height, &channels, 3);
    if (img == NULL)
    {
        printf("Cannot open '%s'!\n", name);
        return 1;
    }

    // cast the output of stb_image's img read func to a 2D array of pixelRGBs
    pixelRGB (*image)[width] = (pixelRGB (*)[width])img;

    // holder for output file name
    char outName[256];
    
    if (naming(outName, name, flags, contrastLevel) != 0)
    {
        printf("Cannot generate txt file name!\n");
        stbi_image_free(image);
        return 1;
    }
    char outPath[512];
    snprintf(outPath, sizeof(outPath), "output/%s", outName);
    FILE *outputFile = fopen(outPath, "wb");
    if (outputFile == NULL)
    {
        printf("Cannot generate txt file!\n");
        return 1;
    }

    // grayscale the whole thing to get started
    grayscale(height, width, image);

    // call flagged filters
    if (flags['e'])
    {
        edge(height, width, image);
    }

    if (flags['m'])
    {
        emboss(height, width, image);
    }

    if (flags['p'])
    {
        posterize(height, width, image);
    }

    if (flags['d'])
    {
        dither(height, width, image);
    }

    if (flags['i'])
    {
        invert(height, width, image);
    }
    
    // transform the whole thing to ASCII art
    toAscii(height, width, image, outputFile, contrastLevel);

    fclose(outputFile);
    stbi_image_free(image);
    return 0;
}

// name the file from "###.jpg" to "###abc.txt" based on active flags (-a -b -c)
int naming(char *outName, char *name, bool flags[256], int contrastLevel)
{
    int filtersLen = strlen(filtersList);

    // identify the images/ init (if there is any) and extract the "naked" image file name
    char *slsh = strrchr(name, '/');
    char *fileName = (slsh != NULL) ? slsh + 1 : name;

    strcpy(outName, fileName);

    // identify the .jpg (or .png, .jpeg...)
    char *dot = strrchr(outName, '.');
    if (dot == NULL)
    {
        return 1;
    }

    // slice off the file type "tail"
    *dot = '\0';

    // prepare the list of suffixes for the name (called filters)
    char suffixes[MAX_FILTERS + 6];
    char buffer[sizeof(int)];
    int i = 0;
    for (int j = 0; j < filtersLen; j++)
    {
        if (flags[filtersList[j]] && filtersList[j] != 'c')
        {
            suffixes[i] = filtersList[j];
            i++;
        }
    }
    sprintf(buffer, "%i", contrastLevel);
    suffixes[i] = *buffer;
    suffixes[i + 1] = '.';
    suffixes[i + 2] = 't';
    suffixes[i + 3] = 'x';
    suffixes[i + 4] = 't';
    suffixes[i + 5] = '\0';

    // construct the final name
    strcat(outName, suffixes);
    return 0;
}