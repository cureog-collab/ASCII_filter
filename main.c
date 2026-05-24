
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core.h"

#define MAX_FILTERS 16

int naming(char *outName, char *name, bool flags[256], int contrastLevel, int ditherLevels);

// define valid filter flags
const char *filtersList = "nbvid:mec:";

int main(int argc, char *argv[])
{

    // extract the input flag and check its validity
    int opt;
    int contrastLevel = 3;
    int ditherLvls = 8;
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
                printf("Invalid contrast level, please choose a number from 1 through 5.\n");
                return 1;
            }
            contrastLevel = contrastCalled;
        }
        // check if the user want to call dither filter and modify it accordingly
        else if (opt == 'd')
        {
            int ditherCalled = atoi(optarg);
            if (ditherCalled < 2 || ditherCalled > 15)
            {
                printf("Invalid dither levels, please choose a number from 2 through 15.\n");
                return 1;
            }
            ditherLvls = ditherCalled;
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

    // scaling the image if it's oversized
    int goalW = 120;
    void *activeImage = img;

    // check if the image is oversized
    if (width > goalW)
    {
        int goalH = (height * goalW) / (width * 2);

        pixelRGB *scaledFlat = imageScaleDown(height, width, goalH, goalW, (pixelRGB (*)[width])img);

        if (scaledFlat != NULL)
        {
            stbi_image_free(img);
            
            activeImage = scaledFlat;
            height = goalH;
            width = goalW;
            printf("Image scaled to %d x %d\n", width, height);
        }
        else
        {
            printf("Scaling failed! System is out of memory. Proceeding with original.\n");
        }
    }

    pixelRGB (*image)[width] = (pixelRGB (*)[width]) activeImage;

    // holder for output file name
    char outName[256];
    
    if (naming(outName, name, flags, contrastLevel, ditherLvls) != 0)
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
    if (flags['b'])
    {
        blur(height, width, image);
    }
    if (flags['e'])
    {
        edge(height, width, image);
    }

    if (flags['m'])
    {
        emboss(height, width, image);
    }

    if (flags['d'])
    {
        dither(height, width, image, ditherLvls);
    }

    if (flags['i'])
    {
        invert(height, width, image);
    }
    
    if (flags['v'])
    {
        vectorMap(height, width, image, outputFile);
    }
    else
    {
        toAscii(height, width, image, outputFile, contrastLevel);
    }
    // transform the whole thing to ASCII art

    fclose(outputFile);
    free(image);
    return 0;
}

// name the file from "###.jpg" to "###abc.txt" based on active flags (-a -b -c)
int naming(char *outName, char *name, bool flags[256], int contrastLevel, int ditherLevels)
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

    // prepare the list of suffixes for the name
    char suffixes[MAX_FILTERS + 8];
    int i = 0;
    for (int j = 0; j < filtersLen; j++)
    {
        if (flags[filtersList[j]] && filtersList[j] != 'c')
        {
            suffixes[i] = filtersList[j];
            i++;
        }
    }
    if (flags['d'])
    {
        snprintf(&suffixes[i], sizeof(suffixes) - i, "%d_%d.txt", contrastLevel, ditherLevels);
    }
    else
    {
        snprintf(&suffixes[i], sizeof(suffixes) - i, "%d.txt", contrastLevel);
    }
    // construct the final name
    strcat(outName, suffixes);
    return 0;
}