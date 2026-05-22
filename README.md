# C-based Image-to-ASCII Filter

It does what the name suggests, transforming input images into ASCII art. All coded in C, taking command-line arguments. Supports additional filters (invert, edge detection, emboss, dither...).

Based heavily on the foundational concepts I  learned from CS50 (more precisely, the CS50's "Filter (More)" problem set).

## Features

Beyond simple brightness-to-character mapping, this engine features several custom-built filters applied prior to ASCII conversion (invert, emboss, edge detection...).

# Run the project with the following syntax
./ascii_filter [flags] <path_to_input_image>

Currently supported Flags:

-n: Normal brightness-to-ASCII mode (Default).

-e: Sobel Edge Detection.

-m: Emboss.

-d [2-15]: Atkinson Error Diffusion Dithering.

-c [1-5]: Adjust contrast stretching intensity (Default: 3).

-i: Invert colors.

# Dependencies:
Uses Sean Barrett's header-only stb_image.h to decode the original image files (JPEG/PNG).
