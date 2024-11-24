import cv2
from datetime import datetime, timedelta
import const

class Timer(object):
    enabled = False
    paused = False
    targetTime = None
    timeLeft = None

    def start(self, sec = 600):
        self.clear()
        self.targetTime = datetime.now() + timedelta(seconds=sec+1)
        self.enabled = True
    
    def hide(self):
        self.clear()
        self.enabled = False

    def clear(self):
        self.targetTime = None
        self.timeLeft = None
        self.paused = False

    def draw(self, img):
        if not self.enabled:
            return

        currentTimeLeft = self.getTimeLeft()
        if currentTimeLeft == None:
            return

        min = currentTimeLeft // 60
        sec = currentTimeLeft % 60

        if min < 1:
            color = const.RED
        elif min < 5:
            color = const.YELLOW
        else:
            color = const.GREEN

        timerMinSec = str(min).zfill(2) + ":" + str(sec).zfill(2)
        cv2.putText(img, timerMinSec, (const.x(367), const.y(27)), const.FONT, 0.5, color)

    def getTimeLeft(self):
        if not self.enabled:
            return None
        
        if self.paused:
            return self.timeLeft
        elif self.targetTime != None:
            return int((self.targetTime - datetime.now()).total_seconds())
        else:
            return None

    def switchPause(self, pause: bool = None):
        if not self.enabled:
            return

        newPaused = not self.paused
        if pause != None:
            newPaused = pause

        if newPaused == self.paused:
            return
        
        if newPaused:
            self.timeLeft = self.getTimeLeft()
            self.paused = True
        else:
            self.start(self.timeLeft)
            self.paused = False
