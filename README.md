## C-based Image-to-ASCII Filter

It does what the name suggests, transforming input images into ASCII art. All coded in C, taking command-line arguments. Supports additional filters (invert, edge detection, emboss, dither...).

Based heavily on the foundational concepts I  learned from CS50 (more precisely, the CS50's "Filter (More)" problem set).

## Features

Beyond simple brightness-to-character mapping, this engine features several custom-built filters applied prior to ASCII conversion (invert, emboss, edge detection...).

## Usage (Terminal)
Run the project with the following syntax:
./ascii_filter [flags] <input_image_address> [max_width]

Arguments:

<input_image_address>: Path to your input image (JPEG/PNG).

[max_width]: (Optional) Maximum width in pixels for the output. Defaults to 1920.

Supported Flags:

-n: Normal brightness-to-ASCII mode (Default).

-b: Gaussian Blur.

-e: Edge Detection (Recommended to use with -b, e.g., -be).

-m: Emboss filter.

-v: Vector Map *(Experimental)* (Recommended to use with -b).

-d [2-15]: Atkinson Error Diffusion Dithering (defines shading levels).

-c [1-5]: Adjust contrast stretching intensity (Default: 3).

-i: Invert colors.

## Usage (Web)
Navigate to the deployed project URL.

Upload an image via the "Choose File" button.

Configure settings (Max width, contrast, filters) via the sidebar settings.

Click "RENDER ASCII" to process the image using the C-Core engine and view the result in the ASCII tab.

## Dependencies
stb_image.h: Sean Barrett's header-only library for image decoding.

Created by cureog