//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#ifndef FADEINFADEOUTYUV_LIB_H
#define FADEINFADEOUTYUV_LIB_H

//#define DEBUG
#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1080
#define PIXEL_NUMBER (IMAGE_WIDTH * IMAGE_HEIGHT)
#define YUV_SIZE (PIXEL_NUMBER * 3 / 2)
#define RGB_SIZE (PIXEL_NUMBER * 3)

#define RANGE(x, a, b) (x < a ? a : ( x > b ? b : x))
#define UV_INDEX(y) ((y / (IMAGE_WIDTH * 2)) * (IMAGE_WIDTH / 2) + (y % IMAGE_WIDTH) / 2)

void yuv_to_rgb(unsigned char *yuv_image, unsigned char *rgb_image);

void rgb_to_yuv(unsigned char *rgb_image, unsigned char *yuv_image);

#endif //FADEINFADEOUTYUV_LIB_H
