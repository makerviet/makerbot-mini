import urllib.request
import cv2
import numpy as np

CAM_URL = "http://192.168.4.1:80"
stream = urllib.request.urlopen(CAM_URL)
bytes = bytes()

while True:

    bytes += stream.read(1024)
    a = bytes.find(b'\xff\xd8')
    b = bytes.find(b'\xff\xd9')

    if a != -1 and b != -1:
        image = bytes[a:b+2]
        bytes = bytes[b+2:]

        try:
            image = cv2.imdecode(np.frombuffer(
                image, dtype=np.uint8), cv2.IMREAD_COLOR)
        except:
            continue

        cv2.imshow("Image", image)
        if cv2.waitKey(1) == 27:
            exit(0)
