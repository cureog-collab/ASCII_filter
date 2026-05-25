#include <emscripten.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "core.h" 

const bool isWeb = true;

// allocate memory for js
EMSCRIPTEN_KEEPALIVE
uint8_t* jsMalloc(int size)
{
    return (uint8_t*)malloc(size);
}

EMSCRIPTEN_KEEPALIVE
void jsFree(uint8_t* ptr)
{
    free(ptr);
}

//communicate with web
EMSCRIPTEN_KEEPALIVE
char* process_image(uint8_t* image_buffer, int buffer_size, int maxW, 
                    int contrastLevel, int ditherLvls, 
                    int do_blur, int do_edge, int do_emboss, 
                    int do_dither, int do_invert, int do_vector) 
{
    int width, height, channels;
    
    // read the image file from RAM
    unsigned char* img = stbi_load_from_memory(image_buffer, buffer_size, &width, &height, &channels, 3);
    if (img == NULL)
    {
        return "ERROR: Cannot decode image!";
    }

    void *activeImage = img;
    int currentH = height;
    int currentW = width;

    // rescale the image if needed
    if (width > maxW)
    {
        int maxH = (height * maxW) / width;
        pixelRGB *scaledFlat = imageScaleDown(height, width, maxH, maxW, (pixelRGB (*)[width])img);
        if (scaledFlat != NULL) {
            free(img);
            activeImage = scaledFlat;
            currentH = maxH;
            currentW = maxW;
        }
    }

    // cast the image to a 2D array for easy use
    pixelRGB (*image)[currentW] = (pixelRGB (*)[currentW]) activeImage;

    // call filters
    grayscale(currentH, currentW, image);

    if (do_blur)
    {
        blur(currentH, currentW, image);
    }
    if (do_edge)
    {
        edge(currentH, currentW, image);
    }
    if (do_emboss)
    {
        emboss(currentH, currentW, image);
    }
    if (do_dither)
    {
        dither(currentH, currentW, image, ditherLvls);
    }
    if (do_invert)
    {
        invert(currentH, currentW, image);
    }

    char* final_ascii;

    // output the final ASCII map
    if (do_vector)
    {
        final_ascii = vectorMap(currentH, currentW, image, NULL, isWeb);
    } else {
        final_ascii = toAscii(currentH, currentW, image, NULL, contrastLevel, isWeb);
    }

    free(activeImage);

    return final_ascii;
}