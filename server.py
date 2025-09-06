import threading
from io import BytesIO

from PIL import Image
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse

import kinect
from message import StatusMessage, TiltMessage

# Start Kinect capture in background thread
threading.Thread(target=kinect.__start, daemon=True).start()

app = FastAPI()


@app.get("/")
async def root():
    with open("public/index.html", "r") as file:
        index = file.read()
    return HTMLResponse(index)


@app.post("/tilt")
async def set_tilt(request: TiltMessage) -> StatusMessage:
    try:
        kinect.set_angle(request.angle)
        return StatusMessage(status="OK", message=f"Tilt set to {request.angle} degrees")
    except ValueError as e:
        return StatusMessage(status="Error", message=str(e))


@app.websocket("/ws/video")
async def ws_video(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            image, depth = kinect.get_frame()

            # Merge image and depth side by side
            combined_width = image.width + depth.width
            combined_height = max(image.height, depth.height)
            combined_image = Image.new("RGB", (combined_width, combined_height))
            combined_image.paste(image, (0, 0))
            depth_rgb = depth.convert("RGB")
            combined_image.paste(depth_rgb, (image.width, 0))
            image = combined_image
            # Convert to JPEG and send over WebSocket
            buff = BytesIO()
            image.save(buff, format="JPEG", quality=80)
            buff.seek(0)
            await websocket.send_bytes(buff.getvalue())
    except WebSocketDisconnect:
        print("Client disconnected")
    except Exception as e:
        print(f"WebSocket error: {e}")
