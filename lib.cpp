//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#include "lib.h"

void yuv_to_rgb(unsigned char *yuv_image, unsigned char *rgb_image) {
    for (int frame_no = 0; frame_no < YUV_SIZE / YUV_FRAME_SIZE; frame_no++) {
        for (int pixel_no = 0; pixel_no < PIXEL_PER_FRAME; pixel_no++) {
            float _y = (float) yuv_image[frame_no * YUV_FRAME_SIZE + pixel_no];
            float _u = (float) yuv_image[frame_no * YUV_FRAME_SIZE + 24 + UV_INDEX(pixel_no)];
            float _v = (float) yuv_image[frame_no * YUV_FRAME_SIZE + 30 + UV_INDEX(pixel_no)];

            float _r = 1.164383 * (_y - 16) + 1.596027 * (_v - 128);
            float _g = 1.164383 * (_y - 16) - 0.391762 * (_u - 128) - 0.812968 * (_v - 128);
            float _b = 1.164383 * (_y - 16) + 2.017232 * (_u - 128);

            rgb_image[frame_no * PIXEL_PER_FRAME * 3 + pixel_no * 3] = (unsigned char) RANGE(_r, 0, 255);
            rgb_image[frame_no * PIXEL_PER_FRAME * 3 + pixel_no * 3 + 1] = (unsigned char) RANGE(_g, 0, 255);
            rgb_image[frame_no * PIXEL_PER_FRAME * 3 + pixel_no * 3 + 2] = (unsigned char) RANGE(_b, 0, 255);
        }
    }
}
