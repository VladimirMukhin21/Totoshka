# pip3 install opencv-python qrcode pyzbar numpy

import cv2
from pyzbar import pyzbar
from datetime import datetime
import time

CAMERA_NUM = 0 # номер камеры
FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
RED = (0,0,255)
GREEN = (0,255,0)
BLUE = (255,0,0)
BLACK = (0,0,0)
QR_OFF = ""
QR_CV2 = "C"
QR_PYZBAR = "P"

def decode(image):    
    if qrMode == QR_CV2:
        cv2Decode(image)
    elif qrMode == QR_PYZBAR:
        pyzbarDecode(image)
    else:
        return
    cv2.putText(image, qrMode, (x(375), y(12)), FONT, 0.5, color=RED)

def pyzbarDecode(image):
    try:
        decoded_objects = pyzbar.decode(image)
    except:
        print("Ошибка распознавания QR-кода.")
        return
    for decoded in decoded_objects:
        qr = decoded.data.decode()

        n_points = len(decoded.polygon)
        for i in range(n_points):
            image = cv2.line(image, decoded.polygon[i], decoded.polygon[(i+1) % n_points], RED, thickness=3)
        cv2.putText(image, qr, (decoded.rect.left, decoded.rect.top-10), FONT, 1, RED)

        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))

def cv2Decode(image):
    try:
        qr, bbox, _ = cv2Decoder.detectAndDecode(image)
    except:
        print("Ошибка распознавания QR-кода.")
        return
    if qr:
        rect = bbox[0]
        thickness=3
        cv2.line(image, (int(rect[0][0]), int(rect[0][1])), (int(rect[1][0]), int(rect[1][1])), RED, thickness)
        cv2.line(image, (int(rect[1][0]), int(rect[1][1])), (int(rect[2][0]), int(rect[2][1])), RED, thickness)
        cv2.line(image, (int(rect[2][0]), int(rect[2][1])), (int(rect[3][0]), int(rect[3][1])), RED, thickness)
        cv2.line(image, (int(rect[3][0]), int(rect[3][1])), (int(rect[0][0]), int(rect[0][1])), RED, thickness)
        x = int(min(rect[0][0], rect[1][0], rect[2][0], rect[3][0]))
        y = int(min(rect[0][1], rect[1][1], rect[2][1], rect[3][1]))
        cv2.putText(image, qr, (x, y-10), FONT, 1, RED)

        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))

def x(x):
    return int(width*x/400)
def y(y):
    return int(height*y/300)

def draw_guides(image):
    thickness=1
    if guideMode == 1:
        cv2.line(image, (x(247), y(300)), (x(227), y(240)), GREEN, thickness) # центр
        cv2.line(image, (x(112), y(300)), (x(150), y(230)), GREEN, thickness) # лев
        cv2.line(image, (x(386), y(300)), (x(300), y(230)), GREEN, thickness) # прав
        cv2.line(image, (x(142), y(297)), (x(351), y(264)), BLUE, thickness) # горизонт

def draw_captured_qrs(image):
    if not showCapturedQrs:
        return
    if len(capturedQrs) == 0:
        return
    size = 0.7
    index = 0
    step = 15
    cv2.putText(image, "QR-коды:", (x(5), y((index+1)*step)), FONT, size, GREEN)
    for qr in capturedQrs:
        index += 1
        text = str.format("{0}. {1}", index, qr)
        cv2.putText(image, text, (x(5), y((index+1)*step)), FONT, size, GREEN)

def switch_record():
    global file
    global recording
    recording = not recording
    if recording:
        name = datetime.now().strftime("%Y%m%d_%H%M%S") + ".avi"
        codec = cv2.VideoWriter_fourcc(*'DIVX')
        # codec = cv2.VideoWriter_fourcc(*'XVID')
        # codec = cv2.VideoWriter_fourcc(*'MJPG') # файл больше раза в 2
        file = cv2.VideoWriter(name, codec, 20, (width, height))
    else:
        file.release()

def record(image):
    if recording:
        file.write(image)
        cv2.circle(image, (x(390), y(10)), radius=3, color=RED, thickness=-1)

HELP = []
HELP.append("shift-9: переподключить камеру")
HELP.append("shift-0: запись в файл")
HELP.append("q: захваченные QR-коды")
HELP.append("w: захват cv2")
HELP.append("e: захват pyzbar")
HELP.append("r: выкл захват")
HELP.append("1: вкл гайдлайны")
HELP.append("2: выкл гайдлайны")
HELP.append("h: справка")
HELP.append("esc: завершить работу")

def draw_help(image):
    if not showHelp:
        return
    cv2.rectangle(
        image,
        (x(0), y(60)),
        (x(270), y(270)),
        color = GREEN,
        thickness = -1)
    for i in range(len(HELP)):
        cv2.putText(image, HELP[i], (x(5), y(60+(i+1)*20)), FONT, 0.7, BLACK)

def draw_scale(image):
    now = datetime.now()
    delta = now - scaleChangedTime
    if (delta.total_seconds() > 1):
        return
    msg = str(scale) + "%"
    cv2.putText(image, msg, (x(180), y(140)), FONT, 1, GREEN)

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
showCapturedQrs = True
scaleChangedTime = datetime.now()
recording = False
showHelp = False
cv2Decoder = cv2.QRCodeDetector()
qrMode = QR_OFF

if __name__ == "__main__":
    cap = cv2.VideoCapture()
    cap.open(CAMERA_NUM)

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    scale = 100
    size = (width, height)
    # change_scale(0)
    # print(width, "x", height)

    while True:
        readed, frame = cap.read()

        if readed:
            decode(frame)
            
            # cv2.putText(frame, "Тотошка", (x(5),y(20)), font, 1, color=(0,255,0), thickness=1, lineType=cv2.LINE_AA)
            draw_guides(frame)
            draw_captured_qrs(frame)
            record(frame)

            draw_help(frame)
            draw_scale(frame)
            frame = cv2.resize(frame, size, interpolation=cv2.INTER_AREA)
            cv2.imshow("TOTOSHKA", frame)
        else:
            cap.release()
            cap.open(CAMERA_NUM)
        
        key = cv2.waitKey(1)
        # print(key)
        if key >= ord("0") and key <= ord("9"):
            guideMode = key - ord("0")
            # print("guideMode:", guideMode)
        if key == ord("+"):
            change_scale(10)
        if key == ord("-"):
            change_scale(-10)
        if key == ord("q"):
            showCapturedQrs = not showCapturedQrs
        if key == ord("w"):
            qrMode = QR_CV2
        if key == ord("e"):
            qrMode = QR_PYZBAR
        if key == ord("r"):
            qrMode = QR_OFF
        if key == 41: # shift-0
            switch_record()
        if key == ord("h"):
            showHelp = not showHelp
        if key == 40: # shift-9
            cap.release()
            cap.open(CAMERA_NUM)
        if key == 27: # Esc => exit
            break

    if recording:
        file.release()
    cap.release()
    cv2.destroyAllWindows()
