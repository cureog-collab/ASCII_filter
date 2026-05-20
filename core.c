#include <string.h>
#include "core.h"

// .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$
const char *BLOCKS = " .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

/* TODO: let user choose between:
full range:    " .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"
balanced:      " .',:;+*?%S#M@"
high contrast: " .:-=+*#%@"
*/

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