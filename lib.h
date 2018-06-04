//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#ifndef FADEINFADEOUTYUV_LIB_H
#define FADEINFADEOUTYUV_LIB_H

#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1080
#define PIXEL_NUMBER (IMAGE_WIDTH * IMAGE_HEIGHT)
#define YUV_SIZE (PIXEL_NUMBER * 3 / 2)
#define RGB_SIZE (PIXEL_NUMBER * 3)

#define RANGE(x, a, b) (x < a ? a : ( x > b ? b : x))

void initialize_uv_index_array();

void yuv_to_rgb_base(unsigned char *yuv_image, unsigned char *rgb_image);

void rgb_to_yuv_base(unsigned char *rgb_image, unsigned char *yuv_image);

void apply_alpha_base(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha);

void yuv_to_rgb_mmx(unsigned char *yuv_image, unsigned char *rgb_image);

void rgb_to_yuv_mmx(unsigned char *rgb_image, unsigned char *yuv_image);

void apply_alpha_mmx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha);

void yuv_to_rgb_sse2(unsigned char *yuv_image, unsigned char *rgb_image);

void rgb_to_yuv_sse2(unsigned char *rgb_image, unsigned char *yuv_image);

void apply_alpha_sse2(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha);

void yuv_to_rgb_avx(unsigned char *yuv_image, unsigned char *rgb_image);

void rgb_to_yuv_avx(unsigned char *rgb_image, unsigned char *yuv_image);

void apply_alpha_avx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha);

#endif //FADEINFADEOUTYUV_LIB_H
