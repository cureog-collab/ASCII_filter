#include <string.h>
#include "core.h"

int takeSample(int height, int width, int row, int col, int boxH, int boxW, pixelRGB image[height][width]);

// .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$
const char *ASCII_BLOCKS[] = {
    // Level 1: Ultra Detail
    " .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$",

    // Level 2: Smooth / Soft Grayscale
    " .`',:;i~_-?|/cxUJQ0Owmdb*#MW8B@$",

    // Level 3: Balanced
    ".',:;+*?%S#",

    // Level 4: High Contrast
    " .:-=+*#@",

    // Level 5: Stencil
    "  .#@"
};

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
// TODO: try other methods to handle scaling and loss-of-detail issues
void toAscii(int height, int width, pixelRGB image[height][width], FILE *out, int contrastLvl)
{
    const char *BLOCKS = ASCII_BLOCKS[contrastLvl - 1];
    const size_t MAX = strlen(BLOCKS);

    // sample box for sampling pixels
    int sampleH = 3;
    int sampleW = 1;

    // locate start and end positions for scanning
    int startY = sampleH / 2;
    int startX = sampleW / 2;
    int endY = height - (sampleH / 2);
    int endX = width - (sampleW / 2);

    for (int row = startY; row <= endY; row += sampleH)
    {
        for (int col = startX; col <= endX; col += sampleW)
        {
            // averages brightness in each sample box of size sampleW * sampleH
            int bright = takeSample(height, width, row, col, sampleH, sampleW, image);
            
            // map the pixel's brightness to the "ASCII brightness"
            image[row][col].r = bright;
            int charPos = (bright * MAX) >> 8;

            // write the corresponding ASCII character to the output file
            fputc(BLOCKS[charPos], out);
        }
        fprintf(out, "\n");
    }
}

// edge detection filter
void edge(int height, int width, pixelRGB image[height][width])
{
    // TODO
    printf("edge\n");
}

// emboss filter
void emboss(int height, int width, pixelRGB image[height][width])
{
    // TODO
    printf("emboss\n");
}

// posterize filter
void posterize(int height, int width, pixelRGB image[height][width])
{
    // TODO
    printf("posterize\n");
}

// dither filter
void dither(int height, int width, pixelRGB image[height][width])
{
    // TODO
    printf("dither\n");
}

// invert filter
void invert(int height, int width, pixelRGB image[height][width])
{
    // TODO
    printf("invert\n");
}

int takeSample(int height, int width, int row, int col, int boxH, int boxW, pixelRGB image[height][width])
{
    int sum = 0;
    int pixels = 0;
    for (int i = row - (boxH / 2); i <= row + (boxH / 2); i++)
    {
        for (int j = col - (boxW / 2); j <= col + (boxW / 2); j++)
        {
            if (!(i < 0 || i > height || j < 0 || j > width))
            {
                pixels++;
                sum += image[i][j].r;
            }
        }
    }
    return sum / pixels;
}
