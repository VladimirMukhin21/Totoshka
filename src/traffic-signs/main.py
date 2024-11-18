import cv2
import numpy as np

BLUE_NUM = 1
RED_NUM = 2
PURPLE_NUM = 3

FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
GREEN = (0,255,0)
BLUE = (255,0,0)

class SignSample(object):
    name = ''
    img = None
    binNum = -1

    def __init__(self, img, name, binNum):
        self.img = img
        self.name = name
        self.binNum = binNum

class Recognized(object):
    name = ''
    accuracy = 0
    contour = []
    rect = []

    def __init__(self, name, accuracy, contour, rect):
        self.name = name
        self.accuracy = accuracy
        self.contour = contour
        self.rect = rect

def bin(img, color: int, isSample: bool):
    if color == BLUE_NUM:
        # hsv=(103,193,208) # bgr=(206,145,52)
        if isSample:
            img = cv2.inRange(img, (103-20,50,50), (103+20,255,255))
        else:
            img = cv2.inRange(img, (105,50,50), (116,240,240))
    elif color == RED_NUM:
        # hsv=(3,218,188) # bgr=(28,42,188)
        if isSample:
            img = cv2.inRange(img, (0,50,50), (3+20,255,255))
        else:
            img = cv2.inRange(img, (125,75,75), (179,205,170))
    elif color == PURPLE_NUM:
        img = cv2.inRange(img, (140,50,50), (255,255,255))
    else:
        raise Exception('Неверный цвет для бинаризации', color)
    return img

def readSample(fileName, color: int):
    img = cv2.imread('samples/' + fileName + '.png')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    img = bin(img, color, isSample=True)

    conts = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    conts = conts[0] # контуры хранятся под индексом 0
    if not conts:
        return None
    
    cont = max(conts, key=cv2.contourArea) # самый длинный и интересный
    # cv2.drawContours(img, cont, -1, (255,0,255), 3)

    (x,y,w,h) = cv2.boundingRect(cont)
    img = img[y:y+h, x:x+w]

    img = cv2.resize(img, (64,64))
    cv2.imshow(fileName, img)
    return SignSample(img, fileName, color)

def process(img, color, samples):
    mask = bin(img, color, isSample=False)
    # cv2.imshow("mask", mask)

    # очищаем маску от шумов
    mask = cv2.erode(mask, (9,9), iterations=3)
    # mask = cv2.dilate(mask, None, iterations=4)
    # mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, (15,15)) # убирает белые точки шума на черном фоне
    # mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, (5,5)) # убирает черные отверстия в белых объектах
    cv2.imshow("cleared " + str(color), mask)

    conts = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    conts = conts[0] # контуры хранятся под индексом 0
    finalRes = []
    if conts:
        cont = max(conts, key=cv2.contourArea) # самый длинный и интересный
        # conts = sorted(conts, key=cv2.contourArea, reverse=True)
        # cont = conts[0] # 0-й контур - самый длинный и интересный
        (x,y,w,h) = cv2.boundingRect(cont) # рамка с найденным знаком

        sign = mask[y:y+h, x:x+w]
        sign = cv2.resize(sign, (64,64))

        # cv2.drawContours(img, cont, -1, (255,0,255), 3)
        # cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 3)
        # cv2.imshow("cont", img)
        # cv2.imshow("sign", sign)

        res = []
        for sample in samples:
            sum = 0
            for i in range(64):
                for j in range(64):
                    if sample.img[i][j] == sign[i][j]:
                        sum += 1
            accuracy = sum*100//4096
            res.append(Recognized(sample.name, accuracy, cont, (x,y,w,h)))
        finalRes.append(max(res, key=lambda x: x.accuracy))
    return finalRes

sampleLeft = readSample('left', BLUE_NUM)
sampleRight = readSample('right', BLUE_NUM)
sampleStop = readSample('stop', RED_NUM)
# sampleAmox = readSample('amox', PURPLE_NUM)

cap = cv2.VideoCapture(1)
while (True):
    ret, img = cap.read()
    # img = cv2.imread("all.png")
    # img = cv2.imread("left-for-detect.png")
    # cv2.imshow('original', img)
    imgOrig = img.copy()
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    img = cv2.GaussianBlur(img, (19, 19), 0)

    processed = []
    processed.extend(process(img, BLUE_NUM, [sampleLeft, sampleRight]))
    processed.extend(process(img, RED_NUM, [sampleStop]))
    # processed.extend(process(img, PURPLE_NUM, [sampleAmox]))

    for item in processed:
        if item.accuracy > 50:
            cv2.drawContours(imgOrig, item.contour, -1, (255,0,255), 2)
            (x,y,w,h) = item.rect
            cv2.rectangle(imgOrig, (x,y), (x+w,y+h), BLUE, 2)
            title = item.name + ' ' + str(item.accuracy) + '%'
            cv2.putText(imgOrig, title, (x,y-10), FONT, fontScale=1, color=BLUE)
            print(title)
    cv2.imshow('original', imgOrig)

    if cv2.waitKey(1)==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
