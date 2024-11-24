import numpy as np
import cv2
from datetime import datetime

class AutoLine(object):
    lastDetectTime = datetime.now()
    previewEnabled = False
    driveEnabled = False

    def switchPreview(self):
        self.previewEnabled = not self.previewEnabled
        if self.previewEnabled:
            print("Просмотр линии включен")
        else:
            self.destroyWindows()
            print("Просмотр линии выключен")

    def switchDrive(self):
        self.driveEnabled = not self.driveEnabled
        if self.driveEnabled:
            print("Езда по линии включена")
        else:
            self.previewEnabled = False
            self.destroyWindows()
            print("Езда по линии выключена")

    def destroyWindows(self):
            try:
                cv2.destroyWindow('lineMask')
                # cv2.destroyWindow('gray')
                # cv2.destroyWindow('blur')
                # cv2.destroyWindow('warp')
                # cv2.destroyWindow('tresh1')
            except:
                pass

    def warpImg(self, img, points, w, h):
        pts1 = np.float32(points)
        pts2 = np.float32([[0,0], [w,0], [0,h], [w,h]])
        matrix = cv2.getPerspectiveTransform(pts1, pts2)
        imgWarp = cv2.warpPerspective(img, matrix, (w, h))
        return imgWarp

    def getHist(self, img, minPer=0.1):
        histValues = np.sum(img, axis=0)
        # print(histValues)
        maxValue = np.max(histValues)
        minValue = minPer * maxValue
        indexArray = np.where(histValues >= minValue)
        basePoint = int(np.average(indexArray))
        # print(basePoint)
        return basePoint

    def feed(self, img, remCtrl):
        if (not self.previewEnabled) and (not self.driveEnabled):
            return
        
        now = datetime.now()
        delta = now - self.lastDetectTime
        if (delta.microseconds > 150000): # 150ms
            self.lastDetectTime = now

            h, w, _ = img.shape

            img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            # cv2.imshow('gray', img)

            img = cv2.GaussianBlur(img, (19, 19), 0)
            # cv2.imshow('blur', img)

            # persp = 200
            # points = [[0+persp,0], [w-persp,0], [0,h], [w,h]]
            # img = cv2.circle(img, points[0], 10, (0,0,0), cv2.FILLED)
            # img = cv2.circle(img, points[1], 10, (0,0,0), cv2.FILLED)
            # img = cv2.circle(img, points[2], 10, (0,0,0), cv2.FILLED)
            # img = cv2.circle(img, points[3], 10, (0,0,0), cv2.FILLED)
            # cv2.imshow('blur', img)

            #img = self.warpImg(img, points, w, h)
            # cv2.imshow('warp', img)

            _, img = cv2.threshold(img, 117, 255, cv2.THRESH_BINARY_INV)
            # cv2.imshow('tresh1', img)

            # сужаем область поиска линии - обрезаем изображение
            #img = img[200:440, 0:w]  # [y1:y2, x1:x2]
            img = img[200:365, 10:w-10]  # [y1:y2, x1:x2]

            basePoint = self.getHist(img)
            img = cv2.rectangle(img, (0,0), (w,10), (0,0,0), cv2.FILLED)
            img = cv2.line(img, (basePoint, 2), (basePoint, 8), (255,255,255), 2)
            if self.previewEnabled:
                cv2.imshow('lineMask', img)
            print(basePoint)
            # basePoint - w//2 = 0 # целевое значение
            turn = int(np.interp(basePoint, [0,w], [0,255]))
            # turn = int(np.interp(basePoint, [0,w], [-127,127]))
            # turn = turn * 1.3
            # turn = max(turn, -127)
            # turn = min(turn, 127)
            # turn = int(np.interp(turn, [-127,127], [0,255]))
            # print(turn)
            if self.driveEnabled:
                remCtrl.sendDriveCommand(157, turn)
