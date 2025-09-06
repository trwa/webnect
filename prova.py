import threading

import cv2
import numpy as np

import kinect

def loop():
    cv2.namedWindow('image')
    cv2.namedWindow('depth')
    while True:
        image, depth = kinect.get_frame()
        if image is None or depth is None:
            continue
        img = cv2.cvtColor(np.array(image), cv2.COLOR_RGB2BGR)
        dpt = np.array(depth)
        cv2.imshow('image', img)
        cv2.imshow('depth', dpt)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cv2.destroyAllWindows()


if __name__ == "__main__":
    kinect.start()
    loop()
    kinect.stop()
