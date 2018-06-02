import sys
import numpy as np
from PIL import Image

IMAGE_WIDTH = 1920
IMAGE_HEIGHT = 1080
PIXEL_NUM = IMAGE_WIDTH * IMAGE_HEIGHT

rgb_file = sys.argv[1]
np_array = np.fromfile(rgb_file, dtype=np.uint8)
y_array = np_array[:PIXEL_NUM].astype(np.float32)
u_array = np_array[PIXEL_NUM:PIXEL_NUM + PIXEL_NUM // 4].astype(np.float32).reshape(IMAGE_HEIGHT // 2, IMAGE_WIDTH // 2)
v_array = np_array[PIXEL_NUM + PIXEL_NUM // 4:].astype(np.float32).reshape(IMAGE_HEIGHT // 2, IMAGE_WIDTH // 2)
u_array = u_array.repeat(2, axis=0).repeat(2, axis=1).reshape(-1)
v_array = v_array.repeat(2, axis=0).repeat(2, axis=1).reshape(-1)

r_array = 1.164383 * (y_array - 16) + 1.596027 * (v_array - 128)
g_array = 1.164383 * (y_array - 16) - 0.391762 * (u_array - 128) - 0.812968 * (v_array - 128)
b_array = 1.164383 * (y_array - 16) + 2.017232 * (u_array - 128)
r_array = np.clip(r_array, 0, 255)
g_array = np.clip(g_array, 0, 255)
b_array = np.clip(b_array, 0, 255)

cat_array = np.concatenate((r_array.reshape(1, -1), g_array.reshape(1, -1), b_array.reshape(1, -1)))
rgb_array = np.transpose(cat_array).astype(np.uint8).reshape(IMAGE_HEIGHT, IMAGE_WIDTH, 3)
image = Image.fromarray(rgb_array)
image.show()
