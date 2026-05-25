#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>  
#include <stdbool.h>

// -- CONSTANTS & KERNELS --

static const int GAUSSIAN_KERNEL_3[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
};

static const int GAUSSIAN_KERNEL_5[5][5] = {
    { 1,  4,  6,  4,  1 },
    { 4, 16, 24, 16,  4 },
    { 6, 24, 36, 24,  6 },
    { 4, 16, 24, 16,  4 },
    { 1,  4,  6,  4,  1 }
};

// -- DATA STRUCTURES --

// define pixel type
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixelRGB;

// -- CORE FILTERS PROTOTYPES --

// grayscale filter
void grayscale(int height, int width, pixelRGB image[height][width]);

// normal image-to-ASCII filter
char *toAscii(int height, int width, pixelRGB image[height][width], FILE *out, int contrastLvl, bool isWeb);

// vector file mapping filter
char *vectorMap(int height, int width, pixelRGB image[height][width], FILE *out, bool isWeb);

// blur filter
void blur(int height, int width, pixelRGB image[height][width]);

// edge detection filter
void edge(int height, int width, pixelRGB image[height][width]);

// emboss filter
void emboss(int height, int width, pixelRGB image[height][width]);

// dither filter
void dither(int height, int width, pixelRGB image[height][width], int calledDivs);

// invert filter
void invert(int height, int width, pixelRGB image[height][width]);

// -- GLOBAL HELPER FUNCTIONS --

int clamp(int x);
int findNearest(int x, int list[], int length);
bool isValid(int posY, int posX, int height, int width);

// image scaler
pixelRGB *imageScaleDown(int currH, int currW, int goalH, int goalW, pixelRGB image[currH][currW]);

#endif