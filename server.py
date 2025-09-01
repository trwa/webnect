import threading
from io import BytesIO

from fastapi import FastAPI
from fastapi.responses import StreamingResponse
from starlette.responses import HTMLResponse

import kinect
from message import StatusMessage, TiltMessage

threading.Thread(target=kinect.start, daemon=True).start()
app = FastAPI()


@app.get("/")
async def root():
    with open("public/index.html", "r") as file:
        index = file.read()
    return HTMLResponse(index)


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
