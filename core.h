#ifndef CORE_H
#define CORE_H

#include <stdio.h>

// define pixel type
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixelRGB;

// grayscale filter
void grayscale(int height, int width, pixelRGB image[height][width]);

// normal image-to-ASCII filter
void toAscii(int height, int width, pixelRGB image[height][width], FILE *out);

// edge detection filter
void edge(int height, int width, pixelRGB image[height][width]);

// emboss filter
void emboss(int height, int width, pixelRGB image[height][width]);

// posterize filter
void posterize(int height, int width, pixelRGB image[height][width]);

// dither filter
void dither(int height, int width, pixelRGB image[height][width]);

// invert filter
void invert(int height, int width, pixelRGB image[height][width]);

#endif
