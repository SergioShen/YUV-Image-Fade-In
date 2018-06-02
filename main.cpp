#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "lib.h"

using namespace std;

unsigned char original_yuv_image[YUV_SIZE];
unsigned char original_rgb_image[RGB_SIZE];
unsigned char processed_yuv_image[YUV_SIZE];
unsigned char processed_rgb_image[RGB_SIZE];

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "No input file" << endl;
        return -1;
    }
    string file_name(argv[1]);

    ifstream original_yuv_file;
    original_yuv_file.open(file_name.c_str(), ios::in | ios::binary);
    original_yuv_file.read((char *) original_yuv_image, YUV_SIZE);

    // Produce original RGB image
    yuv_to_rgb(original_yuv_image, original_rgb_image);

    for (int alpha = 1; alpha < 255; alpha += 3) {
        // Produce alpha RGB image
        apply_alpha(original_rgb_image, processed_rgb_image, (unsigned char) alpha);

#ifdef SAVE_RGB
        // Save RBG image
        ofstream processed_rgb_file;
        processed_rgb_file.open((file_name + string(".") + to_string(alpha) + string(".rgb")).c_str(),
                                ios::out | ios::binary);
        processed_rgb_file.write((char *) processed_rgb_image, RGB_SIZE);
#endif
        // Produce YUV image
        rgb_to_yuv(processed_rgb_image, processed_yuv_image);

        // Save YUV image
        ofstream processed_yuv_file;
        processed_yuv_file.open((file_name + string(".") + to_string(alpha) + string(".yuv")).c_str(),
                                ios::out | ios::binary);
        processed_yuv_file.write((char *) processed_yuv_image, YUV_SIZE);
    }
    return 0;
}