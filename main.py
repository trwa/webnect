from typing import Literal

from fastapi import FastAPI
from pydantic import BaseModel

import kinect

app = FastAPI()


class TiltMsg(BaseModel):
    angle: float


class DataMsg(BaseModel):
    type: Literal['video', 'depth']
    data: list[list[int]]
    timestamp: int


class StatusMsg(BaseModel):
    status: str
    message: str | None = None


@app.get("/")
async def root() -> StatusMsg:
    return StatusMsg(status="OK", message="Kinect server is running")


@app.post("/tilt")
async def set_tilt(request: TiltMsg) -> StatusMsg:
    try:
        kinect.set_tilt(request.angle)
        return StatusMsg(status="OK", message=f"Tilt set to {request.angle} degrees")
    except ValueError as e:
        return StatusMsg(status="Error", message=str(e))


@app.get("/tilt")
async def get_tilt() -> TiltMsg:
    angle = kinect.get_tilt()
    return TiltMsg(angle=angle)


@app.get("/data")
def get_data() -> DataMsg:
    data = kinect.get_data()
    match data:
        case kinect.KinectDataVideo(_, data, timestamp):
            return DataMsg(type="video", data=data.tolist(), timestamp=timestamp)
        case kinect.KinectDataDepth(_, data, timestamp):
            return DataMsg(type="depth", data=data.tolist(), timestamp=timestamp)
        case _:
            raise ValueError("Unknown data type")


@app.post("/start")
async def start_kinect() -> StatusMsg:
    kinect.start(0)
    return StatusMsg(status="OK", message="Kinect backend started")


@app.post("/stop")
async def stop_kinect() -> StatusMsg:
    kinect.stop()
    return StatusMsg(status="OK", message="Kinect backend stopped")
