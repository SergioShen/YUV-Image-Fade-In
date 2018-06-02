//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#include "lib.h"
#include <cstdio>

void yuv_to_rgb(unsigned char *yuv_image, unsigned char *rgb_image) {
    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no++) {
        float _y = (float) yuv_image[pixel_no];
        float _u = (float) yuv_image[PIXEL_NUMBER + UV_INDEX(pixel_no)];
        float _v = (float) yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + UV_INDEX(pixel_no)];

#ifdef DEBUG
        printf("%f, %f, %f\n", _y, _u, _v);
#endif

        float _r = 1.164383 * (_y - 16) + 1.596027 * (_v - 128);
        float _g = 1.164383 * (_y - 16) - 0.391762 * (_u - 128) - 0.812968 * (_v - 128);
        float _b = 1.164383 * (_y - 16) + 2.017232 * (_u - 128);

#ifdef DEBUG
        printf("%f, %f, %f\n", _r, _g, _b);
#endif

        rgb_image[pixel_no * 3] = (unsigned char) RANGE(_r, 0, 255);
        rgb_image[pixel_no * 3 + 1] = (unsigned char) RANGE(_g, 0, 255);
        rgb_image[pixel_no * 3 + 2] = (unsigned char) RANGE(_b, 0, 255);
    }
}

void rgb_to_yuv(unsigned char *rgb_image, unsigned char *yuv_image) {
    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no++) {
        float _r = (float) rgb_image[pixel_no * 3];
        float _g = (float) rgb_image[pixel_no * 3 + 1];
        float _b = (float) rgb_image[pixel_no * 3 + 2];

#ifdef DEBUG
        printf("%f, %f, %f\n", _r, _g, _b);
#endif
        float _y = 0.256788 * _r + 0.504129 * _g + 0.097906 * _b + 16;
        float _u = -0.148223 * _r - 0.290993 * _g + 0.439216 * _b + 128;
        float _v = 0.439216 * _r - 0.367788 * _g - 0.071427 * _b + 128;

#ifdef DEBUG
        printf("%f, %f, %f\n", _y, _u, _v);
#endif

        yuv_image[pixel_no] = (unsigned char) RANGE(_y, 0, 255);
        yuv_image[PIXEL_NUMBER + UV_INDEX(pixel_no)] = (unsigned char) RANGE(_u, 0, 255);
        yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + UV_INDEX(pixel_no)] = (unsigned char) RANGE(_v, 0, 255);
    }
}