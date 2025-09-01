import queue
import time

import PIL
import freenect
import numpy as np
from PIL.Image import Image

__video = queue.Queue()
__depth = queue.Queue()
__angle = queue.Queue()


def __cb_video(dev, data, timestamp):
    image = PIL.Image.fromarray(data, mode="RGB")
    __video.put((image, timestamp))


def __cb_depth(dev, data, timestamp):
    data = data.astype(float)
    data /= 2048.0
    data *= 255.0
    data = data.astype(np.uint8)
    depth = PIL.Image.fromarray(data, mode="L")
    __depth.put((depth, timestamp))


def __cb_body(dev, ctx):
    try:
        angle = __angle.get_nowait()
        if not 0 <= angle <= 15:
            raise ValueError(f"Tilt angle {angle} must be between 0 and 15 degrees")
        freenect.set_tilt_degs(dev, angle)
    except (queue.Empty, ValueError):
        pass


def set_tilt(degrees: float) -> None:
    __angle.put(degrees)


def get_depth() -> tuple[Image, int]:
    return __depth.get()


def get_video() -> tuple[Image, int]:
    return __video.get()


def start() -> None:
    ctx = freenect.init()
    dev = freenect.open_device(ctx, 0)
    freenect.set_depth_mode(dev, freenect.RESOLUTION_MEDIUM, freenect.DEPTH_REGISTERED)
    freenect.runloop(__cb_depth, __cb_video, __cb_body, dev)
