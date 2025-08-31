from pydantic import BaseModel


class TiltMessage(BaseModel):
    angle: float


class DepthMessage(BaseModel):
    rows: int
    cols: int
    data: bytes
    timestamp: int


class StatusMessage(BaseModel):
    status: str
    message: str | None = None
