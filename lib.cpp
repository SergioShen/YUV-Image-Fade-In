//
// Name: lib
// Project: FadeInFadeOutYUV
// Author: Shen Sijie
// Date: 5/27/18
//

#include "lib.h"
#include <cstdio>

int uv_index[PIXEL_NUMBER];

void initialize_uv_index_array() {
    for (int p = 0; p < PIXEL_NUMBER; p += 2 * IMAGE_WIDTH) {
        uv_index[p] = p / 4;
        uv_index[p + 1] = p / 4;
        uv_index[p + IMAGE_WIDTH] = p / 4;
        uv_index[p + IMAGE_WIDTH + 1] = p / 4;
    }
    for (int h = 0; h < IMAGE_HEIGHT; h++) {
        for (int w = 2; w < IMAGE_WIDTH; w += 2) {
            uv_index[h * IMAGE_WIDTH + w] = uv_index[h * IMAGE_WIDTH + w - 2] + 1;
            uv_index[h * IMAGE_WIDTH + w + 1] = uv_index[h * IMAGE_WIDTH + w - 2] + 1;
        }
    }
}

void yuv_to_rgb_base(unsigned char *yuv_image, unsigned char *rgb_image) {
    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no++) {
        float _y = (float) yuv_image[pixel_no];
        float _u = (float) yuv_image[PIXEL_NUMBER + uv_index[pixel_no]];
        float _v = (float) yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no]];

        float _r = 1.164383 * (_y - 16) + 1.596027 * (_v - 128);
        float _g = 1.164383 * (_y - 16) - 0.391762 * (_u - 128) - 0.812968 * (_v - 128);
        float _b = 1.164383 * (_y - 16) + 2.017232 * (_u - 128);

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

        float _y = 0.256788 * _r + 0.504129 * _g + 0.097906 * _b + 16;
        float _u = -0.148223 * _r - 0.290993 * _g + 0.439216 * _b + 128;
        float _v = 0.439216 * _r - 0.367788 * _g - 0.071427 * _b + 128;

        yuv_image[pixel_no] = (unsigned char) RANGE(_y, 0, 255);
        yuv_image[PIXEL_NUMBER + uv_index[pixel_no]] = (unsigned char) RANGE(_u, 0, 255);
        yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no]] = (unsigned char) RANGE(_v, 0, 255);
    }
}

void apply_alpha_base(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no++) {
        dst_rgb_image[pixel_no * 3] = (unsigned char) ((int) src_rgb_image[pixel_no * 3] * (int) alpha / 256);
        dst_rgb_image[pixel_no * 3 + 1] = (unsigned char) ((int) src_rgb_image[pixel_no * 3 + 1] * (int) alpha / 256);
        dst_rgb_image[pixel_no * 3 + 2] = (unsigned char) ((int) src_rgb_image[pixel_no * 3 + 2] * (int) alpha / 256);
    }
}

#if defined(MMX)

typedef short v4hi __attribute__ ((vector_size(8)));

void yuv_to_rgb_mmx(unsigned char *yuv_image, unsigned char *rgb_image) {
    yuv_to_rgb_base(yuv_image, rgb_image);
}

void rgb_to_yuv_mmx(unsigned char *rgb_image, unsigned char *yuv_image) {
    rgb_to_yuv_base(rgb_image, yuv_image);
}

void apply_alpha_mmx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
    static short const_8[4] = {8, 0, 0, 0};
    v4hi src_vector, alpha_vector, result;

    // Initialize const vectors
    for (int i = 0; i < 4; i++) {
        ((short *) &alpha_vector)[i] = alpha;
    }

    // Process every byte
    for (int byte_no = 0; byte_no < PIXEL_NUMBER * 3; byte_no += 4) {
        ((short *) &src_vector)[0] = (short) src_rgb_image[byte_no];
        ((short *) &src_vector)[1] = (short) src_rgb_image[byte_no + 1];
        ((short *) &src_vector)[2] = (short) src_rgb_image[byte_no + 2];
        ((short *) &src_vector)[3] = (short) src_rgb_image[byte_no + 3];

        result = __builtin_ia32_pmullw(src_vector, alpha_vector);
        result = __builtin_ia32_psrlw(result, *(v4hi *) const_8);

        dst_rgb_image[byte_no] = (unsigned char) (((short *) (&result))[0]);
        dst_rgb_image[byte_no + 1] = (unsigned char) (((short *) (&result))[1]);
        dst_rgb_image[byte_no + 2] = (unsigned char) (((short *) (&result))[2]);
        dst_rgb_image[byte_no + 3] = (unsigned char) (((short *) (&result))[3]);
    }
}

#elif defined(SSE2)

typedef float v4sf __attribute__ ((vector_size(16)));
typedef short v8hi __attribute__ ((vector_size(16)));

void yuv_to_rgb_sse2(unsigned char *yuv_image, unsigned char *rgb_image) {
    static float const_16[4]{16.0, 16.0, 16.0, 16.0};
    static float const_128[4]{128.0, 128.0, 128.0, 128.0};
    static float const_1_164383[4]{1.164383, 1.164383, 1.164383, 1.164383};
    static float const_1_596027[4]{1.596027, 1.596027, 1.596027, 1.596027};
    static float const_0_391762[4]{0.391762, 0.391762, 0.391762, 0.391762};
    static float const_0_812968[4]{0.812968, 0.812968, 0.812968, 0.812968};
    static float const_2_017232[4]{2.017232, 2.017232, 2.017232, 2.017232};
    v4sf y_vector, u_vector, v_vector, r_vector, g_vector, b_vector, temp_1_vector, temp_2_vector, temp_3_vector;

    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no += 4) {
        for (int i = 0; i < 4; i++) {
            ((float *) &y_vector)[i] = (float) yuv_image[pixel_no + i];
            ((float *) &u_vector)[i] = (float) yuv_image[PIXEL_NUMBER + uv_index[pixel_no + i]];
            ((float *) &v_vector)[i] = (float) yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no + i]];
        }

        // Subtract const vector
        y_vector = __builtin_ia32_subps(y_vector, *(v4sf *) const_16);
        u_vector = __builtin_ia32_subps(u_vector, *(v4sf *) const_128);
        v_vector = __builtin_ia32_subps(v_vector, *(v4sf *) const_128);

        // R vector
        temp_1_vector = __builtin_ia32_mulps(y_vector, *(v4sf *) const_1_164383);
        temp_2_vector = __builtin_ia32_mulps(v_vector, *(v4sf *) const_1_596027);
        r_vector = __builtin_ia32_addps(temp_1_vector, temp_2_vector);

        // G vector
        temp_2_vector = __builtin_ia32_mulps(u_vector, *(v4sf *) const_0_391762);
        temp_3_vector = __builtin_ia32_mulps(v_vector, *(v4sf *) const_0_812968);
        g_vector = __builtin_ia32_subps(temp_1_vector, temp_2_vector);
        g_vector = __builtin_ia32_subps(g_vector, temp_3_vector);

        // B vector
        temp_2_vector = __builtin_ia32_mulps(u_vector, *(v4sf *) const_2_017232);
        b_vector = __builtin_ia32_addps(temp_1_vector, temp_2_vector);

        for (int i = 0; i < 4; i++) {
            rgb_image[(pixel_no + i) * 3] = (unsigned char) RANGE(((float *) &r_vector)[i], 0, 255);
            rgb_image[(pixel_no + i) * 3 + 1] = (unsigned char) RANGE(((float *) &g_vector)[i], 0, 255);
            rgb_image[(pixel_no + i) * 3 + 2] = (unsigned char) RANGE(((float *) &b_vector)[i], 0, 255);
        }
    }
}

void rgb_to_yuv_sse2(unsigned char *rgb_image, unsigned char *yuv_image) {
    static float const_16[4]{16.0, 16.0, 16.0, 16.0};
    static float const_128[4]{128.0, 128.0, 128.0, 128.0};
    static float const_0_256788[4]{0.256788, 0.256788, 0.256788, 0.256788};
    static float const_0_504129[4]{0.504129, 0.504129, 0.504129, 0.504129};
    static float const_0_097906[4]{0.097906, 0.097906, 0.097906, 0.097906};
    static float const_m_0_148223[4]{-0.148223, -0.148223, -0.148223, -0.148223};
    static float const_0_290993[4]{0.290993, 0.290993, 0.290993, 0.290993};
    static float const_0_439216[4]{0.439216, 0.439216, 0.439216, 0.439216};
    static float const_0_367788[4]{0.367788, 0.367788, 0.367788, 0.367788};
    static float const_0_071427[4]{0.071427, 0.071427, 0.071427, 0.071427};
    v4sf y_vector, u_vector, v_vector, r_vector, g_vector, b_vector, temp_vector;

    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no += 4) {
        for (int i = 0; i < 4; i++) {
            ((float *) &r_vector)[i] = (float) rgb_image[(pixel_no + i) * 3];
            ((float *) &g_vector)[i] = (float) rgb_image[(pixel_no + i) * 3 + 1];
            ((float *) &b_vector)[i] = (float) rgb_image[(pixel_no + i) * 3 + 2];
        }

        // Y vector
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_256788, r_vector);
        y_vector = __builtin_ia32_addps(temp_vector, *(v4sf *) const_16);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_504129, g_vector);
        y_vector = __builtin_ia32_addps(y_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_097906, b_vector);
        y_vector = __builtin_ia32_addps(y_vector, temp_vector);

        // U vector
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_m_0_148223, r_vector);
        u_vector = __builtin_ia32_addps(temp_vector, *(v4sf *) const_128);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_290993, g_vector);
        u_vector = __builtin_ia32_subps(u_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_439216, b_vector);
        u_vector = __builtin_ia32_addps(u_vector, temp_vector);

        // V vector
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_439216, r_vector);
        v_vector = __builtin_ia32_addps(temp_vector, *(v4sf *) const_128);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_367788, g_vector);
        v_vector = __builtin_ia32_subps(v_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps(*(v4sf *) const_0_071427, b_vector);
        v_vector = __builtin_ia32_subps(v_vector, temp_vector);

        for (int i = 0; i < 4; i++) {
            yuv_image[pixel_no + i] = (unsigned char) RANGE(((float *) &y_vector)[i], 0, 255);
            yuv_image[PIXEL_NUMBER + uv_index[pixel_no + i]] = (unsigned char) RANGE(((float *) &u_vector)[i], 0, 255);
            yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no + i]]
                    = (unsigned char) RANGE(((float *) &v_vector)[i], 0, 255);
        }
    }
}

void apply_alpha_sse2(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
    static short const_8[8] = {8, 0, 0, 0, 0, 0, 0, 0};
    v8hi src_vector, alpha_vector, result;

    // Initialize const vectors
    for (int i = 0; i < 8; i++) {
        ((short *) &alpha_vector)[i] = alpha;
    }

    // Process every byte
    for (int byte_no = 0; byte_no < PIXEL_NUMBER * 3; byte_no += 8) {
        for (int i = 0; i < 8; i++)
            ((short *) &src_vector)[i] = (short) src_rgb_image[byte_no + i];

        result = __builtin_ia32_pmullw128(src_vector, alpha_vector);
        result = __builtin_ia32_psrlw128(result, *(v8hi *) const_8);

        for (int i = 0; i < 8; i++)
            dst_rgb_image[byte_no + i] = (unsigned char) (((short *) (&result))[i]);
    }
}

#elif defined(AVX)

typedef float v8sf __attribute__ ((vector_size(32)));
typedef short v8hi __attribute__ ((vector_size(16)));
typedef short v16hi __attribute__ ((vector_size(32)));

void yuv_to_rgb_avx(unsigned char *yuv_image, unsigned char *rgb_image) {
    static float const_16[8]{16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0};
    static float const_128[8]{128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0};
    static float const_1_164383[8]{1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383, 1.164383};
    static float const_1_596027[8]{1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027, 1.596027};
    static float const_0_391762[8]{0.391762, 0.391762, 0.391762, 0.391762, 0.391762, 0.391762, 0.391762, 0.391762};
    static float const_0_812968[8]{0.812968, 0.812968, 0.812968, 0.812968, 0.812968, 0.812968, 0.812968, 0.812968};
    static float const_2_017232[8]{2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232, 2.017232};
    v8sf y_vector, u_vector, v_vector, r_vector, g_vector, b_vector, temp_1_vector, temp_2_vector, temp_3_vector;

    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no += 8) {
        for (int i = 0; i < 8; i++) {
            ((float *) &y_vector)[i] = (float) yuv_image[pixel_no + i];
            ((float *) &u_vector)[i] = (float) yuv_image[PIXEL_NUMBER + uv_index[pixel_no + i]];
            ((float *) &v_vector)[i] = (float) yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no + i]];
        }

        // Subtract const vector
        y_vector = __builtin_ia32_subps256(y_vector, *(v8sf *) const_16);
        u_vector = __builtin_ia32_subps256(u_vector, *(v8sf *) const_128);
        v_vector = __builtin_ia32_subps256(v_vector, *(v8sf *) const_128);

        // R vector
        temp_1_vector = __builtin_ia32_mulps256(y_vector, *(v8sf *) const_1_164383);
        temp_2_vector = __builtin_ia32_mulps256(v_vector, *(v8sf *) const_1_596027);
        r_vector = __builtin_ia32_addps256(temp_1_vector, temp_2_vector);

        // G vector
        temp_2_vector = __builtin_ia32_mulps256(u_vector, *(v8sf *) const_0_391762);
        temp_3_vector = __builtin_ia32_mulps256(v_vector, *(v8sf *) const_0_812968);
        g_vector = __builtin_ia32_subps256(temp_1_vector, temp_2_vector);
        g_vector = __builtin_ia32_subps256(g_vector, temp_3_vector);

        // B vector
        temp_2_vector = __builtin_ia32_mulps256(u_vector, *(v8sf *) const_2_017232);
        b_vector = __builtin_ia32_addps256(temp_1_vector, temp_2_vector);

        for (int i = 0; i < 8; i++) {
            rgb_image[(pixel_no + i) * 3] = (unsigned char) RANGE(((float *) &r_vector)[i], 0, 255);
            rgb_image[(pixel_no + i) * 3 + 1] = (unsigned char) RANGE(((float *) &g_vector)[i], 0, 255);
            rgb_image[(pixel_no + i) * 3 + 2] = (unsigned char) RANGE(((float *) &b_vector)[i], 0, 255);
        }
    }
}

void rgb_to_yuv_avx(unsigned char *rgb_image, unsigned char *yuv_image) {
    static float const_16[8]{16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0, 16.0};
    static float const_128[8]{128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0};
    static float const_0_256788[8]{0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788, 0.256788};
    static float const_0_504129[8]{0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129, 0.504129};
    static float const_0_097906[8]{0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906, 0.097906};
    static float const_m_0_148223[8]{-0.148223, -0.148223, -0.148223, -0.148223,
                                     -0.148223, -0.148223, -0.148223, -0.148223};
    static float const_0_290993[8]{0.290993, 0.290993, 0.290993, 0.290993, 0.290993, 0.290993, 0.290993, 0.290993};
    static float const_0_439216[8]{0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216, 0.439216};
    static float const_0_367788[8]{0.367788, 0.367788, 0.367788, 0.367788, 0.367788, 0.367788, 0.367788, 0.367788};
    static float const_0_071427[8]{0.071427, 0.071427, 0.071427, 0.071427, 0.071427, 0.071427, 0.071427, 0.071427};
    v8sf y_vector, u_vector, v_vector, r_vector, g_vector, b_vector, temp_vector;

    for (int pixel_no = 0; pixel_no < PIXEL_NUMBER; pixel_no += 8) {
        for (int i = 0; i < 8; i++) {
            ((float *) &r_vector)[i] = (float) rgb_image[(pixel_no + i) * 3];
            ((float *) &g_vector)[i] = (float) rgb_image[(pixel_no + i) * 3 + 1];
            ((float *) &b_vector)[i] = (float) rgb_image[(pixel_no + i) * 3 + 2];
        }

        // Y vector
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_256788, r_vector);
        y_vector = __builtin_ia32_addps256(temp_vector, *(v8sf *) const_16);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_504129, g_vector);
        y_vector = __builtin_ia32_addps256(y_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_097906, b_vector);
        y_vector = __builtin_ia32_addps256(y_vector, temp_vector);

        // U vector
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_m_0_148223, r_vector);
        u_vector = __builtin_ia32_addps256(temp_vector, *(v8sf *) const_128);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_290993, g_vector);
        u_vector = __builtin_ia32_subps256(u_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_439216, b_vector);
        u_vector = __builtin_ia32_addps256(u_vector, temp_vector);

        // V vector
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_439216, r_vector);
        v_vector = __builtin_ia32_addps256(temp_vector, *(v8sf *) const_128);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_367788, g_vector);
        v_vector = __builtin_ia32_subps256(v_vector, temp_vector);
        temp_vector = __builtin_ia32_mulps256(*(v8sf *) const_0_071427, b_vector);
        v_vector = __builtin_ia32_subps256(v_vector, temp_vector);

        for (int i = 0; i < 8; i++) {
            yuv_image[pixel_no + i] = (unsigned char) RANGE(((float *) &y_vector)[i], 0, 255);
            yuv_image[PIXEL_NUMBER + uv_index[pixel_no + i]] = (unsigned char) RANGE(((float *) &u_vector)[i], 0, 255);
            yuv_image[PIXEL_NUMBER + PIXEL_NUMBER / 4 + uv_index[pixel_no + i]]
                    = (unsigned char) RANGE(((float *) &v_vector)[i], 0, 255);
        }
    }
}

void apply_alpha_avx(unsigned char *src_rgb_image, unsigned char *dst_rgb_image, unsigned char alpha) {
#if defined(AVX2)
    static short const_8[16] = {8, 0, 0, 0, 0, 0, 0, 0};
    v16hi src_vector, alpha_vector, result;

    // Initialize const vectors
    for (int i = 0; i < 16; i++) {
        ((short *) &alpha_vector)[i] = alpha;
    }

    // Process every byte
    for (int byte_no = 0; byte_no < PIXEL_NUMBER * 3; byte_no += 16) {
        for (int i = 0; i < 16; i++)
            ((short *) &src_vector)[i] = (short) src_rgb_image[byte_no + i];

        result = __builtin_ia32_pmullw256(src_vector, alpha_vector);
        result = __builtin_ia32_psrlw256(result, *(v8hi *) const_8);

        for (int i = 0; i < 16; i++)
            dst_rgb_image[byte_no + i] = (unsigned char) (((short *) (&result))[i]);
    }
#else
    apply_alpha_base(src_rgb_image, dst_rgb_image, alpha);
#endif
}

#endif