import queue
import threading

import PIL
import freenect
import numpy as np
from PIL.Image import Image

__image: Image | None = None
__depth: Image | None = None

__qframe = queue.Queue()
__qangle = queue.Queue()

__task: threading.Thread | None = None
__stop = False


def __cb_video(dev, data, timestamp):
    frame = PIL.Image.fromarray(data, mode="RGB")
    __qframe.put(("video", frame, timestamp))


def __cb_depth(dev, data, timestamp):
    frame = PIL.Image.fromarray((data.astype(float) * 255.0 / 2048.0).astype(np.uint8), mode="L")
    __qframe.put(("depth", frame, timestamp))


def __cb_angle(dev, ctx):
    if __stop:
        raise freenect.Kill

    try:
        angle = __qangle.get_nowait()
        if not -15 <= angle <= 15:
            raise ValueError(f"Tilt angle {angle} must be between -15 and 15 degrees")
        freenect.set_tilt_degs(dev, angle)
    except (queue.Empty, ValueError):
        pass


def set_angle(degrees: float) -> None:
    __qangle.put(degrees)


def get_frame() -> tuple[Image, Image]:
    global __image, __depth
    typ, data, timestamp = __qframe.get()
    match typ:
        case "video":
            __image = data
        case "depth":
            __depth = data
    return __image, __depth


def start() -> None:
    global __task

    def run():
        ctx = freenect.init()
        dev = freenect.open_device(ctx, 0)
        freenect.set_depth_mode(dev, freenect.RESOLUTION_MEDIUM, freenect.DEPTH_REGISTERED)
        freenect.runloop(__cb_depth, __cb_video, __cb_angle, dev)

    __task = threading.Thread(target=run)
    __task.start()


def stop() -> None:
    global __task, __stop
    __stop = True
    __task.join()
    __task = None
