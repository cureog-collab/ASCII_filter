#include <math.h>
#include "core.h"

int takeSample(int height, int width, int row, int col, int boxH, int boxW, pixelRGB image[height][width]);

int clamp(int x);
int findNearest(int x, int list[], int length);

int sobelGx(int height, int width, int row, int col, pixelRGB ref[height][width]);
int sobelGy(int height, int width, int row, int col, pixelRGB ref[height][width]);
int sobelAvg(int x, int y);

int shineLight(int height, int width, int row, int col, pixelRGB ref[height][width]);

bool isValid(int posY, int posX, int height, int width);

// .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$
const char *ASCII_BLOCKS[] = {
    // Level 1: Ultra Detail
    " .`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$",

    // Level 2: Smooth / Soft Grayscale
    " .`',:;i~_-?|/cxUJQ0Owmdb*#MW8B@$",

    // Level 3: Balanced (default)
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
// TODO: try to scale every input image down to a maximum size if it's oversized (but keep it the same if the input image is alrady small enough)
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
    int endY = height - 1 - (sampleH / 2);
    int endX = width - 1 - (sampleW / 2);

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
        fputc('\n', out);
    }
}

// vector file mapping filter
void vectorMap(int height, int width, pixelRGB image[height][width], FILE *out)
{
    const char NOISE[4] = {' ', '.', ':', ';'};
    const char VECTOR_BLOCKS[4][2] = {
        {'|', '+'},
        {'\\', '*'},
        {'-', '+'},
        {'/', '*'}
    };

    // making a copy of the original image to work with
    pixelRGB (*ref)[width] = malloc(height * width * sizeof(pixelRGB));
    if (ref == NULL)
    {
        printf("Cannot allocate memory!\n");
        return;
    }

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            ref[row][col] = image[row][col];
        }
    }

    // geometric and amplitude properties of the segment
    int tuningFactor = 1;
    int xGrad;
    int yGrad;
    int gradMag;
    int noiseThres = 60;
    float angleWRTx;

    // preparing for the loop (similar to toAscii())
    int sampleH = 3;
    int sampleW = 1;
    int startY = sampleH / 2;
    int startX = sampleW / 2;
    int endY = height - 1 - (sampleH / 2);
    int endX = width - 1 - (sampleW / 2);

    for (int row = startY; row <= endY; row += sampleH)
    {
        for (int col = startX; col <= endX; col += sampleW)
        {
            // apply the Sobel kernels to calculate brightness gradient
            xGrad = tuningFactor * sobelGx(height, width, row, col, ref);
            yGrad = tuningFactor * sobelGy(height, width, row, col, ref);
            gradMag = sobelAvg(xGrad, yGrad);

            // calculate angle value in degrees and make sure it is positive
            angleWRTx = 180 / (float) 3.14159265 * atan2f((float) yGrad, (float) xGrad);
            if (angleWRTx < 0)
            {
                angleWRTx += 180;
            }

            // if the change is so minute, considers it noise
            if (gradMag < noiseThres)
            {
                int noiseIndx = gradMag / 15;
                if (noiseIndx > 3)
                {
                    noiseIndx = 3;
                }
                fputc(NOISE[noiseIndx], out);
            }
            // draw
            else
            {
                // TODO
            }
        }
        fputc('\n', out);
    }

    free(ref);
}

// edge detection filter
// use Sobel operator
void edge(int height, int width, pixelRGB image[height][width])
{
    // copy the original image
    pixelRGB (*ref)[width] = malloc(height * width * sizeof(pixelRGB));
    if (ref == NULL)
    {
        printf("Cannot allocate memory!\n");
        return;
    }

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            ref[row][col] = image[row][col];
        }
    }

    //apply the Sobel op
    int gxVal;
    int gyVal;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            gxVal = sobelGx(height, width, row, col, ref);
            gyVal = sobelGy(height, width, row, col, ref);
            image[row][col].r = sobelAvg(gxVal, gyVal);
            image[row][col].g = image[row][col].r;
            image[row][col].b = image[row][col].r;
        }
    }

    free(ref);
}

// emboss filter
void emboss(int height, int width, pixelRGB image[height][width])
{
    // copy the original image
    pixelRGB (*ref)[width] = malloc(height * width * sizeof(pixelRGB));
    if (ref == NULL)
    {
        printf("Cannot allocate memory!\n");
        return;
    }

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            ref[row][col] = image[row][col];
        }
    }

    // "shine" light from -45 degree onto the image
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            image[row][col].r = shineLight(height, width, row, col, ref);
            image[row][col].g = image[row][col].r;
            image[row][col].b = image[row][col].r;
        }
    }

    free(ref);
}

// dither filter
void dither(int height, int width, pixelRGB image[height][width], int calledDivs)
{
    int NUM_LEVELS = calledDivs;
    int step = 255 / (NUM_LEVELS - 1);
    int shades[NUM_LEVELS];
    shades[0] = 0;
    for (int i = 1; i < NUM_LEVELS; i++)
    {
        shades[i] = shades[i - 1] + step;
    }

    int oldBright;

    int newBright;
    float errCoeff;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // store the current brightness of the pixel to oldBright
            oldBright = image[row][col].r;

            // calculate new brightness and its quantization error thereof
            newBright = shades[findNearest(oldBright, shades, NUM_LEVELS)];
            errCoeff = (oldBright - newBright) / (float) 8;

            // assign new brightness for the current pixel
            image[row][col].r = newBright;
            image[row][col].g = newBright;
            image[row][col].b = newBright;
            
            // Atkinson diffuse
            if (col + 1 < width)
            {
                image[row][col + 1].r = clamp(round(image[row][col + 1].r + errCoeff));
                image[row][col + 1].g = image[row][col + 1].r;
                image[row][col + 1].b = image[row][col + 1].r;
                if (col + 2 < width)
                {
                    image[row][col + 2].r = clamp(round(image[row][col + 2].r + errCoeff));
                    image[row][col + 2].g = image[row][col + 2].r;
                    image[row][col + 2].b = image[row][col + 2].r;
                }
            }
            if (row + 1 < height)
            {
                image[row + 1][col].r = clamp(round(image[row + 1][col].r + errCoeff));
                image[row + 1][col].g = image[row + 1][col].r;
                image[row + 1][col].b = image[row + 1][col].r;
                if (col - 1 >= 0)
                {
                    image[row + 1][col - 1].r = clamp(round(image[row + 1][col - 1].r + errCoeff));
                    image[row + 1][col - 1].g = image[row + 1][col - 1].r;
                    image[row + 1][col - 1].b = image[row + 1][col - 1].r;
                }
                if (col + 1 < width)
                {
                    image[row +1][col + 1].r = clamp(round(image[row + 1][col + 1].r + errCoeff));
                    image[row +1][col + 1].g = image[row +1][col + 1].r;
                    image[row +1][col + 1].b = image[row +1][col + 1].r;
                }
                if (row + 2 < height)
                {
                    image[row + 2][col].r = clamp(round(image[row + 2][col].r + errCoeff));
                    image[row + 2][col].g = image[row + 2][col].r;
                    image[row + 2][col].b = image[row + 2][col].r;
                }
            }
        }
    }
}

// invert filter
void invert(int height, int width, pixelRGB image[height][width])
{
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            image[row][col].r = 255 - image[row][col].r;
            image[row][col].g = image[row][col].r;
            image[row][col].b = image[row][col].r;
        }
    }
}

// -- helper funcs --

int takeSample(int height, int width, int row, int col, int boxH, int boxW, pixelRGB image[height][width])
{
    int sum = 0;
    int pixels = 0;
    for (int i = row - (boxH / 2); i <= row + (boxH / 2); i++)
    {
        for (int j = col - (boxW / 2); j <= col + (boxW / 2); j++)
        {
            if (isValid(i, j, height, width))
            {
                pixels++;
                sum += image[i][j].r;
            }
        }
    }
    return sum / pixels;
}

// make sure the colorcode stay in the range 0x00 - 0xff
int clamp(int x)
{
    if (x < 0)
    {
        return 0;
    }
    else if (x > 255)
    {
        return 255;
    }
    return x;
}

int findNearest(int x, int list[], int length)
{
    int nearestIndex = 0;
    int nearestGap = abs(x - list[0]);
    int gap;
    for (int i = 0; i < length; i++)
    {
        gap = abs(x - list[i]);
        if (gap < nearestGap)
        {
            nearestIndex = i;
            nearestGap = gap;
        }
    }
    return nearestIndex;
}

// Sobel Gx kernel
int sobelGx(int height, int width, int row, int col, pixelRGB ref[height][width])
{
    int output = 0;
    int factor = 0;
    for (int vNear = row - 1; vNear <= row + 1; vNear++)
    {
        for (int hNear = col - 1; hNear <= col + 1; hNear += 2)
        {
            if (isValid(vNear, hNear, height, width))
            {
                factor = (hNear - col) * (abs(vNear - row) * -1 + 2);
                // summing them up gradually
                output += factor * ref[vNear][hNear].r;
            }
        }
    }
    return output;
}

// Sobel Gy kernel
int sobelGy(int height, int width, int row, int col, pixelRGB ref[height][width])
{
    int output = 0;
    int factor = 0;
    for (int hNear = col - 1; hNear <= col + 1; hNear++)
    {
        for (int vNear = row - 1; vNear <= row + 1; vNear += 2)
        {
            if (isValid(vNear, hNear, height, width))
            {
                factor = (vNear - row) * (abs(hNear - col) * -1 + 2);
                // summing them up gradually
                output += factor * ref[vNear][hNear].r;
            }
        }
    }
    return output;
}

int sobelAvg(int x, int y)
{
    // return clamp(round(sqrt(x * x + y * y)));
    return clamp(abs(x) + abs(y));
}

int shineLight(int height, int width, int row, int col, pixelRGB ref[height][width])
{
    int highlight = 0;
    int shadow = 0;

    // sum the highlight
    highlight += isValid(row - 2, col - 2, height, width) ? ref[row - 2][col - 2].r : 0;
    highlight += isValid(row - 1, col - 1, height, width) ? ref[row - 1][col - 1].r : 0;

    // sum the shadow
    shadow += isValid(row + 2, col + 2, height, width) ? ref[row + 2][col + 2].r : 0;
    shadow += isValid(row + 1, col + 1, height, width) ? ref[row + 1][col + 1].r : 0;
    
    return clamp((highlight - shadow) * 2 + 128);
}

bool isValid(int posY, int posX, int height, int width)
{
    return (posY >= 0 && posY < height && posX >= 0 && posX < width);
}
