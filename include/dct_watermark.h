#pragma once

#include "bitmap_image.h"
#include <fstream>

using namespace std;

// Function prototypes
double matrixSumD(const int X, const int Y, const int i, const int j, const int width);
double matrixSumF(const int n, const int i, const int j, const int width);
int W(int n, int N);
double C(int u);
double c(int u);
void getD(const int x, const int y, const int width);
void getF(const int n, const int width);
void DCT(const int width);
void inverse_DCT(const int width);
void embed_watermark(const bitmap_image& mark, const int M, const double delta);
void draw_bmp_watermark(const int height, const int width, const int point_x = 0, const int point_y = 0);
// Copies the pixel values of the entire image into the pixel array
void copy_bmp_pixel(const bitmap_image& bmp);
void clear_pixel();
void clear_res();
void add_noise(const double sigma);
double decode_watermark(const bitmap_image& mark, const int M, const double delta);
void draw_watermark(const int height, const int width, const int point_x = 0, const int point_y = 0);
void set_pixel(ofstream& out, char pix);
double theory_p_e(const double sigma, const double delta);