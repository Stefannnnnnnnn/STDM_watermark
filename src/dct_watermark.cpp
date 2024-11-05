/*
 * dct_watermark.cpp
 * Author: Tianyi Li
 * Date: 2024.03.15
 *
 * Functionality: This source file implements functions for Discrete Cosine Transform (DCT), watermark embedding and decoding,
 * and pixel manipulation for bitmap images.
*/

#include <vector>
#include <iostream>
#include <random>
#include "../include/constants.h"
#include "../include/hdc_graphics.h"

using namespace std;

// Constants
const int LOWER_BOUND = 0;
const int UPPER_BOUND = 1;
mt19937_64 gen(time(0));
uniform_real_distribution<> distr(LOWER_BOUND, UPPER_BOUND);

double D[4096][8][8];
double F[4096][8][8];
vector<vector<int>> pixel;
vector<int> res;

// Function to compute the DCT matrix sum
double matrixSumD(const int X, const int Y, const int i, const int j, const int width) {
    double sum = 0;
    for (int y = Y; y < Y + width; y++) {
        for (int x = X; x < X + width; x++) {
            sum += pixel[y][x] * cos((2 * (x - X) + 1) * i * PI / (2 * width)) * 
                                     cos((2 * (y - Y) + 1) * j * PI / (2 * width));
        }
    }
    return sum;
}

// Function to compute normalization coefficient
double C(int u) {
    return (u == 0) ? 1 / sqrt(2) : 1;
}

// Compute the DCT coefficients for a given 8x8 block
void getD(const int x, const int y, const int width) {
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < width; i++) {
            D[x + y * pixel.size() / width][i][j] = 
                1 / sqrt(2 * width) * C(i) * C(j) * matrixSumD(x * width, y * width, i, j, width);
        }
    }
}

// Function to determine the watermarking factor
int W(int n, int N) {
    return (n % 2) ? 1 : -1;
}

// Function to compute normalization coefficient for inverse DCT
double c(int u) {
    return (u == 0) ? 1 : sqrt(2);
}

// Compute the IDCT matrix sum
double matrixSumF(const int n, const int i, const int j, const int width) {
    double sum = 0;

    for (int v = 0; v < width; v++) {
        for (int u = 0; u < width; u++) {
            sum += c(u) * c(v) * D[n][u][v] * 
                   cos((i + 0.5) * u * PI / width) * 
                   cos((j + 0.5) * v * PI / width) / width;
        }
    }
    return clamp(sum, 0.0, 255.0);
}

// Compute the IDCT coefficients
void getF(const int n, const int width) {
    for (int j = 0; j < width; j++) {
        for (int i = 0; i < width; i++) {
            F[n][i][j] = matrixSumF(n, i, j, width);
        }
    }
}

// Perform the DCT on the image
void DCT(const int width) {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            getD(j, i, width);
        }
    }
}

// Perform the inverse DCT on the image
void inverse_DCT(const int width) {
    for (int i = 0; i < _bmp_height / width; i++) {
        for (int j = 0; j < _bmp_width / width; j++) {
            getF(i * _bmp_height / width + j, width);
        }
    }
}

// Quantization functions
double quantization_delta(const double x, const double delta) {
    return delta * floor(x / delta + 0.5);
}

double quantization_b(const double x, const int b, const double delta) {
    double d_b = (b == 1) ? delta / 4 : -delta / 4;
    return quantization_delta(x - d_b, delta) + d_b;
}

// Function to embed a watermark into the image
void embed_watermark(bitmap_image mark, const int M, const double delta) {
    const int K = 8;
    const int L = mark.height() * mark.width();
    const int N = M * K / L;
    double** s = new double*[K * M];

    vector<double> x_projection(M, 0);
    int count = 0;

    // Prepare DCT coefficients for watermark embedding
    for (int i = 0; i < M; i++) {
        s[count++] = &D[i][7][0];
        s[count++] = &D[i][6][1];
        s[count++] = &D[i][5][2];
        s[count++] = &D[i][4][3];
        s[count++] = &D[i][3][4];
        s[count++] = &D[i][2][5];
        s[count++] = &D[i][1][6];
        s[count++] = &D[i][0][7];

        // Compute x_projection for watermarking
        for (int j = 0; j < K; j++) {
            x_projection[i] += D[i][7 - j][j] * W(j, N);
        }
        x_projection[i] /= N;
    }

    count = 0;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < N; j++) {
            int b = mark.get_pixel(i / mark.width(), i % mark.width());
            *s[count] += (quantization_b(x_projection[i], b, delta) - x_projection[i]) * W(j, N);
            count++;
        }
    }
    delete[] s; // Free dynamically allocated memory
}

// Function to draw the watermarked image
void draw_bmp_watermark(const int height, const int width, const int point_x, const int point_y) {
    BITMAPFILEHEADER* bf = new BITMAPFILEHEADER;
    ifstream in("LENA.bmp", ios::binary);
    ofstream out("LENA_tj.bmp", ios::binary);

    in.read((char*)bf, sizeof(BITMAPFILEHEADER));
    in.seekg(0, ios::beg);

    char* header = new char[bf->bfOffBits];
    in.read(header, bf->bfOffBits);
    out.write(header, bf->bfOffBits);
    delete[] header;

    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            int color = static_cast<int>(round(F[i / 8 * 64 + j / 8][j % 8][i % 8]));
            set_pixel(out, (char)color);
        }
    }
    in.close();
    out.close();
    cout << "Watermarked image saved as LENA_tj.bmp" << endl;
}

// Function to set a pixel value in the output bitmap
void set_pixel(ofstream& out, char pix) {
    out << pix;
}

// Copy pixel data from the bitmap image
void copy_bmp_pixel(bitmap_image bmp) {
    for (int i = 0; i < bmp.height(); i++) {
        vector<int> tmp;
        for (int j = 0; j < bmp.width(); j++) {
            tmp.push_back(bmp.get_pixel(i, j));
        }
        pixel.push_back(tmp);
    }
}

// Function to compute the pixel comparison
double comp_pixel(bitmap_image mark) {
    int sum = 0;
    int count = 0;

    for (int i = 0; i < mark.height(); i++) {
        for (int j = 0; j < mark.width(); j++) {
            if ((mark.get_pixel(i, j) == -1 ? 0 : 1) == res[count++]) {
                sum++;
            }
        }
    }
    return static_cast<double>(sum) / (mark.height() * mark.width());
}

// Decode the watermark from the image
double decode_watermark(bitmap_image mark, const int M, const double delta) {
    const int K = 8;
    const int L = mark.height() * mark.width();
    const int N = M * K / L;
    double* S = new double[K * M];

    int count = 0;

    for (int i = 0; i < M; i++) {
        S[count++] = D[i][7][0];
        S[count++] = D[i][6][1];
        S[count++] = D[i][5][2];
        S[count++] = D[i][4][3];
        S[count++] = D[i][3][4];
        S[count++] = D[i][2][5];
        S[count++] = D[i][1][6];
        S[count++] = D[i][0][7];
    }
    
    count = 0;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < N; j++) {
            double s = S[count];
            int b = mark.get_pixel(i / mark.width(), i % mark.width());
            res[count++] = (s < quantization_b(x_projection[i], b, delta) - x_projection[i]) ? 1 : 0;
        }
    }

    delete[] S; // Free dynamically allocated memory
    return comp_pixel(mark);
}

void clear_pixel() {
	vector<vector<int>>().swap(pixel);
}

void clear_res() {
	vector<int>().swap(res);
}

void add_noise(double sigma) {
	double noise;
	for (int i = 0; i < 4096; i++) {
		for (int j = 0; j < _width; j++)
			for (int k = 0; k < _width; k++) {
				double ran1 = distr(gen);
				double ran2 = distr(gen);
				noise = sqrt(-2 * log(1 - ran1)) * sin(2 * PI * ran2) * sigma;
				F[i][j][k] += noise;
			}
	}
}

double ierfc(const double y)
// inverse of the error function erfc
// Copyright(C) 1996 Takuya OOURA (email: ooura@mmm.t.u-tokyo.ac.jp)
{
	const double IERFC_LIM = 27;
	double z = (y > 1) ? 2 - y : y;
	if (z < 1e-300) return (y > 1) ? -IERFC_LIM : IERFC_LIM;
	double w = 0.916461398268964 - log(z);
	double u = sqrt(w);
	double s = (log(u) + 0.488826640273108) / w;
	double t = 1 / (u + 0.231729200323405);
	double x = u * (1 - s * (s * 0.124610454613712 + 0.5)) -
		((((-0.0728846765585675 * t + 0.269999308670029) * t +
			0.150689047360223) * t + 0.116065025341614) * t +
			0.499999303439796) * t;
	t = 3.97886080735226 / (x + 3.97886080735226);
	u = t - 0.5;
	s = (((((((((0.00112648096188977922 * u +
		1.05739299623423047e-4) * u - 0.00351287146129100025) * u -
		7.71708358954120939e-4) * u + 0.00685649426074558612) * u +
		0.00339721910367775861) * u - 0.011274916933250487) * u -
		0.0118598117047771104) * u + 0.0142961988697898018) * u +
		0.0346494207789099922) * u + 0.00220995927012179067;
	s = ((((((((((((s * u - 0.0743424357241784861) * u -
		0.105872177941595488) * u + 0.0147297938331485121) * u +
		0.316847638520135944) * u + 0.713657635868730364) * u +
		1.05375024970847138) * u + 1.21448730779995237) * u +
		1.16374581931560831) * u + 0.956464974744799006) * u +
		0.686265948274097816) * u + 0.434397492331430115) * u +
		0.244044510593190935) * t -
		z * exp(x * x - 0.120782237635245222);
	x += s * (x * s + 1);
	return (y > 1) ? -x : x;
}

double Q(const double x) {
	return 0.5 * erfc(x / sqrt(2));
}

double theory_p_e(const double sigma,const double delta) {
	const int N = 8;
	double p_e = 0;
	for (int m = -100; m <= 100; m++) {
		p_e += Q(sqrt(N) * (m * delta + delta / 4) / sigma) - Q(sqrt(N) * (m * delta + 3 * delta / 4) / sigma);
	}
	return p_e;
}