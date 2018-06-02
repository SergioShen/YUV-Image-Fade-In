//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#include "lib.h"
#include <cstdio>

void yuv_to_rgb_base(unsigned char *yuv_image, unsigned char *rgb_image) {
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

void rgb_to_yuv_base(unsigned char *rgb_image, unsigned char *yuv_image) {
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

void apply_alpha_base(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no++) {
        dst_rgb_image[pixel_no * 3] = (unsigned char) ((int) src_rgb_image[pixel_no * 3] * (int) alpha / 256);
        dst_rgb_image[pixel_no * 3 + 1] = (unsigned char) ((int) src_rgb_image[pixel_no * 3 + 1] * (int) alpha / 256);
        dst_rgb_image[pixel_no * 3 + 2] = (unsigned char) ((int) src_rgb_image[pixel_no * 3 + 2] * (int) alpha / 256);
    }
}

typedef short v4hi __attribute__ ((vector_size(8)));

void yuv_to_rgb_mmx(unsigned char *yuv_image, unsigned char *rgb_image) {
    yuv_to_rgb_base(yuv_image, rgb_image);
}

void rgb_to_yuv_mmx(unsigned char *rgb_image, unsigned char *yuv_image) {
    rgb_to_yuv_base(rgb_image, yuv_image);
}

void apply_alpha_mmx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
    v4hi src_vector, alpha_vector, const_8_vector, result;

    // Initialize const vectors
    for (int i = 0; i < 4; i++) {
        ((short *) &alpha_vector)[i] = alpha;
        ((short *) &const_8_vector)[i] = 0;
    }
    ((short *) &const_8_vector)[0] = 8;

    // Process every byte
    for (int byte_no = 0; byte_no < PIXEL_NUMBER * 3; byte_no += 4) {
        ((short *) &src_vector)[0] = (short) src_rgb_image[byte_no];
        ((short *) &src_vector)[1] = (short) src_rgb_image[byte_no + 1];
        ((short *) &src_vector)[2] = (short) src_rgb_image[byte_no + 2];
        ((short *) &src_vector)[3] = (short) src_rgb_image[byte_no + 3];

        result = __builtin_ia32_pmullw(src_vector, alpha_vector);
        result = __builtin_ia32_psrlw(result, const_8_vector);

        dst_rgb_image[byte_no] = (unsigned char) (((short *) (&result))[0]);
        dst_rgb_image[byte_no + 1] = (unsigned char) (((short *) (&result))[1]);
        dst_rgb_image[byte_no + 2] = (unsigned char) (((short *) (&result))[2]);
        dst_rgb_image[byte_no + 3] = (unsigned char) (((short *) (&result))[3]);
    }
}

void yuv_to_rgb_sse2(unsigned char *yuv_image, unsigned char *rgb_image) {

}

void rgb_to_yuv_sse2(unsigned char *rgb_image, unsigned char *yuv_image) {

}

void apply_alpha_sse2(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {

}

void yuv_to_rgb_avx(unsigned char *yuv_image, unsigned char *rgb_image) {

}

void rgb_to_yuv_avx(unsigned char *rgb_image, unsigned char *yuv_image) {

}

void apply_alpha_avx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {

}
