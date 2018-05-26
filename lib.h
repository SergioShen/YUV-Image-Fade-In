//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#ifndef FADEINFADEOUTYUV_LIB_H
#define FADEINFADEOUTYUV_LIB_H

#define PIXEL_NUMBER 259200
#define YUV_SIZE (PIXEL_NUMBER * 3 / 2)
#define YUV_FRAME_SIZE 36
#define PIXEL_PER_FRAME 24
#define RGB_SIZE (PIXEL_NUMBER * 3)

#define RANGE(x, a, b) (x < a ? a : ( x > b ? b : x))
#define UV_INDEX(y) ((y / 12) * 3 + (y % 6) / 2)

void yuv_to_rgb(unsigned char *yuv_image, unsigned char *rgb_image);

#endif //FADEINFADEOUTYUV_LIB_H
