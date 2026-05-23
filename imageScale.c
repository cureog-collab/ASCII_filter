#include <math.h>
#include "core.h"

// recursively downscale the image by a half through Gaussian pyramid

// hand the aftermost result to bicubic algorithm, scaling the image down 1 final time