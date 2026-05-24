## C-based Image-to-ASCII Filter

It does what the name suggests, transforming input images into ASCII art. All coded in C, taking command-line arguments. Supports additional filters (invert, edge detection, emboss, dither...).

Based heavily on the foundational concepts I  learned from CS50 (more precisely, the CS50's "Filter (More)" problem set).

## Features

Beyond simple brightness-to-character mapping, this engine features several custom-built filters applied prior to ASCII conversion (invert, emboss, edge detection...).

## Usage

Run the project with the following syntax:
`./ascii_filter [flags] <input_image_address> [max_width]`

**Arguments:**
* `<input_image_address>`: Path to your input image (JPEG/PNG).
* `[max_width]`: *(Optional)* Ceiling value (in px) for the output width. Defaults to `1920`.

**Currently Supported Flags:**
* `-n`: Normal brightness-to-ASCII mode (Default).
* `-b`: Gaussian Blur *(Experimental)* (Acts as a low-pass filter to reduce noise).
* `-e`: Sobel Edge Detection *(Experimental)* (Highly recommended to use alongside `-b` to prevent noise artifacting, e.g., `-be`).
* `-m`: Emboss filter.
* `-v`: Vector Map *(Experimental)* (Renders gradient directions. Recommended to use with `-b`).
* `-d [2-15]`: Atkinson Error Diffusion Dithering (defines the number of shading levels).
* `-c [1-5]`: Adjust contrast stretching intensity (Default: 3).
* `-i`: Invert colors.

> **Note:** The `-b` (Blur), `-e` (Edge), and `-v` (Vector Map) filters are currently under development.

## Dependencies:
Uses Sean Barrett's header-only stb_image.h to decode the original image files (JPEG/PNG).
