# pip3 install opencv-python qrcode pyzbar numpy
# Выбор пути установки интерпретатора (например, если перестала находится установленная cv2):
# https://code.visualstudio.com/docs/python/environments#_working-with-python-interpreters
# Ctrl+Shift+P -> Python: Select Interpreter -> "C:\Users\<USER>\AppData\Local\Programs\Python\Python310\python.exe"

import cv2
from pyzbar import pyzbar
from datetime import datetime
import time
from threading import Thread
import winsound
import serial
import keyboard

CAMERA_NUM = 0 #1 + cv2.CAP_FFMPEG # номер камеры
PORT = "COM3" # 5 - левый, 3 - правый ближний, 6 - правый дальний
BAUDRATE = 9600
FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
RED = (0,0,255)
GREEN = (0,255,0)
BLUE = (255,0,0)
BLACK = (0,0,0)
YELLOW = (0,255,255)
QR_OFF = ""
QR_CV2 = "C"
QR_PYZBAR = "P"
EMPTY_STR = ""

def beep():
    frequency = 2500
    duration = 500
    t = Thread(target=winsound.Beep, args=(frequency, duration))
    t.start()

def decode(image):
    if qrMode == QR_CV2:
        cv2Decode(image)
    elif qrMode == QR_PYZBAR:
        pyzbarDecode(image)
    else:
        return
    cv2.putText(image, qrMode, (x(375), y(12)), FONT, 0.5, color=RED)

def pyzbarDecode(image):
    global qrDecodeTime
    global qr
    global pyzbar_box
    now = datetime.now()
    delta = now - qrDecodeTime
    if (delta.microseconds > 300000): # 300ms
        qrDecodeTime = now
        qr = EMPTY_STR
        try:
            decoded = pyzbar.decode(image)
            pyzbar_box = decoded.pop()
            qr = pyzbar_box.data.decode()
            if qr != EMPTY_STR:
                beep()
        except:
            qr = EMPTY_STR
            # print("Ошибка распознавания QR-кода.")
            return
        if qr == EMPTY_STR:
            return
        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))
    if qr == EMPTY_STR:
        return
    n_points = len(pyzbar_box.polygon)
    for i in range(n_points):
        image = cv2.line(image, pyzbar_box.polygon[i], pyzbar_box.polygon[(i+1) % n_points], RED, thickness=3)
    cv2.putText(image, qr, (pyzbar_box.rect.left, pyzbar_box.rect.top-10), FONT, 1, RED)

def cv2Decode(image):
    global qrDecodeTime
    global qr
    global cv2_box
    now = datetime.now()
    delta = now - qrDecodeTime
    if (delta.microseconds > 300000): # 300ms
        qrDecodeTime = now
        qr = EMPTY_STR
        try:
            qr, decoded, _ = cv2Decoder.detectAndDecode(image)
            cv2_box = decoded[0]
            if qr != EMPTY_STR:
                beep()
        except:
            qr = EMPTY_STR
            # print("Ошибка распознавания QR-кода.")
            return
        if qr == EMPTY_STR:
            return
        capturedQrs.add(qr)
        print("QR:", qr, "  HIST:",  ", ".join(capturedQrs))
    if qr == EMPTY_STR:
        return
    thickness=3
    cv2.line(image, (int(cv2_box[0][0]), int(cv2_box[0][1])), (int(cv2_box[1][0]), int(cv2_box[1][1])), RED, thickness)
    cv2.line(image, (int(cv2_box[1][0]), int(cv2_box[1][1])), (int(cv2_box[2][0]), int(cv2_box[2][1])), RED, thickness)
    cv2.line(image, (int(cv2_box[2][0]), int(cv2_box[2][1])), (int(cv2_box[3][0]), int(cv2_box[3][1])), RED, thickness)
    cv2.line(image, (int(cv2_box[3][0]), int(cv2_box[3][1])), (int(cv2_box[0][0]), int(cv2_box[0][1])), RED, thickness)
    x = int(min(cv2_box[0][0], cv2_box[1][0], cv2_box[2][0], cv2_box[3][0]))
    y = int(min(cv2_box[0][1], cv2_box[1][1], cv2_box[2][1], cv2_box[3][1]))
    cv2.putText(image, qr, (x, y-10), FONT, 1, RED)

def x(x):
    return int(width*x/400)
def y(y):
    return int(height*y/300)

def draw_guides(image):
    thickness=1
    if guideMode == 1:
        cv2.line(image, (x(190), y(280)), (x(190), y(20)), BLUE, thickness) # центр
        cv2.line(image, (x(131), y(170)), (x(154), y(120)), GREEN, thickness) # лев
        cv2.line(image, (x(249), y(170)), (x(227), y(120)), GREEN, thickness) # прав
        cv2.line(image, (x(100), y(170)), (x(280), y(170)), GREEN, thickness) # горизонт
    elif guideMode == 2:
        cv2.line(image, (x(247), y(300)), (x(227), y(240)), GREEN, thickness) # центр
        cv2.line(image, (x(112), y(300)), (x(150), y(230)), GREEN, thickness) # лев
        cv2.line(image, (x(386), y(300)), (x(300), y(230)), GREEN, thickness) # прав
        cv2.line(image, (x(142), y(297)), (x(351), y(264)), BLUE, thickness) # горизонт
    elif guideMode == 5:
        for i in range(0, 400, 100):
            cv2.line(image, (x(i), y(0)), (x(i), y(300)), GREEN, thickness)
        for i in range(0, 300, 100):
            cv2.line(image, (x(0), y(i)), (x(400), y(i)), GREEN, thickness)

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

def read_telemetry(image):
    if not readTelemetry:
        return
    global dist
    global clrl
    global clrr

    serialAvailable = ser.inWaiting()
    if serialAvailable:
        data = ser.readline().decode().strip()
        dist, clrl, clrr = data.split(',')

    size = 0.6
    firstStr = 100
    step = 15
    cv2.putText(image, "Датчики:", (x(333), y(firstStr)), FONT, size, GREEN)
    cv2.putText(image, "dist", (x(330), y(firstStr+step)), FONT, size, GREEN)
    cv2.putText(image, dist, (x(365), y(firstStr+step)), FONT, size, GREEN)
    cv2.putText(image, "clrl", (x(330), y(firstStr+2*step)), FONT, size, GREEN)
    cv2.putText(image, clrl, (x(365), y(firstStr+2*step)), FONT, size, GREEN)
    cv2.putText(image, "clrr", (x(330), y(firstStr+3*step)), FONT, size, GREEN)
    cv2.putText(image, clrr, (x(365), y(firstStr+3*step)), FONT, size, GREEN)

def connect_camera(num):
    print("Connecting to camera " + str(num) + "...")
    cap.release()
    CAMERA_NUM = num - 1
    cap.open(CAMERA_NUM)
    print("Connected to camera " + str(num))

def switch_record():
    global file
    global recording
    recording = not recording
    if recording:
        name = datetime.now().strftime("%Y%m%d_%H%M%S") + ".avi"
        codec = cv2.VideoWriter_fourcc(*'DIVX')
        # codec = cv2.VideoWriter_fourcc(*'XVID')
        # codec = cv2.VideoWriter_fourcc(*'MJPG') # файл больше раза в 2
        file = cv2.VideoWriter(name, codec, 25, (width, height))
    else:
        file.release()

def record(image):
    if recording:
        file.write(image)
        cv2.circle(image, (x(390), y(10)), radius=3, color=RED, thickness=-1)

HELP = []
HELP.append("shift-1/2/3: выбор камеры")
HELP.append("shift-8: таймер")
HELP.append("shift-0: запись в файл")
HELP.append("q: захваченные QR-коды")
HELP.append("w: захват cv2")
HELP.append("e: захват pyzbar")
HELP.append("r: выкл захват")
HELP.append("1: гайдлайны верхней камеры")
HELP.append("2: гайдлайны нижней камеры")
HELP.append("3: выкл гайдлайны")
HELP.append("h: справка")
HELP.append("esc: завершить работу")

def draw_help(image):
    if not showHelp:
        return
    cv2.rectangle(
        image,
        (x(0), y(40)),
        (x(270), y(290)),
        color = GREEN,
        thickness = -1)
    for i in range(len(HELP)):
        cv2.putText(image, HELP[i], (x(5), y(40+(i+1)*20)), FONT, 0.7, BLACK)

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

def clock(image):
    if not showClock:
        return
    
    global now
    global timeLeft
    pre = int(datetime.now().strftime("%M%S"))
    
    if timeLeft > 0 and pause == False:
        if pre != now:
            timeLeft -= 1
            now = pre
    min = timeLeft // 60
    sec = timeLeft % 60

    if min < 1:
        color = RED
    elif min < 5:
        color = YELLOW
    else:
        color = GREEN
    if min <= 9:
        min = str(0) + str(min)
    if sec <= 9:
        sec = str(0) + str(sec)

    timerMinSec = str(min) + ":" + str(sec)
    cv2.putText(image, timerMinSec, (x(367), y(35)), FONT, 0.5, color)

capturedQrs = set()
guideMode = 1  # 0
showCapturedQrs = True
scaleChangedTime = datetime.now()
recording = False
readTelemetry = False
showHelp = False
showClock = False
pause = False
cv2Decoder = cv2.QRCodeDetector()
qrMode = QR_OFF
qrDecodeTime = datetime.now()
cv2_box = []
pyzbar_box = []
qr = EMPTY_STR
dist = str(10)    # начальное значение датчика расстояния (нужно только для старта вывода телеметрии)
clrl = str(10)    # начальное значение датчика расстояния (нужно только для старта вывода телеметрии)
clrr = str(10)    # начальное значение датчика расстояния (нужно только для старта вывода телеметрии)
now = int(datetime.now().strftime("%M%S"))


if __name__ == "__main__":
    print("Connecting...")
    cap = cv2.VideoCapture()
    cap.open(CAMERA_NUM)
    ser = serial.Serial(PORT, BAUDRATE)
    print("Connected")
    ser.write(b'telemetry_stop')

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    scale = 130
    size = (width, height)
    change_scale(0)
    # print(width, "x", height)

    while True:
        readed, frame = cap.read()

        if readed:
            decode(frame)
            
            # cv2.putText(frame, "Тотошка", (x(5),y(20)), font, 1, color=(0,255,0), thickness=1, lineType=cv2.LINE_AA)
            draw_guides(frame)
            draw_captured_qrs(frame)
            read_telemetry(frame)
            draw_help(frame)
            draw_scale(frame)
            clock(frame)
            record(frame)
            frame = cv2.resize(frame, size, interpolation=cv2.INTER_LINEAR)
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
            qr = EMPTY_STR
        if key == ord("e"):
            qrMode = QR_PYZBAR
            qr = EMPTY_STR
        if key == ord("r"):
            qrMode = QR_OFF
            qr = EMPTY_STR
        if key == ord("l"):
            readTelemetry = not readTelemetry
            if (readTelemetry):
                ser.write(b'telemetry_start')
            else:
                ser.write(b'telemetry_stop')
        if key == 42: # shift-8
            showClock = not showClock
            timeLeft = 601  # 600sec = 10min, пишем время в секундах +1
        if key == ord("p"):
            pause = not pause
        if key == 41: # shift-0
            switch_record()
            showClock = not showClock
            timeLeft = 601  # 600sec = 10min, пишем время в секундах +1
        if key == ord("h"):
            showHelp = not showHelp
        if key == ord("!"): # shift-1
            connect_camera(1)
        if key == ord("@"): # shift-2
            connect_camera(2)
        if key == ord("#"): # shift-3
            connect_camera(3)
        if key == 27: # Esc => exit
            break

    if recording:
        file.release()
    ser.write(b'telemetry_stop')
    if ser.is_open:
        ser.close()
        print("Serial connection closed")
    cap.release()
    cv2.destroyAllWindows()
