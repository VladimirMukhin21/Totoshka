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
import const
from enabledModes import EnabledModes
from timer import Timer
from control import RemoteControl
from autoLine import AutoLine
from signDetector import SignDetector
from roadTraffic import RoadTraffic

CAMERA_NUM = 0 #1 + cv2.CAP_FFMPEG # номер камеры
QR_OFF = ""
QR_CV2 = "qrcv"
QR_PYZBAR = "qrpy"
EMPTY_STR = ""

enabledModes = EnabledModes()
timer = Timer()
remCtrl = RemoteControl()
autoLine = AutoLine()
signDetector = SignDetector()
roadTraffic = RoadTraffic(signDetector, remCtrl)

def beep():
    frequency = 2500
    duration = 500
    t = Thread(target=winsound.Beep, args=(frequency, duration))
    t.start()

def qrDecode(image):
    if qrMode == QR_CV2:
        cv2Decode(image)
    elif qrMode == QR_PYZBAR:
        pyzbarDecode(image)
    else:
        return

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
        image = cv2.line(image, pyzbar_box.polygon[i], pyzbar_box.polygon[(i+1) % n_points], const.RED, thickness=3)
    cv2.putText(image, qr, (pyzbar_box.rect.left, pyzbar_box.rect.top-10), const.FONT, 1, const.RED)

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
    cv2.line(image, (int(cv2_box[0][0]), int(cv2_box[0][1])), (int(cv2_box[1][0]), int(cv2_box[1][1])), const.RED, thickness)
    cv2.line(image, (int(cv2_box[1][0]), int(cv2_box[1][1])), (int(cv2_box[2][0]), int(cv2_box[2][1])), const.RED, thickness)
    cv2.line(image, (int(cv2_box[2][0]), int(cv2_box[2][1])), (int(cv2_box[3][0]), int(cv2_box[3][1])), const.RED, thickness)
    cv2.line(image, (int(cv2_box[3][0]), int(cv2_box[3][1])), (int(cv2_box[0][0]), int(cv2_box[0][1])), const.RED, thickness)
    x = int(min(cv2_box[0][0], cv2_box[1][0], cv2_box[2][0], cv2_box[3][0]))
    y = int(min(cv2_box[0][1], cv2_box[1][1], cv2_box[2][1], cv2_box[3][1]))
    cv2.putText(image, qr, (x, y-10), const.FONT, 1, const.RED)

def draw_guides(image):
    thickness=1
    if guideMode == 1:
        cv2.line(image, (const.x(190), const.y(280)), (const.x(190), const.y(20)), const.BLUE, thickness) # центр
        cv2.line(image, (const.x(131), const.y(170)), (const.x(154), const.y(120)), const.GREEN, thickness) # лев
        cv2.line(image, (const.x(249), const.y(170)), (const.x(227), const.y(120)), const.GREEN, thickness) # прав
        cv2.line(image, (const.x(100), const.y(170)), (const.x(280), const.y(170)), const.GREEN, thickness) # горизонт
    elif guideMode == 2:
        cv2.line(image, (const.x(247), const.y(300)), (const.x(227), const.y(240)), const.GREEN, thickness) # центр
        cv2.line(image, (const.x(112), const.y(300)), (const.x(150), const.y(230)), const.GREEN, thickness) # лев
        cv2.line(image, (const.x(386), const.y(300)), (const.x(300), const.y(230)), const.GREEN, thickness) # прав
        cv2.line(image, (const.x(142), const.y(297)), (const.x(351), const.y(264)), const.BLUE, thickness) # горизонт
    elif guideMode == 5:
        for i in range(0, 400, 100):
            cv2.line(image, (const.x(i), const.y(0)), (const.x(i), const.y(300)), const.GREEN, thickness)
        for i in range(0, 300, 100):
            cv2.line(image, (const.x(0), const.y(i)), (const.x(400), const.y(i)), const.GREEN, thickness)

def draw_captured_qrs(image):
    if not showCapturedQrs:
        return
    if len(capturedQrs) == 0:
        return
    size = 0.7
    index = 0
    step = 15
    cv2.putText(image, "QR-коды:", (const.x(5), const.y((index+1)*step)), const.FONT, size, const.GREEN)
    for qr in capturedQrs:
        index += 1
        text = str.format("{0}. {1}", index, qr)
        cv2.putText(image, text, (const.x(5), const.y((index+1)*step)), const.FONT, size, const.GREEN)

def draw_telemetry(image):
    readed, dist, clrl, clrr = remCtrl.readTelemetry()
    if readed:
        size = 0.6
        firstStr = 100
        step = 15
        cv2.putText(image, "Датчики:", (const.x(333), const.y(firstStr)), const.FONT, size, const.GREEN)
        cv2.putText(image, "dist", (const.x(330), const.y(firstStr+step)), const.FONT, size, const.GREEN)
        cv2.putText(image, str(dist), (const.x(365), const.y(firstStr+step)), const.FONT, size, const.GREEN)
        cv2.putText(image, "clrl", (const.x(330), const.y(firstStr+2*step)), const.FONT, size, const.GREEN)
        cv2.putText(image, str(clrl), (const.x(365), const.y(firstStr+2*step)), const.FONT, size, const.GREEN)
        cv2.putText(image, "clrr", (const.x(330), const.y(firstStr+3*step)), const.FONT, size, const.GREEN)
        cv2.putText(image, str(clrr), (const.x(365), const.y(firstStr+3*step)), const.FONT, size, const.GREEN)

def connect_camera(num):
    print("Подключение камеры " + str(num) + "...")
    cap.release()
    CAMERA_NUM = num - 1
    cap.open(CAMERA_NUM)
    print("Камера " + str(num) + " подключена")

def switch_record():
    global file
    global recording
    global timer
    recording = not recording
    if recording:
        timer.start()
        name = datetime.now().strftime("%Y%m%d_%H%M%S") + ".avi"
        codec = cv2.VideoWriter_fourcc(*'DIVX')
        # codec = cv2.VideoWriter_fourcc(*'XVID')
        # codec = cv2.VideoWriter_fourcc(*'MJPG') # файл больше раза в 2
        file = cv2.VideoWriter(name, codec, 25, (const.width, const.height))
    else:
        timer.switchPause(True)
        file.release()

def record(image):
    if recording:
        file.write(image)
        cv2.circle(image, (const.x(390), const.y(10)), radius=3, color=const.RED, thickness=-1)

HELP = []
HELP.append("shift-1/2/3: выбор камеры")
HELP.append("shift-0: запись в файл")
HELP.append("q: захваченные QR-коды")
HELP.append("w/e/r: захват QR cv2/pyzbar/выкл")
HELP.append("1/2/3: гайдлайны верх/нижн/выкл")
HELP.append("l: датчики")
HELP.append("a: просмотр линии")
HELP.append("s: автолиния")
HELP.append("d: дорожные знаки")
HELP.append("*: таймер")
HELP.append("p: пауза таймера")
HELP.append("h: справка")
HELP.append("esc: завершить работу")

def draw_help(image):
    if not showHelp:
        return
    cv2.rectangle(
        image,
        (const.x(0), const.y(10)),
        (const.x(310), const.y(290)),
        color = const.GREEN,
        thickness = -1)
    for i in range(len(HELP)):
        cv2.putText(image, HELP[i], (const.x(5), const.y(10+(i+1)*20)), const.FONT, 0.7, const.BLACK)

def draw_scale(image):
    now = datetime.now()
    delta = now - scaleChangedTime
    if (delta.total_seconds() > 1):
        return
    msg = str(scale) + "%"
    cv2.putText(image, msg, (const.x(180), const.y(140)), const.FONT, 1, const.GREEN)

def change_scale(delta):
    global scale
    global size
    global scaleChangedTime
    scale += delta
    scale = max(scale, 30)  # ограничение минимального масштаба
    scale = min(scale, 200) # ограничение максимального масштаба
    size = (int(const.width*scale/100), int(const.height*scale/100))
    scaleChangedTime = datetime.now()
    print("scale:", scale, "%,  image size:", size[0], "x", size[1])

capturedQrs = set()
guideMode = 1  # 0
showCapturedQrs = True
scaleChangedTime = datetime.now()
recording = False
showHelp = False
cv2Decoder = cv2.QRCodeDetector()
qrMode = QR_OFF
qrDecodeTime = datetime.now()
cv2_box = []
pyzbar_box = []
qr = EMPTY_STR
now = int(datetime.now().strftime("%M%S"))


if __name__ == "__main__":
    print("Подключение камеры...")
    cap = cv2.VideoCapture()
    cap.open(CAMERA_NUM)
    print("Камера подключена")

    remCtrl.connect()

    const.width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    const.height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    scale = 130
    size = (const.width, const.height)
    change_scale(0)
    # print(width, "x", height)

    while True:
        readed, frame = cap.read()

        if readed:
            qrDecode(frame)
            autoLine.feed(frame, remCtrl)
            if roadTraffic.enabled:
                frame, _ = roadTraffic.feed(frame)
            elif signDetector.enabled:
                frame, _ = signDetector.feed(frame)
            
            # cv2.putText(frame, "Тотошка", (x(5),y(20)), font, 1, color=(0,255,0), thickness=1, lineType=cv2.LINE_AA)
            draw_guides(frame)
            draw_captured_qrs(frame)
            draw_telemetry(frame)
            draw_help(frame)
            draw_scale(frame)
            timer.draw(frame)
            enabledModes.show(frame)
            record(frame)
            frame = cv2.resize(frame, size, interpolation=cv2.INTER_LINEAR)
            cv2.imshow("TOTOSHKA", frame)
        else:
            cap.release()
            cap.open(CAMERA_NUM)
             
        key = cv2.waitKey(1)
        if key >= ord("0") and key <= ord("9"):
            guideMode = key - ord("0")
            # print("guideMode:", guideMode)
        elif key == ord("+"):
            change_scale(10)
        elif key == ord("-"):
            change_scale(-10)
        elif key == ord("q"):
            showCapturedQrs = not showCapturedQrs
        elif key == ord("w"):
            enabledModes.switch(qrMode, False)
            qrMode = QR_CV2
            enabledModes.switch(qrMode, True)
            qr = EMPTY_STR
        elif key == ord("e"):
            enabledModes.switch(qrMode, False)
            qrMode = QR_PYZBAR
            enabledModes.switch(qrMode, True)
            qr = EMPTY_STR
        elif key == ord("r"):
            enabledModes.switch(qrMode, False)
            qrMode = QR_OFF
            qr = EMPTY_STR
        elif key == ord("l"):
            remCtrl.telemetrySwitch()
        elif key == 42: # shift-8
            if timer.enabled:
                timer.hide()
            else:
                timer.start()
        elif key == ord("p"):
            timer.switchPause()
        elif key == 41: # shift-0
            switch_record()
        elif key == ord("h"):
            showHelp = not showHelp
        elif key == ord("!"): # shift-1
            connect_camera(1)
        elif key == ord("@"): # shift-2
            connect_camera(2)
        elif key == ord("#"): # shift-3
            connect_camera(3)
        elif key == 27: # Esc => exit
            break
        elif key == ord("a"):
            autoLine.switchPreview()
        elif key == ord("s"):
            enabledModes.switch('line')
            autoLine.switchDrive()
        elif key == ord("d"):
            enabledModes.switch('sign')
            signDetector.switch()
        # elif key == ord("f"):
        #     roadTraffic.switch()
        elif key != -1:
            print("Нажата клавиша с кодом " + str(key) + ", действие не задано")

    if recording:
        file.release()
    remCtrl.disconnect()
    cap.release()
    cv2.destroyAllWindows()
