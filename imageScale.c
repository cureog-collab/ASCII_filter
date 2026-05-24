#include <math.h>
#include "core.h"

pixelRGB *decimateHalf(int currH, int currW, pixelRGB image[currH][currW], int *nextH, int *nextW);
pixelRGB *bilinearScale(int currH, int currW, int goalH, int goalW, pixelRGB image[currH][currW]);

// recursively downscale the image by a half through Gaussian pyramid
pixelRGB *imageScaleDown(int currH, int currW, int goalH, int goalW, pixelRGB image[currH][currW])
{
    // base case
    if (currH / 2 <= goalH || currW / 2 <= goalW)
    {
        pixelRGB *finalImg = bilinearScale(currH, currW, goalH, goalW, image);
        return finalImg;
    }

    // Gaussian decimate
    int nextH;
    int nextW;
    pixelRGB *halfImg = decimateHalf(currH, currW, image, &nextH, &nextW);

    // call the function again
    pixelRGB *resultImg = imageScaleDown(nextH, nextW, goalH, goalW, (pixelRGB (*)[nextW])halfImg);

    free(halfImg);
    return resultImg;
}

pixelRGB *decimateHalf(int currH, int currW, pixelRGB image[currH][currW], int *nextH, int *nextW)
{
    // halve the image dimensions
    *nextH = currH / 2;
    *nextW = currW / 2;

    const int GAUSSIAN_KERNEL_3[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    pixelRGB (*newImg)[*nextW] = malloc(*nextH * *nextW * sizeof(pixelRGB));
    if (newImg == NULL)
    {
        printf("Cannot scale down image!\n");
        return NULL;
    }

    int srcRow;
    int srcCol;

    // loop through each pixel of the new image
    for (int row = 0; row < *nextH; row++)
    {
        for (int col = 0; col < *nextW; col++)
        {
            int rSum = 0;
            int gSum = 0;
            int bSum = 0;

            srcRow = row << 1;
            srcCol = col << 1;

            int vNear;
            int hNear;

            // loop through a 3x3 block surrounding the pixel in the original image
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    vNear = srcRow + i;
                    hNear = srcCol + j;
                    if (isValid(vNear, hNear, currH, currW))
                    {
                        rSum += image[vNear][hNear].r * GAUSSIAN_KERNEL_3[i + 1][j + 1];
                        gSum += image[vNear][hNear].g * GAUSSIAN_KERNEL_3[i + 1][j + 1];
                        bSum += image[vNear][hNear].b * GAUSSIAN_KERNEL_3[i + 1][j + 1];
                    }
                }
            }

            newImg[row][col].r = rSum >> 4;
            newImg[row][col].g = gSum >> 4;
            newImg[row][col].b = bSum >> 4;
        }
    }

    return (pixelRGB *)newImg;
}

pixelRGB *bilinearScale(int currH, int currW, int goalH, int goalW, pixelRGB image[currH][currW])
{
    pixelRGB (*newImg)[goalW] = malloc(goalH * goalW * sizeof(pixelRGB));
    if (newImg == NULL)
    {
        printf("Cannot scale down image!\n");
        return NULL;
    }

    float yRatio = (currH - 1) / (float) (goalH - 1);
    float xRatio = (currW - 1) / (float) (goalW - 1);

    float srcRow;
    float srcCol;
    float dy;
    float dx;
    float w[2][2];

    // scan each pixel in the new image
    for (int row = 0; row < goalH; row++)
    {
        for (int col = 0; col < goalW; col++)
        {
            srcRow = yRatio * row;
            srcCol = xRatio * col;

            // define the vicinity of the pixel to work with
            int vNear0 = (int) srcRow;
            int hNear0 = (int) srcCol;
            int vNear1 = (vNear0 + 1 < currH) ? vNear0 + 1 : vNear0;
            int hNear1 = (hNear0 + 1 < currW) ? hNear0 + 1 : hNear0;

            // distance from the float position to the pinned pixel
            dy = srcRow - vNear0;
            dx = srcCol - hNear0;

            // calculate bilinear scaling coefficients
            float w00 = (1.0f - dx) * (1.0f - dy);
            float w01 = dx * (1.0f - dy);
            float w10 = (1.0f - dx) * dy;
            float w11 = dx * dy;

            // calculate sums of weighted brightness
            float rSum = image[vNear0][hNear0].r * w00 + image[vNear0][hNear1].r * w01 +
                         image[vNear1][hNear0].r * w10 + image[vNear1][hNear1].r * w11;
            
            float gSum = image[vNear0][hNear0].g * w00 + image[vNear0][hNear1].g * w01 +
                         image[vNear1][hNear0].g * w10 + image[vNear1][hNear1].g * w11;
                         
            float bSum = image[vNear0][hNear0].b * w00 + image[vNear0][hNear1].b * w01 +
                         image[vNear1][hNear0].b * w10 + image[vNear1][hNear1].b * w11;

            // write into the new image's pixel
            newImg[row][col].r = (int) rSum;
            newImg[row][col].g = (int) gSum;
            newImg[row][col].b = (int) bSum;
        }
    }

    return (pixelRGB *)newImg;
}
