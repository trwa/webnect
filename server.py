import asyncio
import threading
from io import BytesIO

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import StreamingResponse, HTMLResponse

import kinect
from message import StatusMessage, TiltMessage

# Start Kinect capture in background thread
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
    image.save(buff, format="PNG")  # Keep PNG
    buff.seek(0)
    return StreamingResponse(buff, media_type="image/png", headers={"X-Timestamp": str(timestamp)})


@app.get("/depth")
def get_depth():
    depth, timestamp = kinect.get_depth()
    buff = BytesIO()
    depth.save(buff, format="PNG")
    buff.seek(0)
    return StreamingResponse(buff, media_type="image/png", headers={"X-Timestamp": str(timestamp)})


@app.websocket("/ws/video")
async def ws_video(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            # Block until the next frame is available
            image, timestamp = kinect.get_video()

            # Encode as PNG
            buff = BytesIO()
            image.save(buff, format="JPEG", quality=80)
            buff.seek(0)
            await websocket.send_bytes(buff.getvalue())
            await asyncio.sleep(0.01)
    except WebSocketDisconnect:
        print("Client disconnected")
    except Exception as e:
        print(f"WebSocket error: {e}")


@app.websocket("/ws/depth")
async def ws_depth(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            # Block until the next depth frame is available
            depth, timestamp = kinect.get_depth()

            # Encode as PNG
            buff = BytesIO()
            depth.save(buff, format="JPEG", quality=80)
            buff.seek(0)
            await websocket.send_bytes(buff.getvalue())
            await asyncio.sleep(0.01)
    except WebSocketDisconnect:
        print("Client disconnected")
    except Exception as e:
        print(f"WebSocket error: {e}")
