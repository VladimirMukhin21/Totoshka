# pip3 install opencv-python qrcode pyzbar numpy

import cv2
from pyzbar import pyzbar
from datetime import datetime

FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
RED = (0,0,255)
GREEN = (0,255,0)
BLUE = (255,0,0)

def draw_barcode(decoded, image, text):
    n_points = len(decoded.polygon)
    for i in range(n_points):
        image = cv2.line(image, decoded.polygon[i], decoded.polygon[(i+1) % n_points], RED, thickness=3)
    # cv2.rectangle(
    #     image,
    #     (decoded.rect.left, decoded.rect.top), 
    #     (decoded.rect.left + decoded.rect.width, decoded.rect.top + decoded.rect.height),
    #     color = (0, 0, 255),
    #     thickness = 2)
    cv2.putText(image, text, (decoded.rect.left, decoded.rect.top-10), FONT, 1, RED)

def decode(image):
    decoded_objects = pyzbar.decode(image)
    for obj in decoded_objects:
        qr = obj.data.decode()
        draw_barcode(obj, image, qr)
        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))

def x(x):
    return int(width*scale*x/400/100)
def y(y):
    return int(height*scale*y/300/100)

def draw_guides(image):
    thickness=1
    match guideMode:
        case 1:
            # cv2.putText(image, "guides mode: 1", (x(5),y(10)), font, 1, GREEN, thickness=1, lineType=cv2.LINE_4)
            cv2.line(image, (x(30), y(250)), (x(370), y(250)), GREEN, thickness) # горизонт
            cv2.line(image, (x(200), y(240)), (x(200), y(300)), GREEN, thickness) # центр верт
            cv2.line(image, (x(0), y(300)), (x(70), y(230)), GREEN, thickness) # наклон лев
            cv2.line(image, (x(400), y(300)), (x(330), y(230)), GREEN, thickness) # наклон прав

def draw_captured_qrs(image):
    if not printCapturedQrs:
        return
    if len(capturedQrs) == 0:
        return
    size = 0.7
    index = 0
    cv2.putText(image, "QR-коды:", (5,(index+1)*20), FONT, size, GREEN)
    for qr in capturedQrs:
        index += 1
        text = str.format("{0}. {1}", index, qr) #str(index) + ". " + qr
        cv2.putText(image, text, (5,(index+1)*20), FONT, 0.7, GREEN)

def draw_scale(image):
    now = datetime.now()
    delta = now - scaleChangedTime
    if (delta.total_seconds() > 1):
        return
    msg = str(scale) + "%"
    cv2.putText(image, msg, (x(180),y(140)), FONT, 1, GREEN)

def change_scale(delta):
    global scale
    global size
    global scaleChangedTime
    scale += delta
    scale = max(scale, 30)  # ограничение минимального масштаба
    scale = min(scale, 200) # ограничение максимального масштаба
    size = (int(width*scale/100), int(height*scale/100))
    scaleChangedTime = datetime.now()
    print("scale:", scale, "%,  image size:", size[0], "x", size[1])

capturedQrs = set()
guideMode = 1
printCapturedQrs = True
scaleChangedTime = datetime.now()

if __name__ == "__main__":
    cap = cv2.VideoCapture()
    cap.open(0) # номер камеры
    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    scale = 100
    change_scale(0)
    # print(width, "x", height)
    while True:
        readed, frame = cap.read()
        if readed:
            decode(frame)
            frame = cv2.resize(frame, size, interpolation=cv2.INTER_AREA)
            draw_guides(frame)
            draw_captured_qrs(frame)
            draw_scale(frame)
            # cv2.putText(frame, "Тотошка", (x(5),y(20)), font, 1, color=(0,255,0), thickness=1, lineType=cv2.LINE_AA)
            cv2.imshow("TOTOSHKA", frame)
        key = cv2.waitKey(1)
        # print(key)
        if key >= ord("0") and key <= ord("9"):
            guideMode = key - ord("0")
            print("guideMode:", guideMode)
        if key == ord("+"):
            change_scale(10)
        if key == ord("-"):
            change_scale(-10)
        if key == ord("q"):
            printCapturedQrs = not printCapturedQrs
        if key == 27: # Esc => exit
            break
