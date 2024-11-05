/*
 * bitmap_image.cpp
 * Author: Tianyi Li
 * Date: 2024.03.05
 *
 * This file implements the bitmap_image class for handling BMP images.
 * It provides functionalities to read BMP files, retrieve image dimensions, 
 * access pixel colors, and display the image and its color table.
 */

#include <iostream>
#include <fstream>
#include <stdexcept>
#include "../include/hdc_graphics.h"
#include "../include/constants.h"

using namespace std;

// Constructor that initializes the bitmap_image from a BMP file
bitmap_image::bitmap_image(const char* filename)
    : bf(nullptr), bi(nullptr), pColorTable(nullptr), pixel(nullptr)
{
    ifstream in(filename, ios::in | ios::binary);
    if (!in) {
        throw runtime_error("Failed to open the file");
    }

    bf = new(nothrow) BITMAPFILEHEADER;
    bi = new(nothrow) BITMAPINFOHEADER;

    if (!bf || !bi) {
        throw runtime_error("Memory allocation failed for BITMAP headers");
    }

    in.read(reinterpret_cast<char*>(bf), sizeof(BITMAPFILEHEADER));
    in.read(reinterpret_cast<char*>(bi), sizeof(BITMAPINFOHEADER));

    // Allocate color table based on the bit count
    int colorTableSize = (bi->biBitCount == 8) ? 256 : 2;
    pColorTable = new(nothrow) RGBQUAD[colorTableSize];

    if (!pColorTable) {
        throw runtime_error("Memory allocation failed for color table");
    }

    in.read(reinterpret_cast<char*>(pColorTable), colorTableSize * sizeof(RGBQUAD));

    // Allocate memory for pixel data
    pixel = new(nothrow) int*[bi->biHeight];
    if (!pixel) {
        throw runtime_error("Memory allocation failed for pixel array");
    }
    for (int i = 0; i < bi->biHeight; ++i) {
        pixel[i] = new(nothrow) int[bi->biWidth];
        if (!pixel[i]) {
            throw runtime_error("Memory allocation failed for pixel row");
        }
    }

    readBmp(in);
}

// Destructor to free allocated memory
bitmap_image::~bitmap_image() {
    delete[] pColorTable;
    for (int i = 0; i < bi->biHeight; ++i) {
        delete[] pixel[i];
    }
    delete[] pixel;
    delete bf;
    delete bi;
}

// Returns the height of the image
int bitmap_image::height() const {
    return bi->biHeight;
}

// Returns the width of the image
int bitmap_image::width() const {
    return bi->biWidth;
}

// Returns the RGB color of the specified pixel
int bitmap_image::get_pixel(int row, int col) const {
    if (row < 0 || row >= height() || col < 0 || col >= width()) {
        throw out_of_range("Pixel coordinates are out of bounds");
    }
    return pixel[row][col];
}

// Reads the BMP file and initializes pixel data
void bitmap_image::readBmp(ifstream& in) {
    in.seekg(bf->bfOffBits, ios::beg);
    switch (bi->biBitCount) {
        case 1:
            for (int i = 0; i < bi->biHeight; ++i) {
                unsigned char byte;
                for (int j = 0; j < bi->biWidth / 8; ++j) {
                    byte = in.get();
                    for (int k = 0; k < 8; ++k) {
                        int pix = (byte & 0x80) ? 1 : -1; // 0 is black, 1 is white
                        pixel[bi->biHeight - i - 1][j * 8 + k] = pix;
                        byte <<= 1; // Shift left for the next pixel
                    }
                }
            }
            break;
        case 8:
            for (int i = 0; i < bi->biHeight; ++i) {
                for (int j = 0; j < bi->biWidth; ++j) {
                    int color = in.get();
                    pixel[bi->biHeight - i - 1][j] = color;
                }
                // Padding for rows
                if (bi->biWidth % 4 != 0) {
                    in.seekg(4 - (bi->biWidth % 4), ios::cur);
                }
            }
            break;
        default:
            throw runtime_error("Unsupported bit count for BMP image");
    }
}

// Displays the entire image from top to bottom and left to right
void bitmap_image::draw_bmp(const int point_x, const int point_y) {
    for (int i = 0; i < height(); i++) {
        for (int j = 0; j < width(); j++) {
            int rgb = RGB(get_pixel(i, j), get_pixel(i, j), get_pixel(i, j));
            hdc_set_pencolor(rgb);
            hdc_base_point(point_x + j, point_y + i);
        }
    }
}

// Draws the color table for the image
void bitmap_image::draw_pcolortable() {
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            hdc_set_pencolor(RGB(pColorTable[j].rgbRed, pColorTable[j].rgbGreen, pColorTable[j].rgbBlue));
            hdc_base_point(j, i);
        }
    }
