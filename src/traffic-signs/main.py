import cv2
import numpy as np

BLUE_NUM = 1
RED_NUM = 2
PURPLE_NUM = 3

FONT = cv2.FONT_HERSHEY_COMPLEX # только этот шрифт содержит русские буквы
GREEN = (0,255,0)

def bin(img, color: int):
    if color == BLUE_NUM:
        # hsv=(103,193,208) # bgr=(206,145,52)
        lower = np.array([103-20,50,50])
        upper = np.array([103+20,255,255])
        img = cv2.inRange(img, lower, upper)
        # img = cv2.inRange(img, (200,0,0), (220,170,170))
        # img = cv2.inRange(img, (80,150,150), (150,230,230))
    elif color == RED_NUM:
        # hsv=(3,218,188) # bgr=(28,42,188)
        lower = np.array([0,50,50])
        upper = np.array([3+20,255,255])
        img = cv2.inRange(img, lower, upper)
        # img = cv2.inRange(img, (0,155,155), (80,230,230))
    elif color == PURPLE_NUM:
        lower = np.array([140,50,50])
        upper = np.array([255,255,255])
        img = cv2.inRange(img, lower, upper)
    else:
        raise Exception('Неверный цвет для бинаризации', color)
    return img

def readSample(fileName, color: int):
    img = cv2.imread('samples/' + fileName + '.png')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    img = bin(img, color)

    conts = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    contIdx = 0
    conts = conts[contIdx] # контуры хранятся под индексом 0
    if not conts:
        return None
    
    conts = sorted(conts, key=cv2.contourArea, reverse=True)
    # cv2.drawContours(img, conts, contIdx, (255,0,255), 3)

    (x,y,w,h) = cv2.boundingRect(conts[contIdx])
    img = img[y:y+h, x:x+w]

    img = cv2.resize(img, (64,64))
    cv2.imshow(fileName, img)
    return (img, fileName, color)

def process(img, color, samples):
    mask = bin(img, color)
    # cv2.imshow("mask", mask)

    # очищаем маску от шумов
    # mask = cv2.erode(mask, None, iterations=2)
    # mask = cv2.dilate(mask, None, iterations=4)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, (15,15)) # убирает белые точки шума на черном фоне
    # mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, (5,5)) # убирает черные отверстия в белых объектах
    cv2.imshow("cleared", mask)

    conts = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    conts = conts[0] # контуры хранятся под индексом 0
    res = []
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

        for sample in samples:
            sum = 0
            for i in range(64):
                for j in range(64):
                    if sample[0][i][j] == sign[i][j]:
                        sum += 1
            res.append([sample[1], sum, cont, (x,y,w,h)])
    return res

sampleLeft = readSample('left', BLUE_NUM)
sampleRight = readSample('right', BLUE_NUM)
sampleStop = readSample('stop', RED_NUM)
sampleAmox = readSample('amox', PURPLE_NUM)

cap = cv2.VideoCapture(0)
while (True):
    ret, img = cap.read()
    # img = cv2.imread("all.png")
    # img = cv2.imread("left-for-detect.png")
    # cv2.imshow('original', img)
    imgOrig = img.copy()
    img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    img = cv2.GaussianBlur(img, (19, 19), 0)

    weights = []
    weights.extend(process(img, BLUE_NUM, [sampleLeft, sampleRight]))
    weights.extend(process(img, RED_NUM, [sampleStop]))
    weights.extend(process(img, PURPLE_NUM, [sampleAmox]))
    for item in weights:
        if item[1] > 3000:
            cv2.drawContours(imgOrig, item[2], -1, (255,0,255), 2)
            (x,y,w,h) = item[3]
            cv2.rectangle(imgOrig, (x,y), (x+w,y+h), GREEN, 2)
            cv2.putText(imgOrig, item[0], (x,y-10), FONT, fontScale=1, color=GREEN)
            print(item[0])
    # print(weights)
    cv2.imshow('original', imgOrig)

    if cv2.waitKey(1)==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
