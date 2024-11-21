import cv2
import const

class SignSample(object):
    name = ''
    img = None
    binColor = ''

    def __init__(self, img, name, binColor):
        self.img = img
        self.name = name
        self.binColor = binColor

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

class SignDetector(object):
    MIN_CONTOUR_AREA = 5000
    MIN_ACCURACY = 60

    BLUE_COLOR = 'blue'
    RED_COLOR = 'red'
    PURPLE_COLOR = 'purple'

    sampleLeft = None
    sampleRight = None
    sampleStop = None
    # sampleAmox = None

    enabled = False

    def __init__(self):
        self.sampleLeft = self.readSample('left', self.BLUE_COLOR)
        self.sampleRight = self.readSample('right', self.BLUE_COLOR)
        self.sampleStop = self.readSample('stop', self.RED_COLOR)
        # self.sampleAmox = self.readSample('amox', self.PURPLE_COLOR)

    def switch(self):
        self.enabled = not self.enabled
        if self.enabled:
            print("Распознавание знаков включено")
        else:
            print("Распознавание знаков выключено")

    def bin(self, img, color, isSample: bool):
        if color == self.BLUE_COLOR:
            # hsv=(103,193,208) # bgr=(206,145,52)
            if isSample:
                img = cv2.inRange(img, (103-20,50,50), (103+20,255,255))
            else:
                img = cv2.inRange(img, (105,50,50), (116,240,240))
        elif color == self.RED_COLOR:
            # hsv=(3,218,188) # bgr=(28,42,188)
            if isSample:
                img = cv2.inRange(img, (0,50,50), (3+20,255,255))
            else:
                img = cv2.inRange(img, (125,75,75), (179,205,170))
        elif color == self.PURPLE_COLOR:
            img = cv2.inRange(img, (140,50,50), (255,255,255))
        else:
            raise Exception('Неверный цвет для бинаризации', color)
        return img

    def readSample(self, fileName, color):
        img = cv2.imread('samples/' + fileName + '.png')
        img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        img = self.bin(img, color, isSample=True)

        conts = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        conts = conts[0] # контуры хранятся под индексом 0
        if not conts:
            return None
        
        cont = max(conts, key=cv2.contourArea) # самый длинный и интересный
        # cv2.drawContours(img, cont, -1, (255,0,255), 3)

        (x,y,w,h) = cv2.boundingRect(cont)
        img = img[y:y+h, x:x+w]

        img = cv2.resize(img, (64,64))
        # cv2.imshow(fileName, img)
        return SignSample(img, fileName, color)

    def detect(self, img, color, samples):
        mask = self.bin(img, color, isSample=False)

        # очищаем маску от шумов
        mask = cv2.erode(mask, (9,9), iterations=3)
        # mask = cv2.dilate(mask, None, iterations=4)
        # mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, (15,15)) # убирает белые точки шума на черном фоне
        # mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, (5,5)) # убирает черные отверстия в белых объектах
        # cv2.imshow(str(color) + ' mask', mask)

        conts = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        conts = conts[0] # контуры хранятся под индексом 0
        finalRes = []
        if conts:
            cont = max(conts, key=cv2.contourArea) # самый длинный и интересный
            # conts = sorted(conts, key=cv2.contourArea, reverse=True)
            # cont = conts[0] # 0-й контур - самый длинный и интересный

            area = cv2.contourArea(cont)
            # print(area)
            if area > self.MIN_CONTOUR_AREA:
                (x,y,w,h) = cv2.boundingRect(cont) # рамка с найденным знаком

                sign = mask[y:y+h, x:x+w]
                sign = cv2.resize(sign, (64,64))

                res = []
                for sample in samples:
                    sum = 0
                    for i in range(64):
                        for j in range(64):
                            if sample.img[i][j] == sign[i][j]:
                                sum += 1
                    accuracy = sum*100//4096
                    if accuracy >= self.MIN_ACCURACY:
                        res.append(Recognized(sample.name, accuracy, cont, (x,y,w,h)))
                if res:
                    finalRes.append(max(res, key=lambda x: x.accuracy))
        return finalRes

    def feed(self, img):
        if not self.enabled:
            return img
        
        imgForAnalyze = img.copy()
        imgForAnalyze = cv2.cvtColor(imgForAnalyze, cv2.COLOR_BGR2HSV)
        imgForAnalyze = cv2.GaussianBlur(imgForAnalyze, (19, 19), 0)

        detected = []
        detected.extend(self.detect(imgForAnalyze, self.BLUE_COLOR, [self.sampleLeft, self.sampleRight]))
        detected.extend(self.detect(imgForAnalyze, self.RED_COLOR, [self.sampleStop]))
        # detected.extend(detect(imgForAnalyze, PURPLE_COLOR, [sampleAmox]))

        for item in detected:
            cv2.drawContours(img, item.contour, -1, (255,0,255), 2)
            (x,y,w,h) = item.rect
            cv2.rectangle(img, (x,y), (x+w,y+h), const.BLUE, 2)
            title = item.name + ' ' + str(item.accuracy) + '%'
            cv2.putText(img, title, (x,y-10), const.FONT, fontScale=1, color=const.BLUE)
            # print(title)
        return img
