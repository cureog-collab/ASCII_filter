#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixelRGB;

// `^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$

void grayscale(int height, int width, pixelRGB image[height][width]);
void toAscii(int height, int width, pixelRGB image[height][width], FILE *out);
int naming(char *outName, char *name, int length);

const char *BLOCKS = "`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }
    
    // make a copy of the input image for use
    int width, height, channels;
    char *name = argv[1];
    int length = strlen(name);
    unsigned char *img = stbi_load(name, &width, &height, &channels, 3);
    if (img == NULL)
    {
        printf("Cannot open '%s'!\n", name);
        return 1;
    }

    // cast the output of stb_image's img read func to a 2D array of pixelRGBs
    pixelRGB (*image)[width] = (pixelRGB (*)[width])img;

    char outName[256];
    
    if (naming(outName, name, length) != 0)
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

    grayscale(height, width, image);
    toAscii(height, width, image, outputFile);

    fclose(outputFile);
    stbi_image_free(image);
    return 0;
}

// name the file from "###.jpg" (or "###.jpeg"...) to "###.txt"
int naming(char *outName, char *name, int length)
{
    // remove the images/ init
    char *slsh = strrchr(name, '/');
    char *fileName = (slsh != NULL) ? slsh + 1 : name;

    strcpy(outName, fileName);

    char *dot = strrchr(outName, '.');
    if (dot != NULL)
    {
        dot[1] = 't';
        dot[2] = 'x';
        dot[3] = 't';
        dot[4] = '\0';
        return 0;
    }
    return 1;
}

// apply the grayscale filter
void grayscale(int height, int width, pixelRGB image[height][width])
{
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // calculate perceived brightness of each pixel
            // >> 8 is used in place of / 256 for speed
            unsigned char bright = (77 * image[row][col].r + 150 * image[row][col].g + 29 * image[row][col].b)  >> 8;
            image[row][col].r = bright;
            image[row][col].g = bright;
            image[row][col].b = bright;
        }
    }
}

// generate ASCII image result
void toAscii(int height, int width, pixelRGB image[height][width], FILE *out)
{
    const size_t MAX = strlen(BLOCKS);

    for (int row = 0; row < height; row += 3)
    {
        for (int col = 0; col < width; col += 1)
        {
            // map the pixel's brightness to the "ASCII brightness"
            int bright = image[row][col].r;
            int charPos = (bright * MAX) >> 8;

            // write the corresponding ASCII character to the output file
            fputc(BLOCKS[charPos], out);
        }
        fprintf(out, "\n");
    }
}

// TODO: try other methods to handle scaling and loss-of-detail issues
// TODO: add an option to invert the image
// TODO: try using colors to elevate the output