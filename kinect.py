from threading import Lock

import PIL
import freenect
import numpy as np
from PIL.Image import Image

__mutex = Lock()
__ctx = freenect.init()


def set_tilt(degrees: float) -> None:
    if not 0 <= degrees <= 15:
        raise ValueError(f"Tilt angle {degrees} must be between 0 and 15 degrees")
    with __mutex:
        dev = freenect.open_device(__ctx, 0)
        freenect.set_tilt_degs(dev, degrees)
        freenect.close_device(dev)


def get_depth() -> tuple[Image, int]:
    with __mutex:
        data, timestamp = freenect.sync_get_depth()  # (480, 640) uint16
        data = data.astype(float)
        data /= 2048.0
        data *= 255.0
        data = data.astype(np.uint8)
        depth = PIL.Image.fromarray(data, mode="L")
        return depth, timestamp


def get_video() -> tuple[Image, int]:
    with __mutex:
        data, timestamp = freenect.sync_get_video()  # (480, 640, 3) uint8
        image = PIL.Image.fromarray(data, mode="RGB")
        return image, timestamp
