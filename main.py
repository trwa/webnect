from io import BytesIO

from fastapi import FastAPI
from fastapi.responses import StreamingResponse

import kinect
from message import StatusMessage, TiltMessage

app = FastAPI()


@app.get("/")
async def root() -> StatusMessage:
    return StatusMessage(status="OK", message="Kinect server is running")


@app.post("/tilt")
async def set_tilt(request: TiltMessage) -> StatusMessage:
    try:
        kinect.set_tilt(request.angle)
        return StatusMessage(status="OK", message=f"Tilt set to {request.angle} degrees")
    except ValueError as e:
        return StatusMessage(status="Error", message=str(e))


@app.get("/image")
def get_image():
    image, timestamp = kinect.get_video()
    buff = BytesIO()
    image.save(buff, format="PNG")
    buff.seek(0)
    return StreamingResponse(buff, media_type="image/png")


@app.get("/depth")
def get_depth():
    depth, timestamp = kinect.get_depth()
    buff = BytesIO()
    depth.save(buff, format="PNG")
    buff.seek(0)
    return StreamingResponse(buff, media_type="image/png")
