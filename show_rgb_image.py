import sys
import numpy as np
from PIL import Image

IMAGE_WIDTH = 1920
IMAGE_HEIGHT = 1080

rgb_file = sys.argv[1]
np_array = np.fromfile(rgb_file, dtype=np.uint8).reshape(IMAGE_HEIGHT, IMAGE_WIDTH, 3)
image = Image.fromarray(np_array)
image.show()
