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

void yuv_to_rgb(unsigned char *yuv_image, unsigned char *rgb_image) {
#if defined(MMX)
    yuv_to_rgb_mmx(yuv_image, rgb_image);
#elif defined(SSE2)
    yuv_to_rgb_sse2(yuv_image, rgb_image);
#elif defined(AVX)
    yuv_to_rgb_avx(yuv_image, rgb_image);
#else
    yuv_to_rgb_base(yuv_image, rgb_image);
#endif
}

void rgb_to_yuv(unsigned char *rgb_image, unsigned char *yuv_image) {
#if defined(MMX)
    rgb_to_yuv_mmx(rgb_image, yuv_image);
#elif defined(SSE2)
    rgb_to_yuv_sse2(rgb_image, yuv_image);
#elif defined(AVX)
    rgb_to_yuv_avx(rgb_image, yuv_image);
#else
    rgb_to_yuv_base(rgb_image, yuv_image);
#endif
}

void apply_alpha(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
#if defined(MMX)
    apply_alpha_mmx(src_rgb_image, dst_rgb_image, alpha);
#elif defined(SSE2)
    apply_alpha_sse2(src_rgb_image, dst_rgb_image, alpha);
#elif defined(AVX)
    apply_alpha_avx(src_rgb_image, dst_rgb_image, alpha);
#else
    apply_alpha_base(src_rgb_image, dst_rgb_image, alpha);
#endif
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "No input file" << endl;
        return -1;
    }
    string file_name(argv[1]);

    initialize_uv_index_array();

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