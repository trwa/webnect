import dataclasses
import queue
import threading
from typing import Annotated, Literal

import freenect
import numpy.typing as npt
from numpy import uint8, uint16

########################################################################################################################
# Queues
########################################################################################################################

_data_queue: queue.Queue = queue.Queue()
_set_tilt_queue: queue.Queue = queue.Queue()
_get_tilt_queue: queue.Queue = queue.Queue()
_get_tilt_request = threading.Event()

_stop_event = threading.Event()


########################################################################################################################
# Data Classes
########################################################################################################################

@dataclasses.dataclass
class KinectDataDepth:
    dev: freenect.DevPtr
    data: Annotated[npt.NDArray[uint16], Literal[480, 640]]
    timestamp: int


@dataclasses.dataclass
class KinectDataVideo:
    dev: freenect.DevPtr
    data: Annotated[npt.NDArray[uint8], Literal[480, 640, 3]]
    timestamp: int


########################################################################################################################
# Callbacks
########################################################################################################################

def __onvideo(dev, data, timestamp) -> None:
    kdv = KinectDataVideo(dev, data, timestamp)
    _data_queue.put(kdv)


def __ondepth(dev, data, timestamp) -> None:
    kdd = KinectDataDepth(dev, data, timestamp)
    _data_queue.put(kdd)


########################################################################################################################
# Worker thread
########################################################################################################################

def _worker(device_index: int = 0) -> None:
    ctx = None
    dev = None
    try:
        ctx = freenect.init()
        num_devices = freenect.num_devices(ctx)
        if device_index >= num_devices:
            raise RuntimeError(f"Device index {device_index} out of range {num_devices}")

        dev = freenect.open_device(ctx, device_index)
        if not dev:
            raise RuntimeError(f"Failed to open device {device_index}")

        freenect.set_depth_callback(dev, __ondepth)
        freenect.set_video_callback(dev, __onvideo)
        freenect.start_depth(dev)
        freenect.start_video(dev)

        while not _stop_event.is_set():
            freenect.process_events(ctx)

            # Handle tilt set requests
            try:
                angle = _set_tilt_queue.get_nowait()
                freenect.set_tilt_degs(dev, angle)
            except queue.Empty:
                pass

            # Handle tilt get requests
            if _get_tilt_request.is_set():
                tilt = freenect.get_tilt_state(dev)
                degs = freenect.get_tilt_degs(tilt)
                _get_tilt_request.clear()
                _get_tilt_queue.put(degs)

    finally:
        if dev:
            freenect.close_device(dev)
        if ctx:
            freenect.shutdown(ctx)


########################################################################################################################
# Public API
########################################################################################################################

_thread: threading.Thread | None = None


def start(device_index: int = 0) -> None:
    global _thread
    if _thread and _thread.is_alive():
        return
    _stop_event.clear()
    _thread = threading.Thread(target=_worker, args=(device_index,), daemon=True)
    _thread.start()


def stop() -> None:
    _stop_event.set()
    if _thread:
        _thread.join()


def get_data() -> KinectDataVideo | KinectDataDepth:
    """Get the next video/depth frame."""
    data = _data_queue.get(block=True, timeout=None)
    return data


def set_tilt(angle: float) -> None:
    """Request the device tilt to be set."""
    if 0 <= angle <= 15:
        _set_tilt_queue.put(angle)
    else:
        raise ValueError("Angle must be between 0 and 15 degrees")


def get_tilt() -> float:
    """Request the current tilt angle."""
    _get_tilt_request.set()
    return _get_tilt_queue.get(block=True, timeout=None)
