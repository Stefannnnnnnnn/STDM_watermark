/*
 * Author: Tianyi
 * Date: 2024.03.18
 * Functionality: The Entrance of the project.The program embeds a watermark into
 * an image using DCT and decodes it. It performs operations including reading images,
 * applying DCT, embedding watermarks, and testing robustness against noise and 
 * quantization parameters.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include "./include/dct_watermark.h"
#include "./include/constants.h"

using namespace std;

int main(int argc, char** argv) {
    // Set console window size for display
    system("mode con cols=175 lines=45");
    system("cls");

    hdc_init(0, 7, 1366, 768);
    hdc_cls();

    bitmap_image bmp("LENA.bmp");
    bitmap_image mark("tj-logo.bmp");
    ofstream out1("result1.txt");
    ofstream out2("result2.txt");

    for (double delta = 4; delta <= 4; delta += 0.01) {
        for (double sigma = 1.5; sigma <= 1.5; sigma += 0.01) {
            // Load pixel values from the bitmap image into the pixel array
            copy_bmp_pixel(bmp);

            // Perform DCT transformation
            DCT(_width);

            // Embed watermark into the image
            embed_watermark(mark, M, delta);

            // Perform inverse DCT transformation
            inverse_DCT(_width);

            // Generate the watermarked image
            draw_bmp_watermark(bmp.height(), bmp.width());

            // Read the pixel values from the watermarked image into an array
            clear_pixel();
            copy_bmp_pixel(); // Assuming this function takes the new image as input

            // Perform DCT transformation on the watermarked image
            DCT(_width);

            // Decode the watermark from the watermarked image
            double res = decode_watermark(mark, M, delta);

            // Log results based on the parameter being tested
            out1 << delta << ' ' << setprecision(6) << (1 - res) << endl;
            cout << "Watermark decoded successfully! Quantization step size: " << delta
                 << "  Experimental error rate: " << setprecision(6) << (1 - res) << endl;
            out2 << delta << ' ' << setprecision(6) << theory_p_e(sigma, delta) << endl;

            // Clear pixel data for next iteration
            clear_pixel();
            clear_res();
        }
    }

    // Release resources and close output files
    hdc_release();
    out1.close();
    out2.close();

    return 0;
}