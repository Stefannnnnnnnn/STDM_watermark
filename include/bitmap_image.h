/*
 * bitmap_image.h
 * Author: Tianyi Li
 * Date: 2024.03.05
 *
 * This header file defines the bitmap_image class for handling BMP images.
 * It provides functionalities to read BMP files, retrieve image dimensions, 
 * access pixel colors, and display the image and its color table.
 */

#pragma once
#include <Windows.h>

class bitmap_image
{
protected:
    /* BMP file header and info header */
    BITMAPFILEHEADER* bf; // Bitmap file header
    BITMAPINFOHEADER* bi; // Bitmap info header
    int** pixel;          // 2D array for storing pixel data

public:
    RGBQUAD* pColorTable; // Color table for the image

    // Constructor that initializes the bitmap_image from a BMP file
    bitmap_image(const char* filename);
    
    // Returns the width of the image
    int width() const;

    // Returns the height of the image
    int height() const;

    // Returns the RGB color of the specified pixel
    int get_pixel(int row, int col) const;

    // Reads the BMP file and initializes image data
    void readBmp(const char* filename);

    // Displays the entire image from top to bottom and left to right
    void draw_bmp(const int point_x = 0, const int point_y = 0);

    // Draws the color table for the image
    void draw_pcolortable();
};



